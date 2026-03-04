#ifndef FEERTOS_GLOBALS_H
#define FEERTOS_GLOBALS_H

#define TICK_RATE 1e-3

#define IDLE_STACK_SIZE 128
#define IDLE_TASK_NAME "IdleTask"

#define FeeRTOS_ENTER_CRITICAL() unsigned char sreg_save = SREG; cli()
#define FeeRTOS_EXIT_CRITICAL() SREG = sreg_save

#endif