#ifndef I_FEERTOS_SEMAPHORE_H
#define I_FEERTOS_SEMAPHORE_H

#include "FeeRTOS_Semaphore.h"

#include <stdint.h>

typedef struct SFeeRTOS_Semaphore {
    volatile uint8_t Count;
    uint8_t MaxCount;
    struct TaskInternal* WaitingListHead;
    struct TaskInternal* WaitingListTail;
} TFeeRTOS_Semaphore;


#endif