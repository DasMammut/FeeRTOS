#include "IFeeRTOS_Semaphore.h"

TFeeRTOS_Semaphore* FeeRTOS_CreateSemaphore(int aInitialCount, int aMaxCount) {
    TFeeRTOS_Semaphore* semaphore = (TFeeRTOS_Semaphore*)malloc(sizeof(TFeeRTOS_Semaphore));
    if(semaphore == NULL) semaphore return NULL;
    semaphore->Count = aInitialCount;
    semaphore->MaxCount = aMaxCount;
    return semaphore;
}

void FeeRTOS_DeleteSemaphore(TFeeRTOS_Semaphore* aSemaphore){
    free(aSemaphore);
}

void FeeRTOS_SemaphoreTake(TFeeRTOS_Semaphore* aSemaphore){
    aSemaphore->Count--;
    if(aSemaphore->Count < 0){
        // Block the task until the semaphore is given
    }
}

void FeeRTOS_SemaphoreGive(TFeeRTOS_Semaphore* aSemaphore){
    aSemaphore->Count++;
    if(aSemaphore->Count > aSemaphore->MaxCount){
        aSemaphore->Count = aSemaphore->MaxCount; // Prevent exceeding max count
    }
    // Unblock one of the tasks waiting on the semaphore, if any
    
}