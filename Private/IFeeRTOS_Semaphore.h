#ifndef I_FEERTOS_SEMAPHORE_H
#define I_FEERTOS_SEMAPHORE_H

#include "FeeRTOS_Semaphore.h"

#include <stdint.h>

#include "IFeeRTOS.h"

typedef struct SFeeRTOS_Semaphore {
    volatile uint8_t Count;
    uint8_t MaxCount;
    TFeeRTOS_TaskHandle WaitingListHead;
    TFeeRTOS_TaskHandle WaitingListTail;
} TFeeRTOS_Semaphore;


#endif