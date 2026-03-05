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
#include "IFeeRTOS_Heap.h"

typedef union UTaskBlockedFlags {
    struct {
        bool Suspend : 1;
        bool Delay : 1;
        bool Semaphore : 1;
    };
    uint8_t All;
} TTaskBlockedFlags;

typedef struct SFeeRTOS_Task {
    void (*TaskFunction)(void* aUserData);
    void* UserData;
    TFeeRTOS_StackHandle Stack;
    TTaskBlockedFlags BlockedFlags;
    TFeeRTOS_TimerHandle DelayTimer;
    TTaskPriority Priority;
    struct SFeeRTOS_Task* nextWaiting;
    struct SFeeRTOS_Task* nextTask;
} TFeeRTOS_Task;

TFeeRTOS_TaskHandle getCurrentTask(void);
TFeeRTOS_TaskHandle getCallbackTask(void);

volatile uint32_t TickCount;

#endif