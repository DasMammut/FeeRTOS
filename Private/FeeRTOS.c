#include "IFeeRTOS.h"

static TFeeRTOS_TaskHandle TaskListHead = NULL;
static TFeeRTOS_TaskHandle CurrentTask = NULL;
static TFeeRTOS_TaskHandle CallbackTaskHandle = NULL;

static volatile bool SchedulerRunning = false;
volatile uint16_t SavedSP; // Linker von Asm zu C, da SP in Asm direkt manipuliert wird
volatile bool ForcedYield = false;

static TFeeRTOS_TaskHandle getNextTask(void);

static inline bool isReadyToRun(TFeeRTOS_TaskHandle task);

static void IdleTask(void* aUserData); // IdleTask ist immer bereit, wenn kein anderer Task läuft, daher immer lowest prio

static void DelayCallback(void* args); // Callback für die Timer, damit sie den Task wieder freigeben können, wenn die Zeit abgelaufen ist

void FeeRTOS_Init(void) {
    FeeRTOS_CreateTask(IdleTask, NULL, IDLE_TASK_STACK_SIZE, TASK_PRIORITY_IDLE);
    CallbackTaskHandle = FeeRTOS_CreateTask(CallbackTask, NULL, CALLBACK_TASK_STACK_SIZE, TASK_PRIORITY_CALLBACK);
}

TFeeRTOS_TaskHandle FeeRTOS_CreateTask(void (*aTaskFunction)(void* aUserData), void* aUserData, uint16_t StackSize, TTaskPriority Priority) {
    if(aTaskFunction == NULL) return NULL;
    TFeeRTOS_TaskHandle t = (TFeeRTOS_TaskHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Task));
    if (t == NULL) return NULL;

    t->TaskFunction = aTaskFunction;
    t->UserData = aUserData;
    t->Stack = FeeRTOS_CreateStack(StackSize);
    t->nextTask = NULL;
    t->BlockedFlags.All = 0;
    t->DelayTimer = NULL;
    t->Priority = Priority;
    t->BasePriority = Priority; // Für Prioritätsvererbung
    t->nextWaiting = NULL;
    if (t->Stack == NULL) {
        FeeRTOS_Free(t);
        return NULL;
    }

    FeeRTOS_ENTER_CRITICAL();
    if (TaskListHead == NULL) {
        TaskListHead = t;
    } 
    else {
        TFeeRTOS_TaskHandle current = TaskListHead;
        while (current->nextTask != NULL) {
            current = current->nextTask;
        }
        current->nextTask = t;
    }

    // Initialen Kontext auf den Stack legen (simuliert ISR-Entry + Context Save)
    Port_InitializeStack(t->Stack, aTaskFunction, aUserData);

    FeeRTOS_EXIT_CRITICAL();
    return t;
}

void FeeRTOS_DeleteTask(TFeeRTOS_TaskHandle aTaskHandle) {
    if (aTaskHandle == NULL) return;

    FeeRTOS_ENTER_CRITICAL();

    if(aTaskHandle->DelayTimer != NULL) {
        FeeRTOS_DeleteTimer(aTaskHandle->DelayTimer);
        aTaskHandle->DelayTimer = NULL;
    }

    FeeRTOS_DestroyStack(aTaskHandle->Stack);

    bool isCurrentTask = (aTaskHandle == CurrentTask);

    if (aTaskHandle == TaskListHead) {
        TaskListHead = aTaskHandle->nextTask;
    } 
    else {
        TFeeRTOS_TaskHandle previous = TaskListHead;
        while (previous->nextTask != aTaskHandle) {
            previous = previous->nextTask;
        }
        previous->nextTask = aTaskHandle->nextTask;
    }

    FeeRTOS_Free(aTaskHandle);

    FeeRTOS_EXIT_CRITICAL();
    if(isCurrentTask) {
        FeeRTOS_Yield();
    }
}

void FeeRTOS_StartScheduler(void) {
    if(TaskListHead == NULL) return;

    CurrentTask = TaskListHead; // getNextTask() um von anfang an nach höchster Prio zu Schedulen
    CurrentTask = getNextTask();
    SchedulerRunning = true;

    FeeRTOS_SetupTickTimer(TICK_RATE);

    FeeRTOS_ENTER_CRITICAL();

    SavedSP = (uint16_t)(uintptr_t)CurrentTask->Stack->StackPointer;

    asm volatile("jmp Port_RestoreContext"); // Direkter Jump zu Port_restoreContext (Stack wird von ISR-Entry simuliert)

    __builtin_unreachable(); // Compiler info dass diese Funktion nie zurückkehrt
}

void FeeRTOS_Yield(void) {
	if(!SchedulerRunning) return;
    FeeRTOS_ENTER_CRITICAL();
    ForcedYield = true;

    Port_Yield();
    
    FeeRTOS_EXIT_CRITICAL();
    asm volatile("nop"); // Buffer für den Timer-Interrupt
    asm volatile("nop");
}

