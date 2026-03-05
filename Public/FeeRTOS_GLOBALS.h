#ifndef FEERTOS_GLOBALS_H
#define FEERTOS_GLOBALS_H

#define TICK_RATE 1e-3

#define IDLE_TASK_STACK_SIZE 128
#define CALLBACK_TASK_STACK_SIZE 128

#define HEAP_SIZE 5120
#define ALLOW_INTERNAL_FRAGMENTATION // Comment out to save RAM at cost of potentially more fragmentation

#endif