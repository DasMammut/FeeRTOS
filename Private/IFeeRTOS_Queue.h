#ifndef I_FEERTOS_QUEUE_H
#define I_FEERTOS_QUEUE_H

#include "FeeRTOS_Queue.h"
#include "IFeeRTOS.h"
#include "IFeeRTOS_Semaphore.h"
#include "IFeeRTOS_Ringbuffer.h"

typedef struct SFeeRTOS_Queue {
    TFeeRTOS_RingbufferHandle buffer;
    TFeeRTOS_SemaphoreHandle ItemsAvailable;
    TFeeRTOS_SemaphoreHandle SpaceAvailable;
} TFeeRTOS_Queue;

#endif