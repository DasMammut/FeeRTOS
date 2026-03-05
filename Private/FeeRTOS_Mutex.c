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
    if(aMutex->Count == 0){
        aMutex->Owner = currentTask;
        aMutex->Count++;
        FeeRTOS_EXIT_CRITICAL();
        return;
    }
    if(aMutex->Owner == currentTask){
        aMutex->Count++;
        FeeRTOS_EXIT_CRITICAL();
        return;
    }
    
    TFeeRTOS_TaskHandle waitingTask = aMutex->WaitingListHead;
    while(waitingTask->nextWaiting != NULL){
        if(waitingTask == currentTask) break; // Task ist bereits in der Warteliste
        waitingTask = waitingTask->nextWaiting;
    }
    waitingTask->nextWaiting = currentTask;
    currentTask->nextWaiting = NULL;
    currentTask->SemaphoreBlocked = true;
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

    aMutex->Owner = NULL;

    // Nächsten wartenden Task freigeben
    if(aMutex->WaitingListHead != NULL){
        TFeeRTOS_TaskHandle nextTask = aMutex->WaitingListHead;
        aMutex->WaitingListHead = nextTask->nextWaiting;
        nextTask->nextWaiting = NULL;
        nextTask->SemaphoreBlocked = false;
    }
    FeeRTOS_EXIT_CRITICAL();
}