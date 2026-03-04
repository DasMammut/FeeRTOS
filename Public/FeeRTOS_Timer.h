#ifndef FEERTOS_TIMER_H
#define FEERTOS_TIMER_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct SFeeRTOS_Timer* TFeeRTOS_TimerHandle;

/*
 * FeeRTOS_CreateTimer
 * Erstellt einen Software-Timer. Wenn callback != NULL, wird beim
 * Ablauf die Callback-Funktion im CallbackTask-Kontext aufgerufen.
 * Wenn callback == NULL, wird nur das Overflow-Flag gesetzt.
 */
TFeeRTOS_TimerHandle FeeRTOS_CreateTimer(unsigned long aMillis, void (*callback)(void* args), void* args);

void FeeRTOS_DeleteTimer(TFeeRTOS_TimerHandle timer);

void FeeRTOS_UpdateTimers();


#endif