void FeeRTOS_Delay(uint16_t aDelayMs) {
    FeeRTOS_ENTER_CRITICAL();
    CurrentTask->DelayTimer = FeeRTOS_CreateTimer(aDelayMs, DelayCallback, CurrentTask, false);
    if(CurrentTask->DelayTimer == NULL) {
        FeeRTOS_EXIT_CRITICAL();
        return; // malloc fehlgeschlagen, nicht blockieren
    }
    CurrentTask->BlockedFlags.Delay = true;
    FeeRTOS_EXIT_CRITICAL();
    FeeRTOS_Yield();
}

void FeeRTOS_SuspendTask(TFeeRTOS_TaskHandle aTaskHandle){
    if(aTaskHandle == NULL) return;

    FeeRTOS_ENTER_CRITICAL();
    aTaskHandle->BlockedFlags.Suspend = true;
    FeeRTOS_EXIT_CRITICAL();
    if(aTaskHandle == CurrentTask) FeeRTOS_Yield();
}

void FeeRTOS_ResumeTask(TFeeRTOS_TaskHandle aTaskHandle){
	if(aTaskHandle == NULL) return;

    FeeRTOS_ENTER_CRITICAL();
    aTaskHandle->BlockedFlags.Suspend = false;
    FeeRTOS_EXIT_CRITICAL();
}

TFeeRTOS_TaskHandle getCurrentTask(void) {
    return CurrentTask;
}

TFeeRTOS_TaskHandle getCallbackTask(void) {
    return CallbackTaskHandle;
}

__attribute__((used))
void Schedule(void) {
    Port_Schedule(); 

    if (!ForcedYield) {
        TickCount++;
        FeeRTOS_UpdateTimers();
    } 
    else {
        ForcedYield = false;
    }

    CurrentTask->Stack->StackPointer = (void*)(uintptr_t)SavedSP;

    CurrentTask = getNextTask();

    SavedSP = (uint16_t)(uintptr_t)CurrentTask->Stack->StackPointer;
}

static TFeeRTOS_TaskHandle getNextTask(void) {
    TFeeRTOS_TaskHandle temp = TaskListHead;
    TTaskPriority highestPrioFound = TASK_PRIORITY_IDLE;

    // 1. Finde die aktuell höchste Priorität unter allen READY Tasks
    while(temp != NULL) {
        if(isReadyToRun(temp)) {
            if(temp->Priority > highestPrioFound) {
                highestPrioFound = temp->Priority;
            }
        }
        temp = temp->nextTask;
    }

    // 2. Entscheidungs-Logik:
    // Fall A: Es gibt einen Task mit HÖHERER Prio als CurrentTask
    // Fall B: CurrentTask ist nicht mehr Ready (muss also sowieso weg)
    if (highestPrioFound > CurrentTask->Priority || !isReadyToRun(CurrentTask)) {
        // Suche den ERSTEN Task mit dieser Prio (von vorne)
        temp = TaskListHead;
        while(temp != NULL) {
            if(isReadyToRun(temp) && temp->Priority == highestPrioFound) {
                return temp;
            }
            temp = temp->nextTask;
        }
    } 
    // Fall C: Round-Robin (Gleiche Prio wie CurrentTask)
    else {
        // Wir starten die Suche direkt NACH dem aktuellen Task
        temp = CurrentTask->nextTask;
        if(temp == NULL) temp = TaskListHead;

        while(temp != CurrentTask) {
            if(isReadyToRun(temp) && temp->Priority == highestPrioFound) {
                return temp;
            }
            temp = temp->nextTask;
            if(temp == NULL) temp = TaskListHead;
        }
    }

    // Wenn nichts gefunden wurde oder CurrentTask der einzige Ready-Task mit höchster Prio ist
    return CurrentTask;
}

static inline bool isReadyToRun(TFeeRTOS_TaskHandle task) {
    return task->BlockedFlags.All == 0; // Alle Blockade-Flags müssen 0 sein, damit der Task ready ist
}

static void IdleTask(void* aUserData) {
    (void)aUserData;
    while (1) {
        if(TaskListHead->nextTask == NULL) asm volatile("sleep"); // asm volatile("nop");
        // else FeeRTOS_Yield(); // Wenn es einen anderen Task gibt, yielden
		TFeeRTOS_TaskHandle temp = TaskListHead;
        while(temp != NULL){
            if(isReadyToRun(temp) && temp != CurrentTask) {
                FeeRTOS_Yield(); // Wenn es einen anderen aktiven Task gibt, yielden
                break;
            }
            temp = temp->nextTask;
        }
    }
}

static void DelayCallback(void* args) {
    TFeeRTOS_TaskHandle task = (TFeeRTOS_TaskHandle)args;
    FeeRTOS_ENTER_CRITICAL();
    task->BlockedFlags.Delay = false;
    FeeRTOS_DeleteTimer(task->DelayTimer);
    task->DelayTimer = NULL;
    FeeRTOS_EXIT_CRITICAL();
}