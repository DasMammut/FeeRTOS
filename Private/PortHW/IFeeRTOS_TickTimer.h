#ifndef I_FEERTOS_TICKTIMER_H
#define I_FEERTOS_TICKTIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum{
	TIMER_PRESCALER_1,
	TIMER_PRESCALER_2,
	TIMER_PRESCALER_4,
	TIMER_PRESCALER_8,
	TIMER_PRESCALER_16,
	TIMER_PRESCALER_64,
	TIMER_PRESCALER_256,
	TIMER_PRESCALER_1024
} TFeeRTOS_TickTimerPrescaler;

/*
 * FeeRTOS_SetupTickTimer
 * Initialisiert TCA0 als System-Tick.
 * aCpuClk: Frequenz der CPU (z.B. 3333333 Hz)
 * aTickRateHz: Wie oft der Scheduler pro Sekunde aufgerufen werden soll (z.B. 1000 für 1ms)
 */
bool FeeRTOS_SetupTickTimer(float aTickRate);

/* Deaktiviert den Tick-Timer */
void FeeRTOS_StopTickTimer(void);

#endif