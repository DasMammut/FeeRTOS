#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

typedef enum{
	TIMER_NO_A0_16,
	TIMER_NO_A0_8_1,
	TIMER_NO_A0_8_2,
	TIMER_NO_B0,
	TIMER_NO_B1,
	TIMER_NO_B2,
	TIMER_NO_B3,
	TIMER_NO_B4,
	TIMER_NO_LAST
} TTimerNo;

typedef enum{
	TIMER_MODE_NORMAL,
	TIMER_MODE_PWM_SINGLE,
	TIMER_MODE_WAVEFORM
} TTimerMode;

typedef enum{
	TIMER_CMP_NO_0 = ( 1 << 0 ),
	TIMER_CMP_NO_1 = ( 1 << 1 ),
	TIMER_CMP_NO_2 = ( 1 << 2 )
} TTimerCmpChNo;

typedef void (*TTimerFunction)(void* aUserData);

/*
 * TimerInit
 * Initialisiert einen Timer mit dem angegebenen Modus und Interrupt-Intervall.
 * Allokiert intern Speicher fuer den Timer. Ruft sei() auf wenn erfolgreich.
 *
 * aTimerNo        - Welcher Timer (z.B. TIMER_NO_A0_16)
 * aCpuClk         - CPU-Taktfrequenz in Hz
 * aTimerMode      - Betriebsmodus (Normal, PWM, etc.)
 * aInterruptTime  - Gewuenschtes Interrupt-Intervall in Sekunden
 *
 * Gibt true zurueck wenn erfolgreich, false bei Fehler oder wenn
 * der Timer bereits initialisiert ist.
 */
bool TimerInit(TTimerNo aTimerNo, unsigned long aCpuClk, TTimerMode aTimerMode, float aInterruptTime);

/*
 * TimerDone
 * Deinitialisiert einen Timer und gibt den allokierten Speicher frei.
 * Deaktiviert die Interrupts des Timers.
 *
 * aTimerNo - Welcher Timer freigegeben werden soll
 */
void TimerDone(TTimerNo aTimerNo);

/*
 * TimerSetOverflowFunction
 * Registriert eine Callback-Funktion die bei jedem Timer-Overflow
 * aufgerufen wird.
 *
 * aTimerNo        - Welcher Timer
 * aTimerFunction  - Zeiger auf die Callback-Funktion
 * aUserData       - Zeiger auf beliebige Benutzerdaten, wird dem
 *                   Callback als Parameter uebergeben
 *
 * Gibt true zurueck wenn erfolgreich, false wenn Timer nicht initialisiert.
 */
bool TimerSetOverflowFunction(TTimerNo aTimerNo, TTimerFunction aTimerFunction, void* aUserData);

/*
 * TimerSetPwmChannels
 * Aktiviert PWM-Ausgangskanaele fuer den angegebenen Timer.
 * Mehrere Kanaele koennen mit OR kombiniert werden.
 *
 * aTimerNo      - Welcher Timer
 * aCmpChannels  - Bitmaske der zu aktivierenden Kanaele
 *                 (TIMER_CMP_NO_0, TIMER_CMP_NO_1, TIMER_CMP_NO_2)
 */
void TimerSetPwmChannels(TTimerNo aTimerNo, TTimerCmpChNo aCmpChannels);

/*
 * TimerSetDutyCycle
 * Setzt den Duty-Cycle eines PWM-Kanals.
 *
 * aTimerNo    - Welcher Timer
 * aCmpChannel - Welcher Compare-Kanal
 * aDutyCycle  - Duty-Cycle als Wert zwischen 0.0 und 1.0
 */
void TimerSetDutyCycle(TTimerNo aTimerNo, TTimerCmpChNo aCmpChannel, float aDutyCycle);

#endif