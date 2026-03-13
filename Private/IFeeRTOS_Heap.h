#ifndef I_FEERTOS_HEAP_H
#define I_FEERTOS_HEAP_H

#include "FeeRTOS_GLOBALS.h"

#if defined(HEAP1)
#include "IFeeRTOS_Heap1.h"
#elif defined(HEAP2)
#include "IFeeRTOS_Heap2.h"

#endif

#endif