#ifndef FEERTOS_MUTEX_H
#define FEERTOS_MUTEX_H

typedef struct SFeeRTOS_Mutex* TFeeRTOS_MutexHandle;

/**
 * FeeRTOS_CreateMutex
 * Erstellt einen neuen Mutex.
 * Gibt ein Handle zurueck oder NULL bei Fehler.
 */
TFeeRTOS_MutexHandle FeeRTOS_CreateMutex(void);

/**
 * FeeRTOS_DeleteMutex
 * Loescht einen Mutex anhand seines Handles.
 * Gibt alle wartenden Tasks frei.
 *
 * aMutex - Handle des zu loeschenden Mutex
 */
void FeeRTOS_DeleteMutex(TFeeRTOS_MutexHandle aMutex);

/**
 * FeeRTOS_MutexLock
 * Sperrt den Mutex. Blockiert den Task falls der Mutex bereits gesperrt ist.
 *
 * aMutex - Handle des zu sperrenden Mutex
 */
void FeeRTOS_MutexLock(TFeeRTOS_MutexHandle aMutex);

/**
 * FeeRTOS_MutexUnlock
 * Gibt den Mutex wieder frei.
 *
 * aMutex - Handle des zu entsperrenden Mutex
 */
void FeeRTOS_MutexUnlock(TFeeRTOS_MutexHandle aMutex);

#endif 
