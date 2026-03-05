#ifndef FEERTOS_RINGBUFFER_H
#define FEERTOS_RINGBUFFER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct SFeeRTOS_Ringbuffer* TFeeRTOS_RingbufferHandle;

// Item size in bytes, buffer size in number of items
TFeeRTOS_RingbufferHandle FeeRTOS_CreateRingbuffer(uint16_t aBufferSize, uint8_t aItemSize);

void FeeRTOS_DestroyRingbuffer(TFeeRTOS_RingbufferHandle aRingbuffer);

bool FeeRTOS_RingbufferWrite(TFeeRTOS_RingbufferHandle aRingbuffer, const void* aData);

bool FeeRTOS_RingbufferRead(TFeeRTOS_RingbufferHandle aRingbuffer, void *aData);

#endif