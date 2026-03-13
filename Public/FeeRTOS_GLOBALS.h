#ifndef FEERTOS_GLOBALS_H
#define FEERTOS_GLOBALS_H

#ifndef F_CPU
#error "F_CPU must be defined before including FeeRTOS_GLOBALS.h"
#endif

#define TICK_RATE 1e-3
#define TICK_FREQ (1.0 / TICK_RATE)

/**
 * Stack-Groesse des Idle-Tasks in Bytes
 */
#define IDLE_TASK_STACK_SIZE 128
/**
 * Stack-Groesse des Callback-Tasks in Bytes
 */
#define CALLBACK_TASK_STACK_SIZE 128

/**
 * Definieren welche HEAP Implementierung verwendet werden soll (HEAP1 / HEAP2)
 */
#define HEAP1
/**
 * Heap-Groesse in Bytes
 */
#define HEAP_SIZE 5120
/**
 * Erlaubt interne Fragmentierung im Heap
 * Auskommentieren spart RAM, kann aber zu mehr Fragmentierung fuehren
 */
#define ALLOW_INTERNAL_FRAGMENTATION // Comment out to save RAM at cost of potentially more fragmentation



#endif