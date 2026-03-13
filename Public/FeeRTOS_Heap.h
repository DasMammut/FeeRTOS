#ifndef FEERTOS_HEAP_H
#define FEERTOS_HEAP_H

#include <stddef.h>

/**
 * FeeRTOS_InitHeap
 * Initialisiert den Heap-Speicher.
 * Wird automatisch vor der ersten Speicherallokation aufgerufen.
 */
void FeeRTOS_InitHeap(void);

/**
 * FeeRTOS_Malloc
 * Allokiert Speicher aus dem Heap.
 * Gibt Zeiger auf Speicherbereich oder NULL bei Fehler.
 *
 * size - Groesse des Speicherbereichs in Bytes
 */
void* FeeRTOS_Malloc(size_t size);

/**
 * FeeRTOS_Free
 * Gibt einen zuvor allokierten Speicherbereich wieder frei.
 *
 * ptr - Zeiger auf den freizugebenden Speicher
 */
void FeeRTOS_Free(void* ptr);

#endif