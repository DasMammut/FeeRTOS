#ifndef PORT_ATMEGA4809_PORT_H
#define PORT_ATMEGA4809_PORT_H

#include <stdint.h>
#include "IFeeRTOS_Stack.h"

extern volatile bool ForcedYield;

void Port_InitializeStack(TFeeRTOS_StackHandle aStack, void (*aTaskFunction)(void* aUserData), void* aUserData);

void Port_Yield(void);

void Port_Schedule(void);

#endif