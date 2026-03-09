#ifndef FEERTOS_TIMER_H
#define FEERTOS_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct SFeeRTOS_Timer* TFeeRTOS_TimerHandle;

/**
 * FeeRTOS_CreateTimer
 * Erstellt einen Software-Timer. Wenn callback != NULL, wird beim
 * Ablauf die Callback-Funktion im CallbackTask-Kontext aufgerufen.
 * Wenn callback == NULL, wird nur das Overflow-Flag gesetzt.
 * aAutoReload: true = periodisch (Timer laeuft nach Callback weiter),
 *              false = einmalig (One-Shot, Timer wird nach Callback geloescht)
 *
 * aMillis     - Timerdauer in Millisekunden
 * callback    - Callback-Funktion
 * args        - Argumente fuer Callback
 * aAutoReload - true: periodisch, false: einmalig
 * Gibt ein Handle zurueck oder NULL bei Fehler.
 */
TFeeRTOS_TimerHandle FeeRTOS_CreateTimer(uint32_t aMillis, void (*callback)(void* args), void* args, bool aAutoReload);

/**
 * FeeRTOS_DeleteTimer
 * Loescht einen Timer anhand seines Handles.
 * Gibt alle wartenden Tasks frei.
 *
 * timer - Handle des zu loeschenden Timers
 */
void FeeRTOS_DeleteTimer(TFeeRTOS_TimerHandle timer);

/**
 * FeeRTOS_UpdateTimers
 * Aktualisiert alle laufenden Timer.
 * Sollte zyklisch vom Scheduler aufgerufen werden.
 */
void FeeRTOS_UpdateTimers();


#endif