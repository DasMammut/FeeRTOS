#include <avr/io.h>

#define F_CPU (20000000UL/6)

#include <avr/delay.h>

#include "FeeRTOS.h"
#include "FeeRTOS_Timer.h"

/*
 * Timer-Callback Test:
 * - TimerCallback toggelt PORTA (LEDs) alle 500ms per Callback-Timer
 * - Task_LedC toggelt PORTC alle 300ms per FeeRTOS_Delay (testet Delay mit neuem Timer)
 *
 * Erwartetes Verhalten:
 *   - PORTA blinkt im 500ms-Takt (gesteuert durch Timer-Callback, OHNE eigenen Task)
 *   - PORTC blinkt im 300ms-Takt (gesteuert durch Delay in eigenem Task)
 *   - Beide unabhaengig voneinander
 */

// Timer-Callback: wird vom CallbackTask aufgerufen wenn der Timer ablaeuft
void ToggleLedA(void* args) {
	(void)args;
	PORTA.OUTTGL = 0xFF;
}

// Normaler Task: testet FeeRTOS_Delay (nutzt intern auch den neuen Timer + Callback)
void Task_LedC(void* aUserData){
	(void)aUserData;
	PORTC.DIRSET = 0xFF;
	while (1){
		PORTC.OUTTGL = 0xFF;
		FeeRTOS_Delay(300);
	}
}

TTaskConfig TaskC = {
	.TaskFunction = Task_LedC,
	.UserData = 0,
	.StackSize = 64,
	.Priority = TASK_PRIORITY_MEDIUM,
	.NameID = "LedC"
};


int main(void){
	FeeRTOS_Init();

	// PORTA als Ausgang konfigurieren (fuer Timer-Callback)
	PORTA.DIRSET = 0xFF;
	PORTA.OUTCLR = 0xFF;

	// Timer-Callback: toggelt PORTA alle 500ms (periodisch, da Timer immer wieder Overflow -> Callback)
	FeeRTOS_CreateTimer(500, ToggleLedA, NULL);

	FeeRTOS_CreateTask(&TaskC);

	FeeRTOS_StartScheduler();

	while(1){
	}
	return 0;
}