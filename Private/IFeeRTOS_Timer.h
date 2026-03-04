#ifndef I_FEERTOS_TIMER_H
#define I_FEERTOS_TIMER_H

#include "FeeRTOS_Timer.h"

#include "FeeRTOS_GLOBALS.h"

typedef struct SFeeRTOS_Timer{
	unsigned long TickCounter;
	unsigned long OverflowTicks;
	bool Overflow;
	bool AutoReload;
    void (*Callback)(void* args);
    void* CallbackArgs;
	struct SFeeRTOS_Timer* nextTimer;
} TFeeRTOS_Timer;

void CallbackTask(void* args);

#endif