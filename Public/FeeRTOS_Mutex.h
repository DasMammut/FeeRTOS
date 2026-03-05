#ifndef FEERTOS_MUTEX_H
#define FEERTOS_MUTEX_H

typedef struct SFeeRTOS_Mutex* TFeeRTOS_MutexHandle;

TFeeRTOS_MutexHandle FeeRTOS_CreateMutex(void);

void FeeRTOS_DeleteMutex(TFeeRTOS_MutexHandle aMutex);

void FeeRTOS_MutexLock(TFeeRTOS_MutexHandle aMutex);

void FeeRTOS_MutexUnlock(TFeeRTOS_MutexHandle aMutex);

#endif 
