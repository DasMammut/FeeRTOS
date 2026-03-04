#ifndef FEERTOS_TIMER_H
#define FEERTOS_TIMER_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct FeeRTOS_Timer{
	unsigned long TickCounter;
	unsigned long OverflowTicks;
	bool Overflow;
	struct FeeRTOS_Timer* nextTimer;
} TFeeRTOS_Timer;

TFeeRTOS_Timer* FeeRTOS_CreateTimer(unsigned long aMillis);

void FeeRTOS_DeleteTimer(TFeeRTOS_Timer* timer);

void FeeRTOS_UpdateTimers();


#endif