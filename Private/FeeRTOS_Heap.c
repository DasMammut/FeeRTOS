#include "IFeeRTOS_Heap.h"

static uint8_t Heap[HEAP_SIZE];
static TFeeRTOS_HeapBlock* BlockListHead = NULL;
static TFeeRTOS_HeapBlock* BlockListTail = NULL;
static size_t FreeBytes = HEAP_SIZE; // Includes Header Overhead

static inline TFeeRTOS_HeapBlock* findSmallestMatch(size_t size);

static inline void mergeBlocks(TFeeRTOS_HeapBlock* block, TFeeRTOS_HeapBlock* previous);

void FeeRTOS_InitHeap(void) {
    BlockListHead = (TFeeRTOS_HeapBlock*)Heap;
    BlockListHead->Size = HEAP_SIZE - HeapHeaderSize;
    BlockListHead->IsFree = true;
    BlockListHead->Next = NULL;
    BlockListTail = BlockListHead;
    FreeBytes -= HeapHeaderSize;
}

void* FeeRTOS_Malloc(size_t size) {
    if (size == 0 || size > FreeBytes) return NULL;

    if(BlockListHead == NULL || BlockListTail == NULL) FeeRTOS_InitHeap();

    size = (size + 1) & ~1; // Aufrunden auf nächstes Vielfaches von 2

    TFeeRTOS_HeapBlock* current = findSmallestMatch(size);
    if(current == NULL) return NULL; // Kein passender Block gefunden

    if (current->Size > size) {
        // Block aufteilen
        TFeeRTOS_HeapBlock* newBlock = (TFeeRTOS_HeapBlock*)(((uint8_t*)current) + HeapHeaderSize + size);
        newBlock->Size = current->Size - size - HeapHeaderSize;
        newBlock->IsFree = true;
        newBlock->Next = current->Next;

        current->Size = size;
        current->IsFree = false;
        current->Next = newBlock;

        FreeBytes -= HeapHeaderSize; // Neuer Header für den neuen Block

        if (current == BlockListTail) {
            BlockListTail = newBlock;
        }
    } 
    else {
        // Gesamten Block verwenden
        current->IsFree = false;
    }
    FreeBytes -= current->Size;

    return (uint8_t*)current + HeapHeaderSize;

}

void FeeRTOS_Free(void* ptr) {
    if (ptr == NULL) return;

    TFeeRTOS_HeapBlock* current = BlockListHead;
    TFeeRTOS_HeapBlock* previous = NULL;
    while (current != NULL) {
        if ((uint8_t*)current + HeapHeaderSize == ptr) {
            break;
        }
        previous = current;
        current = current->Next;
    }
    if (current == NULL) return; // Ungültiger Pointer

    current->IsFree = true;
    FreeBytes += current->Size;

    mergeBlocks(current, previous);
}




static inline TFeeRTOS_HeapBlock* findSmallestMatch(size_t size) {
    TFeeRTOS_HeapBlock* current = BlockListHead;
    TFeeRTOS_HeapBlock* bestFit = NULL;

    while (current != NULL) {
        if (current->IsFree && (current->Size >= size + HeapHeaderSize + 2 || current->Size == size)) {
            if (bestFit == NULL || current->Size < bestFit->Size) {
                bestFit = current;
            }
        }
        current = current->Next;
    }
    return bestFit;
}

static inline void mergeBlocks(TFeeRTOS_HeapBlock* block, TFeeRTOS_HeapBlock* previous) {
    // mit dem nächsten Block zusammenführen, wenn dieser frei ist
    if (previous != NULL && previous->IsFree) {
        previous->Size += HeapHeaderSize + block->Size;
        previous->Next = block->Next;
        if (previous->Next == NULL) {
            BlockListTail = previous;
        }
        block = previous; // Für die nächste Zusammenführung
        FreeBytes += HeapHeaderSize; // Header wird entfernt
    }

    if(block->Next != NULL && block->Next->IsFree) {
        block->Size += HeapHeaderSize + block->Next->Size;
        block->Next = block->Next->Next;
        if (block->Next == NULL) {
            BlockListTail = block;
        }
        FreeBytes += HeapHeaderSize; // Header wird entfernt
    }

}