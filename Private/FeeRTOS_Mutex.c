#include "IFeeRTOS_Mutex.h"

TFeeRTOS_MutexHandle FeeRTOS_CreateMutex(void){
    TFeeRTOS_MutexHandle mutex = (TFeeRTOS_MutexHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Mutex));
    if(mutex == NULL) return NULL;
    mutex->Owner = NULL;
    mutex->WaitingListHead = NULL;
    mutex->Count = 0;
    return mutex;
}

void FeeRTOS_DeleteMutex(TFeeRTOS_MutexHandle aMutex){
    if(aMutex == NULL) return;
    FeeRTOS_Free(aMutex);
}

void FeeRTOS_MutexLock(TFeeRTOS_MutexHandle aMutex){
    if(aMutex == NULL) return;
    TFeeRTOS_TaskHandle currentTask = getCurrentTask();
    FeeRTOS_ENTER_CRITICAL();
    
    if(aMutex->Count == 0 || aMutex->Owner == currentTask){
        aMutex->Owner = currentTask;
        aMutex->Count++;
        FeeRTOS_EXIT_CRITICAL();
        return;
    }
    
    // In Warteliste einreihen
    if(aMutex->WaitingListHead == NULL) {
        // Fall A: Liste war leer -> Task wird das neue Oberhaupt
        aMutex->WaitingListHead = currentTask;
    } else {
        // Fall B: Liste nicht leer -> ans Ende laufen
        TFeeRTOS_TaskHandle temp = aMutex->WaitingListHead;
        while(temp->nextWaiting != NULL){
            if(temp == currentTask) { // Sicherheit: Schon drin?
                FeeRTOS_EXIT_CRITICAL();
                return;
            }
            temp = temp->nextWaiting;
        }
        temp->nextWaiting = currentTask;
    }
    
    currentTask->nextWaiting = NULL;
    currentTask->BlockedFlags.Semaphore = true; // Task schlafen legen
    
    FeeRTOS_EXIT_CRITICAL();
    FeeRTOS_Yield(); 
}

void FeeRTOS_MutexUnlock(TFeeRTOS_MutexHandle aMutex){
    if(aMutex == NULL) return;
    FeeRTOS_ENTER_CRITICAL();
    TFeeRTOS_TaskHandle currentTask = getCurrentTask();
    if(aMutex->Owner != currentTask) {
        FeeRTOS_EXIT_CRITICAL();
        return; // Nur der Besitzer kann entsperren
    }

    aMutex->Count--;
    if(aMutex->Count > 0) {
        FeeRTOS_EXIT_CRITICAL();
        return; // Es gibt noch weitere Sperren, daher nicht freigeben
    }

    if(aMutex->WaitingListHead == NULL) {
        aMutex->Owner = NULL; // Kein wartender Task, Mutex freigeben
        FeeRTOS_EXIT_CRITICAL();
        return;
    }

    // Nächsten wartenden Task freigeben
    TFeeRTOS_TaskHandle nextTask = aMutex->WaitingListHead;
    aMutex->WaitingListHead = nextTask->nextWaiting;
    nextTask->nextWaiting = NULL;
    nextTask->BlockedFlags.Semaphore = false;
    aMutex->Owner = nextTask;
    aMutex->Count++;
    FeeRTOS_EXIT_CRITICAL();
}