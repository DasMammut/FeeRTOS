#ifndef I_FEERTOS_MUTEX_H
#define I_FEERTOS_MUTEX_H

#include "FeeRTOS_Mutex.h"
#include "IFeeRTOS.h"

typedef struct SFeeRTOS_Mutex{
    TFeeRTOS_TaskHandle Owner;
    TFeeRTOS_TaskHandle WaitingListHead;
    uint8_t Count;
} TFeeRTOS_Mutex;


#endif