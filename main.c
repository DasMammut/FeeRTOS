#include <avr/io.h>

#define F_CPU (20000000UL/6)

#include <avr/delay.h>

#include "FeeRTOS.h"
#include "FeeRTOS_Timer.h"
#include "FeeRTOS_Mutex.h"

// Globaler Mutex-Handle
TFeeRTOS_MutexHandle gLedMutex;

// Task 1: toggelt PORTA mit Mutex
void Task_LedA(void* aUserData){
    (void)aUserData;
    PORTA.DIRSET = 0xFF;
    while (1){
        FeeRTOS_MutexLock(gLedMutex);
        PORTA.OUTTGL = 0xFF;
        FeeRTOS_Delay(500);
        PORTA.OUTTGL = 0xFF;
        FeeRTOS_MutexUnlock(gLedMutex);
    }
}

// Task 2: toggelt PORTC mit Mutex
void Task_LedC_Mutex(void* aUserData){
    (void)aUserData;
    PORTC.DIRSET = 0xFF;
    while (1){
        FeeRTOS_MutexLock(gLedMutex);
        PORTC.OUTTGL = 0xFF;
        FeeRTOS_Delay(500);
        PORTC.OUTTGL = 0xFF;
        FeeRTOS_MutexUnlock(gLedMutex);
    }
}

int main(void){
    FeeRTOS_Init();

    // Mutex erzeugen
    gLedMutex = FeeRTOS_CreateMutex();

    // Tasks mit Mutex
    FeeRTOS_CreateTask(Task_LedA, NULL, 64, TASK_PRIORITY_MEDIUM);
    FeeRTOS_CreateTask(Task_LedC_Mutex, NULL, 64, TASK_PRIORITY_MEDIUM);

    FeeRTOS_StartScheduler();

    while(1){
    }
    return 0;
}