#include "IFeeRTOS_Queue.h"

TFeeRTOS_QueueHandle FeeRTOS_CreateQueue(uint16_t aQueueLength, uint8_t aItemSize) {
    if (aQueueLength == 0 || aItemSize == 0) return NULL;
    TFeeRTOS_QueueHandle queue = (TFeeRTOS_QueueHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Queue));
    if (queue == NULL) return NULL;

    queue->buffer = FeeRTOS_CreateRingbuffer(aQueueLength, aItemSize);
    if (queue->buffer == NULL) {
        FeeRTOS_Free(queue);
        return NULL;
    }

    queue->ItemsAvailable = FeeRTOS_CreateSemaphore(0, aQueueLength);
    queue->SpaceAvailable = FeeRTOS_CreateSemaphore(aQueueLength, aQueueLength);

    if (queue->ItemsAvailable == NULL || queue->SpaceAvailable == NULL) {
        FeeRTOS_DeleteSemaphore(queue->ItemsAvailable);
        FeeRTOS_DeleteSemaphore(queue->SpaceAvailable);
        FeeRTOS_DestroyRingbuffer(queue->buffer);
        FeeRTOS_Free(queue);
        return NULL;
    }

    return queue;
}

void FeeRTOS_DeleteQueue(TFeeRTOS_QueueHandle aQueue) {
    if (aQueue == NULL) return;

    FeeRTOS_DeleteSemaphore(aQueue->ItemsAvailable);
    FeeRTOS_DeleteSemaphore(aQueue->SpaceAvailable);
    FeeRTOS_DestroyRingbuffer(aQueue->buffer);
    FeeRTOS_Free(aQueue);
}

void FeeRTOS_QueueSend(TFeeRTOS_QueueHandle aQueue, const void* aItem) {
    if (aQueue == NULL || aItem == NULL) return;

    FeeRTOS_SemaphoreTake(aQueue->SpaceAvailable);
    FeeRTOS_RingbufferWrite(aQueue->buffer, aItem);
    FeeRTOS_SemaphoreGive(aQueue->ItemsAvailable);
}

void FeeRTOS_QueueReceive(TFeeRTOS_QueueHandle aQueue, void* aItem) {
    if (aQueue == NULL || aItem == NULL) return;

    FeeRTOS_SemaphoreTake(aQueue->ItemsAvailable);
    FeeRTOS_RingbufferRead(aQueue->buffer, aItem);
    FeeRTOS_SemaphoreGive(aQueue->SpaceAvailable);
}

