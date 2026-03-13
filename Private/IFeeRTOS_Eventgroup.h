#ifndef I_FEERTOS_EVENTGROUP_H
#define I_FEERTOS_EVENTGROUP_H

#include "FeeRTOS_Eventgroup.h"
#include "IFeeRTOS.h"
#include "IFeeRTOS_Heap.h"

typedef struct SFeeRTOS_WaitingTask {
    struct SFeeRTOS_WaitingTask* Next;
    TFeeRTOS_TaskHandle TaskHandle;
    uint8_t Bits;
    bool WaitForAll;
} TFeeRTOS_WaitingTask;

typedef struct SFeeRTOS_Eventgroup {
    TFeeRTOS_WaitingTask* WaitingListHead;
    uint8_t Bits;
} TFeeRTOS_Eventgroup;



#endif