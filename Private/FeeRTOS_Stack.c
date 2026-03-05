#include "IFeeRTOS_Stack.h"

TFeeRTOS_StackHandle Stack_Create(uint16_t aSize) {
	TFeeRTOS_StackHandle stack = (TFeeRTOS_StackHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Stack));
	if (stack == NULL) return NULL;
	stack->Base = FeeRTOS_Malloc(aSize);
	if (stack->Base != NULL) {
		stack->Size = aSize;
		stack->StackPointer = (uint8_t*)stack->Base + aSize - 1;
	}
	else {
		free(stack);
		return NULL;
	}
	return stack;
}

void Stack_Push(TFeeRTOS_StackHandle aStack, void* aData, uint16_t aDataSize) {
	if (aStack == NULL || aStack->Base == NULL) return;
	uint8_t* sp = (uint8_t*)aStack->StackPointer;
	uint8_t* base = (uint8_t*)aStack->Base;
	if ((sp - base) >= (int16_t)aDataSize) {
		sp -= aDataSize;
		memcpy(sp + 1, aData, aDataSize);
		aStack->StackPointer = sp;
	}
}

void Stack_Pop(TFeeRTOS_StackHandle aStack, void* aData, uint16_t aDataSize) {
	if (aStack == NULL || aStack->Base == NULL) return;
	uint8_t* sp = (uint8_t*)aStack->StackPointer;
	uint8_t* topLimit = (uint8_t*)aStack->Base + aStack->Size - 1;
	if ((topLimit - sp) >= (int16_t)aDataSize) {
		memcpy(aData, sp + 1, aDataSize);
		sp += aDataSize;
		aStack->StackPointer = sp;
	}
}

void Stack_Destroy(TFeeRTOS_StackHandle aStack){
    if (aStack == NULL || aStack->Base == NULL) return;
    free(aStack->Base);
    aStack->Base = NULL;
    aStack->StackPointer = NULL;
    aStack->Size = 0;
    free(aStack);
}
}