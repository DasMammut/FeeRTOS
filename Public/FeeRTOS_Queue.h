#ifndef FEERTOS_QUEUE_H
#define FEERTOS_QUEUE_H

#include <stdint.h>

typedef struct SFeeRTOS_Queue* TFeeRTOS_QueueHandle;

TFeeRTOS_QueueHandle FeeRTOS_CreateQueue(uint16_t aQueueLength, uint8_t aItemSize);

void FeeRTOS_DeleteQueue(TFeeRTOS_QueueHandle aQueue);

void FeeRTOS_QueueSend(TFeeRTOS_QueueHandle aQueue, const void* aItem);

void FeeRTOS_QueueReceive(TFeeRTOS_QueueHandle aQueue, void* aItem);


#endif