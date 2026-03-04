#include "IFeeRTOS_Timer.h"

static TFeeRTOS_Timer* TimerListHead = NULL;

TFeeRTOS_Timer* FeeRTOS_CreateTimer(unsigned long aMillis){
	if(aMillis == 0) return NULL;
	TFeeRTOS_Timer* newTimer = (TFeeRTOS_Timer*)FeeRTOS_Malloc(sizeof(TFeeRTOS_Timer));
	if(newTimer == NULL) return NULL;
	
	newTimer->TickCounter = 0;
	newTimer->OverflowTicks = (unsigned long)((aMillis / 1000.0) / TICK_RATE);
	newTimer->Overflow = false;
	newTimer->nextTimer = NULL;

	if(TimerListHead == NULL){
		TimerListHead = newTimer;
	} 
	else {
		TFeeRTOS_Timer* current = TimerListHead;
		while(current->nextTimer != NULL){
			current = current->nextTimer;
		}
		current->nextTimer = newTimer;
	}

	return newTimer;
}

void FeeRTOS_DeleteTimer(TFeeRTOS_Timer* timer){
	if(TimerListHead == NULL || timer == NULL) return;

	if(TimerListHead == timer){
		TimerListHead = TimerListHead->nextTimer;
		free(timer);
		return;
	}

	TFeeRTOS_Timer* current = TimerListHead;
	while(current->nextTimer != NULL && current->nextTimer != timer){
		current = current->nextTimer;
	}

	if(current->nextTimer == timer){
		current->nextTimer = timer->nextTimer;
		free(timer);
	}
}

void FeeRTOS_UpdateTimers(){
	TFeeRTOS_Timer* current = TimerListHead;
	while(current != NULL){
		current->TickCounter++;
		if(current->TickCounter >= current->OverflowTicks){
			current->Overflow = true;
			current->TickCounter = 0;
		}
		current = current->nextTimer;
	}
}