#include "IFeeRTOS.h"

static TFeeRTOS_Timer* TimerListHead = NULL;

TFeeRTOS_TimerHandle FeeRTOS_CreateTimer(uint32_t aMillis, void (*callback)(void* args), void* args, bool aAutoReload){
	if(aMillis == 0) return NULL;
	TFeeRTOS_TimerHandle newTimer = (TFeeRTOS_TimerHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Timer));
	if(newTimer == NULL) return NULL;
	
	newTimer->WakeTick = (uint32_t)((aMillis / 1000.0) / TICK_RATE) + TickCount; // Berechnung der Tick-Grenze
	newTimer->millis = aMillis;
	newTimer->Overflow = false;
	newTimer->AutoReload = aAutoReload;
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
		FeeRTOS_Free(timer);
		return;
	}

	TFeeRTOS_TimerHandle current = TimerListHead;
	while(current->nextTimer != NULL && current->nextTimer != timer){
		current = current->nextTimer;
	}

	if(current->nextTimer == timer){
		current->nextTimer = timer->nextTimer;
		FeeRTOS_Free(timer);
	}
}

void FeeRTOS_UpdateTimers(){
	bool anyOverflow = false;
	TFeeRTOS_TimerHandle current = TimerListHead;
	while(current != NULL){
		if(TickCount >= current->WakeTick){
			current->Overflow = true;
			anyOverflow = true;
			if(current->AutoReload){
				current->WakeTick += (uint32_t)((current->millis / 1000.0) / TICK_RATE); // naechste Tick-Grenze berechnen
			}
		}
		current = current->nextTimer;
	}
	// CallbackTask NUR wecken wenn mindestens ein Timer abgelaufen ist
	if(anyOverflow){
		FeeRTOS_ResumeTask(getCallbackTask());
	}
}

void CallbackTask(void* args) {
	(void)args;
	while(1) {
		FeeRTOS_ENTER_CRITICAL();
		TFeeRTOS_TimerHandle current = TimerListHead;
		while(current != NULL){
			// nextTimer VORHER sichern, da Callback den Timer loeschen koennte (Use-After-FeeRTOS_Free)
			TFeeRTOS_TimerHandle next = current->nextTimer;
			if(current->Overflow && current->Callback != NULL){
				bool autoReload = current->AutoReload;
				current->Overflow = false; // VOR dem Callback zuruecksetzen
				FeeRTOS_EXIT_CRITICAL();
				current->Callback(current->CallbackArgs);
				FeeRTOS_ENTER_CRITICAL();
				// One-Shot Timer nach Callback loeschen
				if(!autoReload){
					FeeRTOS_DeleteTimer(current);
				}
			}
			current = next;
		}
		FeeRTOS_EXIT_CRITICAL();
		FeeRTOS_SuspendTask(getCallbackTask());
	}
}
