#include <avr/io.h>

#define F_CPU (20000000UL/6)

#include <avr/delay.h>

#include "FeeRTOS.h"
#include "FeeRTOS_Semaphore.h"

// Binaere Semaphore (Mutex) — nur ein Task darf gleichzeitig in der "kritischen Sektion" sein
TFeeRTOS_SemaphoreHandle Mutex;

/*
 * Semaphore-Test:
 * Beide Tasks wollen abwechselnd ihre LED einschalten.
 * Durch die Semaphore (init=1, max=1) ist immer nur EINE LED gleichzeitig an.
 *
 * Erwartetes Verhalten:
 *   - LED A geht an fuer 500ms, dann aus
 *   - LED C geht an fuer 500ms, dann aus
 *   - usw. abwechselnd
 *
 * Ohne Semaphore wuerden beide LEDs gleichzeitig blinken.
 */

void Task_LedA(void* aUserData){
	(void)aUserData;
	PORTA.DIRSET = 0xFF;
	while (1){
		FeeRTOS_SemaphoreTake(Mutex);   // Warten bis Semaphore frei
		PORTA.OUTSET = 0xFF;            // LED A an
		FeeRTOS_Delay(500);
		PORTA.OUTCLR = 0xFF;            // LED A aus
		FeeRTOS_SemaphoreGive(Mutex);   // Semaphore freigeben
		FeeRTOS_Delay(100);             // Kurze Pause damit Task B drankommt
	}
}

void Task_LedC(void* aUserData){
	(void)aUserData;
	PORTC.DIRSET = 0xFF;
	while (1){
		FeeRTOS_SemaphoreTake(Mutex);   // Warten bis Semaphore frei
		PORTC.OUTSET = 0xFF;            // LED C an
		FeeRTOS_Delay(500);
		PORTC.OUTCLR = 0xFF;            // LED C aus
		FeeRTOS_SemaphoreGive(Mutex);   // Semaphore freigeben
		FeeRTOS_Delay(100);             // Kurze Pause damit Task A drankommt
	}
}

TTaskConfig TaskA = {
	.TaskFunction = Task_LedA,
	.UserData = 0,
	.StackSize = 64,
	.Priority = TASK_PRIORITY_MEDIUM,
	.NameID = "LedA"
};

TTaskConfig TaskC = {
	.TaskFunction = Task_LedC,
	.UserData = 0,
	.StackSize = 64,
	.Priority = TASK_PRIORITY_MEDIUM,
	.NameID = "LedC"
};


int main(void){
	FeeRTOS_Init();

	// Binaere Semaphore erstellen (init=1 => sofort verfuegbar, max=1)
	Mutex = FeeRTOS_CreateSemaphore(1, 1);

	FeeRTOS_CreateTask(&TaskA);
	FeeRTOS_CreateTask(&TaskC);

	FeeRTOS_StartScheduler();

	while(1){
	}
	return 0;
}