#ifndef I_FEERTOS_STACK_H
#define I_FEERTOS_STACK_H

#include "FeeRTOS_Stack.h"

#include "IFeeRTOS.h"

typedef struct SFeeRTOS_Stack{
    uint16_t Size;
    void* StackPointer;
    void* Base;
} TFeeRTOS_Stack;

#endif