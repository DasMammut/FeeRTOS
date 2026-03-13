#include "FeeRTOS_GLOBALS.h"
#ifdef HEAP2
#ifndef I_FEERTOS_HEAP2_H
#define I_FEERTOS_HEAP2_H

#include "FeeRTOS_Heap.h"
#include "IFeeRTOS.h"

#include <stdint.h>
#include <stdbool.h>

typedef struct SFeeRTOS_HeapBlock{
    struct SFeeRTOS_HeapBlock* Next;
    bool IsFree;
} TFeeRTOS_HeapBlock;

static const uint8_t HeapHeaderSize = (sizeof(TFeeRTOS_HeapBlock) + 1) & ~1; // Aufrunden auf nächstes Vielfaches von 2

#endif
#endif