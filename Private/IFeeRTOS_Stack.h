#ifndef I_FEERTOS_STACK_H
#define I_FEERTOS_STACK_H

#include "FeeRTOS_Stack.h"

#include "IFeeRTOS.h"

typedef struct SFeeRTOS_Stack{
    void* StackPointer;
    void* Base;
    uint16_t Size;
} TFeeRTOS_Stack;

#endif