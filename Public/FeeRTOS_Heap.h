#ifndef FEERTOS_HEAP_H
#define FEERTOS_HEAP_H

#include <stddef.h>

void FeeRTOS_InitHeap(void);

void* FeeRTOS_Malloc(size_t size);

void FeeRTOS_Free(void* ptr);

#endif