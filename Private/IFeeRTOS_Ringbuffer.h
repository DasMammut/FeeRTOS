#ifndef I_FEERTOS_RINGBUFFER_H
#define I_FEERTOS_RINGBUFFER_H

#include "FeeRTOS_Ringbuffer.h"
#include "IFeeRTOS_Heap.h"
#include <stdlib.h>

typedef struct SFeeRTOS_Ringbuffer {
    uint16_t ReadIndex;
    uint16_t WriteIndex;
    uint16_t MemSize; // In number of items
    uint8_t *Buffer;
    uint8_t ItemSize; // In bytes 
} TFeeRTOS_Ringbuffer;

#endif