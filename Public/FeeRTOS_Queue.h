#ifndef FEERTOS_QUEUE_H
#define FEERTOS_QUEUE_H

#include <stdint.h>

typedef struct SFeeRTOS_Queue* TFeeRTOS_QueueHandle;

/**
 * FeeRTOS_CreateQueue
 * Erstellt eine neue Queue mit angegebener Laenge und Item-Groesse.
 * Gibt ein Handle zurueck oder NULL bei Fehler.
 *
 * aQueueLength - Anzahl der Items
 * aItemSize    - Groesse eines Items in Bytes
 */
TFeeRTOS_QueueHandle FeeRTOS_CreateQueue(uint16_t aQueueLength, uint8_t aItemSize);

/**
 * FeeRTOS_DeleteQueue
 * Loescht eine Queue anhand ihres Handles.
 * Gibt alle wartenden Tasks frei.
 *
 * aQueue - Handle der zu loeschenden Queue
 */
void FeeRTOS_DeleteQueue(TFeeRTOS_QueueHandle aQueue);

/**
 * FeeRTOS_QueueSend
 * Sendet ein Item an die Queue.
 *
 * aQueue - Handle der Queue
 * aItem  - Zeiger auf das zu sendende Item
 */
void FeeRTOS_QueueSend(TFeeRTOS_QueueHandle aQueue, const void* aItem);

/**
 * FeeRTOS_QueueReceive
 * Empfaengt ein Item aus der Queue.
 *
 * aQueue - Handle der Queue
 * aItem  - Zeiger auf den Speicher fuer das empfangene Item
 */
void FeeRTOS_QueueReceive(TFeeRTOS_QueueHandle aQueue, void* aItem);


#endif