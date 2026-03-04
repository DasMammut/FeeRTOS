#ifndef FEERTOS_SEMAPHORE_H
#define FEERTOS_SEMAPHORE_H

typedef struct TSemaphore* TFeeRTOS_SemaphoreHandle;

/*
 * FeeRTOS_CreateSemaphore
 * Erstellt eine Semaphore mit einem Anfangswert und einem Maximalwert.
 * Gibt ein Handle zurueck oder NULL bei Fehler.
 */
TFeeRTOS_SemaphoreHandle FeeRTOS_CreateSemaphore(int aInitialCount, int aMaxCount);

/*
 * FeeRTOS_DeleteSemaphore
 * Loescht eine Semaphore und gibt alle wartenden Tasks frei.
 */
void FeeRTOS_DeleteSemaphore(TFeeRTOS_SemaphoreHandle aSemaphore);

/*
 * FeeRTOS_SemaphoreTake
 * Dekrementiert die Semaphore. Wenn Count == 0, wird der aufrufende
 * Task blockiert (SemaphoreBlocked) bis ein anderer Task Give aufruft.
 */
void FeeRTOS_SemaphoreTake(TFeeRTOS_SemaphoreHandle aSemaphore);

/*
 * FeeRTOS_SemaphoreGive
 * Inkrementiert die Semaphore. Wenn Tasks warten, wird der erste
 * wartende Task entblockt statt den Counter zu erhoehen.
 */
void FeeRTOS_SemaphoreGive(TFeeRTOS_SemaphoreHandle aSemaphore);

#endif