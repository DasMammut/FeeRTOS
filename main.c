#include <avr/io.h>
#include <avr/delay.h>

#include "FeeRTOS.h"

#define F_CPU 20000000/6


void Task_1(void* aUserData){
	PORTA.DIRSET = 0xFF;
	while (1){
		PORTA.OUTTGL = 0xFF;
		FeeRTOS_Delay(*(int*)aUserData);
	}
}

void Task_2(void* aUserData){
	PORTC.DIRSET = 0xFF;
	while (1){
		PORTC.OUTTGL = 0xFF;
		FeeRTOS_Delay(*(int*)aUserData);
	}
	
}

void Task_3(void* aUserData){
	FeeRTOS_Delay(*(int*)aUserData);
	FeeRTOS_DeleteTask("Task1");
	FeeRTOS_DeleteTask("Task3");
}

int delay1 = 1000;
int delay2 = 3000;

TTaskConfig Task1 = {
	.TaskFunction = Task_1,
	.UserData = &delay1,
	.StackSize = 64,
	.Priority = TASK_PRIORITY_MEDIUM,
	.NameID = "Task1"
};

TTaskConfig Task2 = {
	.TaskFunction = Task_2,
	.UserData = &delay2,
	.StackSize = 64,
	.Priority = TASK_PRIORITY_MEDIUM,
	.NameID = "Task2"
};


int main(void){
	FeeRTOS_Init();
	
	FeeRTOS_CreateTask(&Task1);
	FeeRTOS_CreateTask(&Task2);
	
	FeeRTOS_StartScheduler();

	// Der Scheduler übernimmt ab hier die Kontrolle, main() sollte nicht weiter ausgeführt werden
	// Falls main() doch zurückkehrt heißt es Speicher konnte für die Tasks nicht allokiert werden
	// oder es wurde die FeeRTOS_Init nicht ausgeführt sowie keine eigen Task erstellt (z.B. nur StartScheduler aufgerufen).
	while(1){
		
	}
	return 0;
}