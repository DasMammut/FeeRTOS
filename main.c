#include <avr/io.h>
#define F_CPU (20000000UL/6)
#include <avr/delay.h>
#include <stdlib.h>

#include "FeeRTOS.h"
#include "FeeRTOS_Eventgroup.h"

// Globales Eventgroup-Handle
TFeeRTOS_EventgroupHandle gEventGroup;

// Task 1: setzt Eventbit und toggelt PORTA
void Task_LedA_Event(void* aUserData){
    (void)aUserData;
    PORTA.DIRSET = 0xFF;
    while (1){
        PORTA.OUTTGL = 0xFF;
        FeeRTOS_EventgroupSetBit(gEventGroup, 0, true); // Setze Bit 0
        FeeRTOS_Delay(500);
    }
}

// Task 2: wartet auf Eventbit und toggelt PORTC
void Task_LedC_Event(void* aUserData){
    (void)aUserData;
    PORTC.DIRSET = 0xFF;
    while (1){
        FeeRTOS_EventgroupWait(gEventGroup, 0x01, true); // Warte auf Bit 0
        PORTC.OUTTGL = 0xFF;
        FeeRTOS_EventgroupSetBit(gEventGroup, 0, false); // Lösche Bit 0
    }
}

int main(void){
    FeeRTOS_Init();

    // Eventgroup erzeugen
    gEventGroup = FeeRTOS_CreateEventgroup();

    // Tasks mit Eventgroup
    FeeRTOS_CreateTask(Task_LedA_Event, NULL, 64, TASK_PRIORITY_MEDIUM);
    FeeRTOS_CreateTask(Task_LedC_Event, NULL, 64, TASK_PRIORITY_MEDIUM);

    FeeRTOS_StartScheduler();

    while(1){}
    return 0;
}