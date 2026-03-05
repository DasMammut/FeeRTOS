#ifndef I_FEERTOS_H
#define I_FEERTOS_H

#include "FeeRTOS.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "FeeRTOS_GLOBALS.h"
#include "IFeeRTOS_HWTimer.h"
#include "IFeeRTOS_Timer.h"
#include "IFeeRTOS_Stack.h"
#include "IFeeRTOS_Semaphore.h"

typedef struct TaskInternal {
    void (*TaskFunction)(void* aUserData);
    void* UserData;
    char NameID[16];
    TStack Stack;
    bool SuspendBlocked;
    bool DelayBlocked;
    bool SemaphoreBlocked;
    TFeeRTOS_Timer* DelayTimer;
    TTaskPriority Priority;
    struct TaskInternal* nextWaiting;
    struct TaskInternal* nextTask;
} TTaskInternal;

TTaskInternal* getCurrentTask(void);

volatile uint32_t TickCount;

#endif