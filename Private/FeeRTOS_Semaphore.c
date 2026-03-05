#include "IFeeRTOS.h"

TFeeRTOS_SemaphoreHandle FeeRTOS_CreateSemaphore(uint8_t aInitialCount, uint8_t aMaxCount) {
    if(aMaxCount == 0 || aInitialCount > aMaxCount) return NULL;

    TFeeRTOS_SemaphoreHandle semaphore = (TFeeRTOS_SemaphoreHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Semaphore));
    if(semaphore == NULL) return NULL;

    semaphore->Count = aInitialCount;
    semaphore->MaxCount = aMaxCount;
    semaphore->WaitingListHead = NULL;
    semaphore->WaitingListTail = NULL;
    return semaphore;
}

void FeeRTOS_DeleteSemaphore(TFeeRTOS_SemaphoreHandle aSemaphore){
    if(aSemaphore == NULL) return;

    FeeRTOS_ENTER_CRITICAL();
    // Alle wartenden Tasks freigeben
    TFeeRTOS_TaskHandle task = aSemaphore->WaitingListHead;
    while(task != NULL){
        task->SemaphoreBlocked = false;
        TFeeRTOS_TaskHandle next = task->nextWaiting;
        task->nextWaiting = NULL;
        task = next;
    }
    free(aSemaphore);
    FeeRTOS_EXIT_CRITICAL();
}

void FeeRTOS_SemaphoreTake(TFeeRTOS_SemaphoreHandle aSemaphore){
    if(aSemaphore == NULL) return;

    FeeRTOS_ENTER_CRITICAL();
    if(aSemaphore->Count > 0){
        // Semaphore verfuegbar -> sofort nehmen
        aSemaphore->Count--;
        FeeRTOS_EXIT_CRITICAL();
        return;
    }

    // Count == 0 -> Task muss warten
    TFeeRTOS_TaskHandle current = getCurrentTask();
    current->nextWaiting = NULL;

    // In Waiting-Liste einhaengen (FIFO)
    if(aSemaphore->WaitingListTail == NULL){
        aSemaphore->WaitingListHead = current;
    } 
    else {
        aSemaphore->WaitingListTail->nextWaiting = current;
    }
    aSemaphore->WaitingListTail = current;

    // Task blockieren
    current->SemaphoreBlocked = true;
    FeeRTOS_EXIT_CRITICAL();
    FeeRTOS_Yield();
}

void FeeRTOS_SemaphoreGive(TFeeRTOS_SemaphoreHandle aSemaphore){
    if(aSemaphore == NULL) return;

    FeeRTOS_ENTER_CRITICAL();
    if(aSemaphore->WaitingListHead != NULL){
        // Ersten wartenden Task aufwecken (statt Counter erhoehen)
        TFeeRTOS_TaskHandle task = aSemaphore->WaitingListHead;
        task->SemaphoreBlocked = false;

        aSemaphore->WaitingListHead = task->nextWaiting;
        task->nextWaiting = NULL;
        if(aSemaphore->WaitingListHead == NULL){
            aSemaphore->WaitingListTail = NULL;
        }
    }
    else if(aSemaphore->Count < aSemaphore->MaxCount){
        // Kein wartender Task -> Counter erhoehen (bis Max)
        aSemaphore->Count++;
    }
    FeeRTOS_EXIT_CRITICAL();
}