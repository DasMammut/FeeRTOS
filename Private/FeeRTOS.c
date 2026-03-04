#include "IFeeRTOS.h"

static TTaskInternal* TaskListHead = NULL;
static TTaskInternal* CurrentTask = NULL;

static volatile bool SchedulerRunning = false;
static volatile uint16_t SavedSP;
static volatile uint16_t MallocSavedSP;
static volatile bool ForcedYield = false;
static volatile uint16_t ForcedYieldSavedCNT = 0;

static void Schedule(void);

static TTaskInternal* getNextTask(void);

static inline bool isReadyToRun(TTaskInternal* task);

static void IdleTask(void* aUserData);


void FeeRTOS_Init(void) {
    TTaskConfig idleTaskConfig = {
        .TaskFunction = IdleTask,
        .UserData     = NULL,
        .StackSize    = IDLE_STACK_SIZE,
        .NameID       = IDLE_TASK_NAME
    };
    FeeRTOS_CreateTask(&idleTaskConfig);
}

void FeeRTOS_CreateTask(TTaskConfig* aTaskConfig) {
    if(aTaskConfig == NULL) return;
    TTaskInternal* t = (TTaskInternal*)FeeRTOS_Malloc(sizeof(TTaskInternal));
    if (t == NULL) return;

    t->TaskFunction = aTaskConfig->TaskFunction;
    t->UserData = aTaskConfig->UserData;
    strncpy(t->NameID, aTaskConfig->NameID, 15);
    t->NameID[15] = '\0';
    t->Stack = Stack_Create(aTaskConfig->StackSize);
    t->nextTask = NULL;
    t->SuspendBlocked = false;
    t->DelayBlocked = false;
    t->SemaphoreBlocked = false;
    t->DelayTimer = NULL;
    t->Priority = aTaskConfig->Priority;
    t->nextWaiting = NULL;
    if (t->Stack.Base == NULL) {
        free(t);
        return;
    }

    FeeRTOS_ENTER_CRITICAL();
    if (TaskListHead == NULL) {
        TaskListHead = t;
    } 
    else {
        TTaskInternal* current = TaskListHead;
        while (current->nextTask != NULL) {
            current = current->nextTask;
        }
        current->nextTask = t;
    }

    // Initialen Kontext auf den Stack legen (simuliert ISR-Entry + Context Save)
    unsigned char byte;
    uint16_t pc = (uint16_t)(uintptr_t)aTaskConfig->TaskFunction;
    byte = (unsigned char)(pc & 0xFF); // PC Low-Byte
    Stack_Push(&t->Stack, &byte, 1);
    byte = (unsigned char)((pc >> 8) & 0xFF); // PC High-Byte
    Stack_Push(&t->Stack, &byte, 1);

    // R1–R31 (UserData in R24:R25 — AVR Calling Convention)
    for (unsigned char r = 0; r <= 31 + 1; r++) {
        if (r == 1) // SREG // Interupts enabled
            byte = 0x80;
        else if (r == 24 + 1) // R24
            byte = (unsigned char)((uint16_t)(uintptr_t)aTaskConfig->UserData & 0xFF);
        else if (r == 25 + 1) // R25
            byte = (unsigned char)(((uint16_t)(uintptr_t)aTaskConfig->UserData >> 8) & 0xFF);
        else
            byte = 0x00;
        Stack_Push(&t->Stack, &byte, 1);
    }
    FeeRTOS_EXIT_CRITICAL();
}

void FeeRTOS_DeleteTask(char* aTaskNameID) {
    if (TaskListHead == NULL || aTaskNameID == NULL || strlen(aTaskNameID) > 16) return;
    TTaskInternal* current = TaskListHead;
    TTaskInternal* previous = NULL;
    while(current != NULL){
        if(strcmp(current->NameID, aTaskNameID) == 0){
            break;
        }
        previous = current;
        current = current->nextTask;
    }

    if(current == NULL || current == TaskListHead) return; // Task nicht gefunden oder IdleTask darf nicht gelöscht werden

    FeeRTOS_ENTER_CRITICAL();

    if(current->DelayTimer != NULL) {
        FeeRTOS_DeleteTimer(current->DelayTimer);
        current->DelayTimer = NULL;
    }

    Stack_Destroy(&current->Stack);

    bool isCurrentTask = (current == CurrentTask);

    if (previous == NULL) {
        TaskListHead = current->nextTask;
    } 
    else {
        previous->nextTask = current->nextTask;
    }

    free(current);

    FeeRTOS_EXIT_CRITICAL();
    if(isCurrentTask) {
        FeeRTOS_Yield();
        while(1);
    }
}

