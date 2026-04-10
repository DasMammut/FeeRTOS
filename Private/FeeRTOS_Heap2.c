#include "IFeeRTOS_Heap2.h"

#ifdef HEAP2

static uint8_t Heap[HEAP_SIZE];
static TFeeRTOS_HeapBlock* BlockListHead = NULL;
static size_t FreeBytes = HEAP_SIZE; // Includes Header Overhead

static inline TFeeRTOS_HeapBlock* findSmallestMatch(size_t size);

static inline void mergeBlocks(TFeeRTOS_HeapBlock* block, TFeeRTOS_HeapBlock* previous);

static inline size_t getBlockSize(TFeeRTOS_HeapBlock* block);

void FeeRTOS_InitHeap(void) {
    BlockListHead = (TFeeRTOS_HeapBlock*)Heap;
    BlockListHead->Next = NULL;
    BlockListHead->IsFree = true;
    FreeBytes = HEAP_SIZE- HeapHeaderSize; // Erster Block belegt Header, aber ist frei
}

void* FeeRTOS_Malloc(size_t size) {
    if (size == 0 || size > FreeBytes) return NULL;

    FeeRTOS_ENTER_CRITICAL();

    if(BlockListHead == NULL) FeeRTOS_InitHeap();

    size = (size + 1) & ~1; // Aufrunden auf nächstes Vielfaches von 2

    TFeeRTOS_HeapBlock* current = findSmallestMatch(size);
    if(current == NULL) {
        FeeRTOS_EXIT_CRITICAL();
        return NULL;
    }

    if (getBlockSize(current) >= size + HeapHeaderSize + 2) { // Genug Platz für Aufteilung (mind. 2 Bytes im Restblock)
        // Block aufteilen
        TFeeRTOS_HeapBlock* newBlock = (TFeeRTOS_HeapBlock*)(((uint8_t*)current) + HeapHeaderSize + size);
        newBlock->IsFree = true;
        newBlock->Next = current->Next;

        current->Next = newBlock;

        FreeBytes -= HeapHeaderSize; // Neuer Header für den neuen Block

    } 
    current->IsFree = false;
    FreeBytes -= getBlockSize(current);

    FeeRTOS_EXIT_CRITICAL();
    return (uint8_t*)current + HeapHeaderSize;

}

void FeeRTOS_Free(void* ptr) {
    if (ptr == NULL) return;

    FeeRTOS_ENTER_CRITICAL();

    TFeeRTOS_HeapBlock* current = BlockListHead;
    TFeeRTOS_HeapBlock* previous = NULL;
    while (current != NULL) {
        if ((uint8_t*)current + HeapHeaderSize == ptr) {
            break;
        }
        previous = current;
        current = current->Next;
    }
    if (current == NULL) {
        FeeRTOS_EXIT_CRITICAL();
        return; // Ungültiger Pointer
    }

    current->IsFree = true;
    FreeBytes += getBlockSize(current);

    mergeBlocks(current, previous);

    FeeRTOS_EXIT_CRITICAL();
}




static inline TFeeRTOS_HeapBlock* findSmallestMatch(size_t size) {
    TFeeRTOS_HeapBlock* current = BlockListHead;
    TFeeRTOS_HeapBlock* bestFit = NULL;

    #ifndef ALLOW_INTERNAL_FRAGMENTATION
    while (current != NULL) {
        if (current->IsFree && (getBlockSize(current) >= size + HeapHeaderSize + 2 || getBlockSize(current) == size)) {
            if (bestFit == NULL || getBlockSize(current) < getBlockSize(bestFit)) {
                bestFit = current;
            }
        }
        current = current->Next;
    }
    #endif
    #ifdef ALLOW_INTERNAL_FRAGMENTATION
    while (current != NULL) {
        if (current->IsFree && getBlockSize(current) >= size) {
            if (bestFit == NULL || getBlockSize(current) < getBlockSize(bestFit)) {
                bestFit = current;
            }
        }
        current = current->Next;
    }
    #endif

    return bestFit;
}

static inline void mergeBlocks(TFeeRTOS_HeapBlock* block, TFeeRTOS_HeapBlock* previous) {
    // mit dem nächsten Block zusammenführen, wenn dieser frei ist
    if (previous != NULL && previous->IsFree) {
        previous->Next = block->Next;
        if (previous->Next == NULL) {
        }
        block = previous; // Für die nächste Zusammenführung
        FreeBytes += HeapHeaderSize; // Header wird entfernt
    }

    if(block->Next != NULL && block->Next->IsFree) {
        block->Next = block->Next->Next;
        if (block->Next == NULL) {
        }
        FreeBytes += HeapHeaderSize; // Header wird entfernt
    }

}

static inline size_t getBlockSize(TFeeRTOS_HeapBlock* block) {
    if (block->Next == NULL) return ((uint8_t*)Heap + HEAP_SIZE) - ((uint8_t*)block + HeapHeaderSize);
    return (uint8_t*)block->Next - ((uint8_t*)block + HeapHeaderSize);
}

#endif