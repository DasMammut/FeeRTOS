#ifndef I_FEERTOS_H
#define I_FEERTOS_H

#include "FeeRTOS.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "FeeRTOS_GLOBALS.h"
#include "IFeeRTOS_Timer.h"
#include "IFeeRTOS_Stack.h"
#include "IFeeRTOS_Semaphore.h"

#define FeeRTOS_ENTER_CRITICAL() unsigned char sreg_save = SREG; cli()
#define FeeRTOS_EXIT_CRITICAL() SREG = sreg_save

typedef enum{
    TASK_STATE_RUNNING,
    TASK_STATE_READY,
    TASK_STATE_DELAYED,
    TASK_STATE_WAITING,
    TASK_STATE_SUSPENDED
} TTaskState;

typedef struct TaskInternal {
    void (*TaskFunction)(void* aUserData);
    void* UserData;
    char NameID[16];
    TStack Stack;
    TTaskState State;
    TFeeRTOS_Timer* DelayTimer;
    TTaskPriority Priority;
    struct TaskInternal* nextTask;
} TTaskInternal;

#endif