void FeeRTOS_StartScheduler(void) {
    if(TaskListHead == NULL) return;

    CurrentTask = TaskListHead;
    SchedulerRunning = true;

    TimerInit(TIMER_NO_A0_16, F_CPU, TIMER_MODE_NORMAL, TICK_RATE);

    FeeRTOS_ENTER_CRITICAL();

    SavedSP = (uint16_t)(uintptr_t)CurrentTask->Stack.StackPointer;

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

void* FeeRTOS_Malloc(unsigned int size) {
    FeeRTOS_ENTER_CRITICAL();

    if (SchedulerRunning) {
        MallocSavedSP = SP;
        SP = RAMEND;
    }

    void* ptr = malloc(size);

    if (SchedulerRunning) {
        SP = MallocSavedSP;
    }

    FeeRTOS_EXIT_CRITICAL();
    return ptr;
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

void FeeRTOS_Delay(unsigned int aDelayMs) {
    FeeRTOS_ENTER_CRITICAL();
    CurrentTask->DelayTimer = FeeRTOS_CreateTimer(aDelayMs);
    if(CurrentTask->DelayTimer == NULL) {
        FeeRTOS_EXIT_CRITICAL();
        return; // malloc fehlgeschlagen, nicht blockieren
    }
    CurrentTask->DelayBlocked = true;
    FeeRTOS_EXIT_CRITICAL();
    FeeRTOS_Yield();
}

void FeeRTOS_SuspendTask(char* aTaskNameID){
    if(aTaskNameID == NULL || CurrentTask == NULL || TaskListHead == NULL || strlen(aTaskNameID) > 16) return;
	TTaskInternal* temp = TaskListHead;
	while(temp != NULL){
		if(strcmp(temp->NameID, aTaskNameID) == 0) break;
		temp = temp->nextTask;
	}
	if(temp == NULL) return;

    FeeRTOS_ENTER_CRITICAL();
    temp->SuspendBlocked = true;
    FeeRTOS_EXIT_CRITICAL();
    if(temp == CurrentTask) FeeRTOS_Yield();
}

void FeeRTOS_ResumeTask(char* aTaskNameID){
	if(aTaskNameID == NULL || CurrentTask == NULL || TaskListHead == NULL || strlen(aTaskNameID) > 16) return;
	TTaskInternal* temp = TaskListHead;
	while(temp != NULL){
		if(strcmp(temp->NameID, aTaskNameID) == 0) break;
		temp = temp->nextTask;
	}
	if(temp == NULL) return;

    FeeRTOS_ENTER_CRITICAL();
    temp->SuspendBlocked = false;
    FeeRTOS_EXIT_CRITICAL();
}

TTaskInternal* getCurrentTask(void) {
    return CurrentTask;
}

__attribute__((used))
static void Schedule(void) {
    CurrentTask->Stack.StackPointer = (void*)(uintptr_t)SavedSP;

    if (ForcedYield) {
        TCA0.SINGLE.CNT = ForcedYieldSavedCNT; // Counter wiederherstellen
        ForcedYield = false;
    } 
    else {
        FeeRTOS_UpdateTimers();
    }

    TTaskInternal* temp = TaskListHead;
    while(temp != NULL){
        if(temp->DelayBlocked) {
            if(temp->DelayTimer != NULL && temp->DelayTimer->Overflow){
                temp->DelayBlocked = false;
                FeeRTOS_DeleteTimer(temp->DelayTimer);
                temp->DelayTimer = NULL;
            }
        }
        temp = temp->nextTask;
    }

    CurrentTask = getNextTask();

    SavedSP = (uint16_t)(uintptr_t)CurrentTask->Stack.StackPointer;

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

static TTaskInternal* getNextTask(void) {
    TTaskInternal* temp = TaskListHead;
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

static inline bool isReadyToRun(TTaskInternal* task) {
    return !(task->SuspendBlocked || task->DelayBlocked || task->SemaphoreBlocked);
}

static void IdleTask(void* aUserData) {
    while (1) {
        if(TaskListHead->nextTask == NULL) asm volatile("sleep"); // asm volatile("nop");
        // else FeeRTOS_Yield(); // Wenn es einen anderen Task gibt, yielden
		TTaskInternal* temp = TaskListHead;
        while(temp != NULL){
            if(isReadyToRun(temp) && temp != CurrentTask) {
                FeeRTOS_Yield(); // Wenn es einen anderen aktiven Task gibt, yielden
                break;
            }
            temp = temp->nextTask;
        }
    }
}