#ifndef FEERTOS_SEMAPHORE_H
#define FEERTOS_SEMAPHORE_H

typedef struct {
    volatile int Count;
    int MaxCount;
} TFeeRTOS_Semaphore;

TFeeRTOS_Semaphore* FeeRTOS_CreateSemaphore(int aInitialCount, int aMaxCount, char* aTaskNameID);

void FeeRTOS_DeleteSemaphore(TFeeRTOS_Semaphore* aSemaphore);

void FeeRTOS_SemaphoreTake(TFeeRTOS_Semaphore* aSemaphore);

void FeeRTOS_SemaphoreGive(TFeeRTOS_Semaphore* aSemaphore);


#endif