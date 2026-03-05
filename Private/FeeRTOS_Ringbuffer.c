/**************************************************************
	File: Ringbuffer.c
	Description:
		Thread save ringbuffer for data exchange with interupt routines. 
		The size of the ringbuffer can be defined and any number of 
		ringbuffers can be used.
	Author: Fanny Fanninger
**************************************************************/
#include "IFeeRTOS_Ringbuffer.h"


TFeeRTOS_RingbufferHandle FeeRTOS_CreateRingbuffer(uint16_t aBufferSize, uint8_t aItemSize){
	if(aBufferSize == 0 || aItemSize == 0) return NULL;
	TFeeRTOS_RingbufferHandle ringbuffer = (TFeeRTOS_RingbufferHandle)FeeRTOS_Malloc(sizeof(TFeeRTOS_Ringbuffer));
	if(ringbuffer == NULL) return NULL;
	
	ringbuffer->Buffer = FeeRTOS_Malloc(aBufferSize * aItemSize);
	
	if(ringbuffer->Buffer == NULL) {
		FeeRTOS_DestroyRingbuffer(ringbuffer);
		return NULL;
	}
	
	ringbuffer->MemSize = aBufferSize;
	ringbuffer->ItemSize = aItemSize;
	return ringbuffer;
}

void FeeRTOS_DestroyRingbuffer(TFeeRTOS_RingbufferHandle aRingbuffer) {
	FeeRTOS_Free(aRingbuffer->Buffer);
	FeeRTOS_Free(aRingbuffer);
}


bool FeeRTOS_RingbufferWrite(TFeeRTOS_RingbufferHandle aRingbuffer, const void* aData){
	uint16_t newWriteIndex = aRingbuffer->WriteIndex + 1;
	
	if(newWriteIndex >= aRingbuffer->MemSize) newWriteIndex = 0;
	if(newWriteIndex == aRingbuffer->ReadIndex) return false;

	memcpy(&aRingbuffer->Buffer[aRingbuffer->WriteIndex * aRingbuffer->ItemSize], aData, aRingbuffer->ItemSize);
	aRingbuffer->WriteIndex = newWriteIndex;
	return true;
}

bool FeeRTOS_RingbufferRead(TFeeRTOS_RingbufferHandle aRingbuffer, void *aData){
	if(aRingbuffer->ReadIndex == aRingbuffer->WriteIndex) return false;

	memcpy(aData, &aRingbuffer->Buffer[aRingbuffer->ReadIndex * aRingbuffer->ItemSize], aRingbuffer->ItemSize);

	uint16_t newReadIndex = aRingbuffer->ReadIndex + 1;
	if(newReadIndex >= aRingbuffer->MemSize) newReadIndex = 0;
	aRingbuffer->ReadIndex = newReadIndex;
	return true;
}