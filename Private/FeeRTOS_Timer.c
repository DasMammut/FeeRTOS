#include "IFeeRTOS_Timer.h"

static TFeeRTOS_Timer* TimerListHead = NULL;

TFeeRTOS_TimerHandle FeeRTOS_CreateTimer(unsigned long aMillis, void (*callback)(void* args), void* args){
	if(aMillis == 0) return NULL;
	TFeeRTOS_TimerHandle newTimer = (TFeeRTOS_TimerHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Timer));
	if(newTimer == NULL) return NULL;
	
	newTimer->TickCounter = 0;
	newTimer->OverflowTicks = (unsigned long)((aMillis / 1000.0) / TICK_RATE);
	newTimer->Overflow = false;
	newTimer->nextTimer = NULL;
	newTimer->Callback = callback;
	newTimer->CallbackArgs = args;

	if(TimerListHead == NULL){
		TimerListHead = newTimer;
	} 
	else {
		TFeeRTOS_TimerHandle current = TimerListHead;
		while(current->nextTimer != NULL){
			current = current->nextTimer;
		}
		current->nextTimer = newTimer;
	}

	return newTimer;
}

void FeeRTOS_DeleteTimer(TFeeRTOS_TimerHandle timer){
	if(TimerListHead == NULL || timer == NULL) return;

	if(TimerListHead == timer){
		TimerListHead = TimerListHead->nextTimer;
		free(timer);
		return;
	}

	TFeeRTOS_TimerHandle current = TimerListHead;
	while(current->nextTimer != NULL && current->nextTimer != timer){
		current = current->nextTimer;
	}

	if(current->nextTimer == timer){
		current->nextTimer = timer->nextTimer;
		free(timer);
	}
}

void FeeRTOS_UpdateTimers(){
	bool anyOverflow = false;
	TFeeRTOS_TimerHandle current = TimerListHead;
	while(current != NULL){
		if(!current->Overflow){
			current->TickCounter++;
			if(current->TickCounter >= current->OverflowTicks){
				current->Overflow = true;
				current->TickCounter = 0;
				anyOverflow = true;
			}
		}
		current = current->nextTimer;
	}
	// CallbackTask NUR wecken wenn mindestens ein Timer abgelaufen ist
	if(anyOverflow){
		FeeRTOS_ResumeTask(CALLBACK_TASK_NAME);
	}
}

void CallbackTask(void* args) {
	(void)args;
	while(1) {
		FeeRTOS_ENTER_CRITICAL();
		TFeeRTOS_TimerHandle current = TimerListHead;
		while(current != NULL){
			// nextTimer VORHER sichern, da Callback den Timer loeschen koennte (Use-After-Free)
			TFeeRTOS_TimerHandle next = current->nextTimer;
			if(current->Overflow && current->Callback != NULL){
				current->Overflow = false; // VOR dem Callback zuruecksetzen
				FeeRTOS_EXIT_CRITICAL();
				current->Callback(current->CallbackArgs);
				FeeRTOS_ENTER_CRITICAL();
			}
			current = next;
		}
		FeeRTOS_EXIT_CRITICAL();
		FeeRTOS_SuspendTask(CALLBACK_TASK_NAME);
	}
}
