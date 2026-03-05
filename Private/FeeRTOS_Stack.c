#include "IFeeRTOS_Stack.h"

TStack Stack_Create(uint16_t aSize) {
	TStack stack = { .StackPointer = NULL, .Base = NULL, .Size = 0 };
	stack.Base = FeeRTOS_Malloc(aSize);
	if (stack.Base != NULL) {
		stack.Size = aSize;
	stack.StackPointer = (uint8_t*)stack.Base + aSize - 1;
	}
	return stack;
}

void Stack_Push(TStack* aStack, void* aData, uint16_t aDataSize) {
	if (aStack == NULL || aStack->Base == NULL) return;
	uint8_t* sp = (uint8_t*)aStack->StackPointer;
	uint8_t* base = (uint8_t*)aStack->Base;
	if ((sp - base) >= (int16_t)aDataSize) {
		sp -= aDataSize;
		memcpy(sp + 1, aData, aDataSize);
		aStack->StackPointer = sp;
	}
}

void Stack_Pop(TStack* aStack, void* aData, uint16_t aDataSize) {
	if (aStack == NULL || aStack->Base == NULL) return;
	uint8_t* sp = (uint8_t*)aStack->StackPointer;
	uint8_t* topLimit = (uint8_t*)aStack->Base + aStack->Size - 1;
	if ((topLimit - sp) >= (int16_t)aDataSize) {
		memcpy(aData, sp + 1, aDataSize);
		sp += aDataSize;
		aStack->StackPointer = sp;
	}
}

void Stack_Destroy(TStack* aStack){
    if (aStack->Base == NULL) return;
    free(aStack->Base);
    aStack->Base = NULL;
    aStack->StackPointer = NULL;
    aStack->Size = 0;
}