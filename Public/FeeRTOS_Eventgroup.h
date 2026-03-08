#ifndef FEERTOS_EVENTGROUP_H
#define FEERTOS_EVENTGROUP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct SFeeRTOS_Eventgroup* TFeeRTOS_EventgroupHandle;

TFeeRTOS_EventgroupHandle FeeRTOS_CreateEventgroup(void);

void FeeRTOS_DeleteEventgroup(TFeeRTOS_EventgroupHandle eventgroup);

void FeeRTOS_EventgroupWait(TFeeRTOS_EventgroupHandle eventgroup, uint8_t aBits, bool aWaitForAll);

void FeeRTOS_EventgroupSetBit(TFeeRTOS_EventgroupHandle eventgroup, uint8_t aBitIndex, bool aSet);



#endif