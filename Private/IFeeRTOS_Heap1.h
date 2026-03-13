#include "FeeRTOS_GLOBALS.h"
#ifdef HEAP1
#ifndef I_FEERTOS_HEAP1_H
#define I_FEERTOS_HEAP1_H

#include "FeeRTOS_Heap.h"
#include "IFeeRTOS.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct SFeeRTOS_HeapBlock{
    size_t Size; // Größe des Blocks in Bytes (ohne Header)
    struct SFeeRTOS_HeapBlock* Next;
    bool IsFree;
} TFeeRTOS_HeapBlock;

static const uint8_t HeapHeaderSize = (sizeof(TFeeRTOS_HeapBlock) + 1) & ~1; // Aufrunden auf nächstes Vielfaches von 2

#endif
#endif