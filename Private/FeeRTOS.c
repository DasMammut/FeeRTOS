#include "IFeeRTOS.h"

static TFeeRTOS_TaskHandle TaskListHead = NULL;
static TFeeRTOS_TaskHandle CurrentTask = NULL;
static TFeeRTOS_TaskHandle CallbackTaskHandle = NULL;

static volatile bool SchedulerRunning = false;
static volatile uint16_t SavedSP;
static volatile uint16_t MallocSavedSP;
static volatile bool ForcedYield = false;
static volatile uint16_t ForcedYieldSavedCNT = 0;

static void Schedule(void);

static TFeeRTOS_TaskHandle getNextTask(void);

static inline bool isReadyToRun(TFeeRTOS_TaskHandle task);

static void IdleTask(void* aUserData);

static void DelayCallback(void* args);

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
    t->BlockedFlags.Suspend = false;
    t->BlockedFlags.Delay = false;
    t->BlockedFlags.Semaphore = false;
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
    uint8_t byte;
    uint16_t pc = (uint16_t)(uintptr_t)aTaskFunction;
    byte = (uint8_t)(pc & 0xFF); // PC Low-Byte
    FeeRTOS_StackPush(t->Stack, &byte, 1);
    byte = (uint8_t)((pc >> 8) & 0xFF); // PC High-Byte
    FeeRTOS_StackPush(t->Stack, &byte, 1);

    // R1–R31 (UserData in R24:R25 — AVR Calling Convention)
    for (uint8_t r = 0; r <= 31 + 1; r++) {
        if (r == 1) // SREG // Interupts enabled
            byte = 0x80;
        else if (r == 24 + 1) // R24
            byte = (uint8_t)((uint16_t)(uintptr_t)aUserData & 0xFF);
        else if (r == 25 + 1) // R25
            byte = (uint8_t)(((uint16_t)(uintptr_t)aUserData >> 8) & 0xFF);
        else
            byte = 0x00;
        FeeRTOS_StackPush(t->Stack, &byte, 1);
    }
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

    CurrentTask = TaskListHead;
    SchedulerRunning = true;

    FeeRTOS_SetupTickTimer(TICK_RATE);

    FeeRTOS_ENTER_CRITICAL();

    SavedSP = (uint16_t)(uintptr_t)CurrentTask->Stack->StackPointer;

    asm volatile(
        "lds  r16, SavedSP      \n\t"
        "lds  r17, SavedSP+1    \n\t"
        "out  __SP_H__, r17     \n\t"
        "out  __SP_L__, r16     \n\t"

        "pop  r31 \n\t"  "pop  r30 \n\t"  "pop  r29 \n\t"  "pop  r28 \n\t"
        "pop  r27 \n\t"  "pop  r26 \n\t"  "pop  r25 \n\t"  "pop  r24 \n\t"
        "pop  r23 \n\t"  "pop  r22 \n\t"  "pop  r21 \n\t"  "pop  r20 \n\t"
        "pop  r19 \n\t"  "pop  r18 \n\t"  "pop  r17 \n\t"  "pop  r16 \n\t"
        "pop  r15 \n\t"  "pop  r14 \n\t"  "pop  r13 \n\t"  "pop  r12 \n\t"
        "pop  r11 \n\t"  "pop  r10 \n\t"  "pop  r9  \n\t"  "pop  r8  \n\t"
        "pop  r7  \n\t"  "pop  r6  \n\t"  "pop  r5  \n\t"  "pop  r4  \n\t"
        "pop  r3  \n\t"  "pop  r2  \n\t"  "pop  r1  \n\t"

        "pop  r0              \n\t"
        "out  __SREG__, r0    \n\t"

        "pop  r0              \n\t"

        "reti                 \n\t"
    );
    __builtin_unreachable(); // Compiler info dass diese Funktion nie zurückkehrt
}

void FeeRTOS_Yield(void) {
	if(!SchedulerRunning) return;
    FeeRTOS_ENTER_CRITICAL();
    ForcedYield = true;
    ForcedYieldSavedCNT = TCA0.SINGLE.CNT; // Counter sichern
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // Pending OVF clearen
    TCA0.SINGLE.CNT = TCA0.SINGLE.PER; // Counter auf PER -> OVF beim naechsten Timer-Takt
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
static void Schedule(void) {
    CurrentTask->Stack->StackPointer = (void*)(uintptr_t)SavedSP;

    if (ForcedYield) {
        TCA0.SINGLE.CNT = ForcedYieldSavedCNT; // Counter wiederherstellen
        ForcedYield = false;
    } 
    else{
        TickCount++;
        FeeRTOS_UpdateTimers();
    }

    CurrentTask = getNextTask();

    SavedSP = (uint16_t)(uintptr_t)CurrentTask->Stack->StackPointer;

    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

ISR(TCA0_OVF_vect, ISR_NAKED) {
    // Context Save
    asm volatile(
        "push r0              \n\t"
        "in   r0, __SREG__    \n\t"
        "push r0              \n\t"

        "push r1  \n\t"  "push r2  \n\t"  "push r3  \n\t"  "push r4  \n\t"
        "push r5  \n\t"  "push r6  \n\t"  "push r7  \n\t"  "push r8  \n\t"
        "push r9  \n\t"  "push r10 \n\t"  "push r11 \n\t"  "push r12 \n\t"
        "push r13 \n\t"  "push r14 \n\t"  "push r15 \n\t"  "push r16 \n\t"
        "push r17 \n\t"  "push r18 \n\t"  "push r19 \n\t"  "push r20 \n\t"
        "push r21 \n\t"  "push r22 \n\t"  "push r23 \n\t"  "push r24 \n\t"
        "push r25 \n\t"  "push r26 \n\t"  "push r27 \n\t"  "push r28 \n\t"
        "push r29 \n\t"  "push r30 \n\t"  "push r31 \n\t"

        "in   r16, __SP_L__   \n\t"
        "in   r17, __SP_H__   \n\t"
        "sts  SavedSP,   r16  \n\t"
        "sts  SavedSP+1, r17  \n\t"

        "call Schedule \n\t"

        // Context Restore
        "lds  r16, SavedSP    \n\t"
        "lds  r17, SavedSP+1  \n\t"
        "out  __SP_H__, r17   \n\t"
        "out  __SP_L__, r16   \n\t"

        "pop  r31 \n\t"  "pop  r30 \n\t"  "pop  r29 \n\t"  "pop  r28 \n\t"
        "pop  r27 \n\t"  "pop  r26 \n\t"  "pop  r25 \n\t"  "pop  r24 \n\t"
        "pop  r23 \n\t"  "pop  r22 \n\t"  "pop  r21 \n\t"  "pop  r20 \n\t"
        "pop  r19 \n\t"  "pop  r18 \n\t"  "pop  r17 \n\t"  "pop  r16 \n\t"
        "pop  r15 \n\t"  "pop  r14 \n\t"  "pop  r13 \n\t"  "pop  r12 \n\t"
        "pop  r11 \n\t"  "pop  r10 \n\t"  "pop  r9  \n\t"  "pop  r8  \n\t"
        "pop  r7  \n\t"  "pop  r6  \n\t"  "pop  r5  \n\t"  "pop  r4  \n\t"
        "pop  r3  \n\t"  "pop  r2  \n\t"  "pop  r1  \n\t"

        "pop  r0              \n\t"
        "out  __SREG__, r0    \n\t"
        "pop  r0              \n\t"

        "reti                 \n\t"
    );
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
    return !task->BlockedFlags.All; // Alle Blockade-Flags müssen 0 sein, damit der Task ready ist
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