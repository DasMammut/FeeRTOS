#include "IFeeRTOS_Stack.h"

TStack Stack_Create(unsigned int aSize) {
	TStack stack = { .StackPointer = NULL, .Base = NULL, .Size = 0 };
	stack.Base = FeeRTOS_Malloc(aSize);
	if (stack.Base != NULL) {
		stack.Size = aSize;
		stack.StackPointer = (char*)stack.Base + aSize - 1;
	}
	return stack;
}

void Stack_Push(TStack* aStack, void* aData, unsigned int aDataSize) {
	if (aStack == NULL || aStack->Base == NULL) return;
	char* sp = (char*)aStack->StackPointer;
	char* base = (char*)aStack->Base;
	if ((sp - base) >= (int)aDataSize) {
		sp -= aDataSize;
		memcpy(sp + 1, aData, aDataSize);
		aStack->StackPointer = sp;
	}
}

void Stack_Pop(TStack* aStack, void* aData, unsigned int aDataSize) {
	if (aStack == NULL || aStack->Base == NULL) return;
	char* sp = (char*)aStack->StackPointer;
	char* topLimit = (char*)aStack->Base + aStack->Size - 1;
	if ((topLimit - sp) >= (int)aDataSize) {
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