#ifndef __tmp_reg__
    #define __tmp_reg__ 0
#endif

#include <avr/io.h>

.extern SavedSP
.extern Schedule

.global TCA0_OVF_vect
.global Port_RestoreContext

; --- Hilfsmakro zum Sichern ---
.macro SAVE_CONTEXT
    push r0
    in   r0, _SFR_IO_ADDR(SREG) 
    push r0

    push r1
    push r2
    push r3
    push r4
    push r5
    push r6
    push r7
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push r16
    push r17
    push r18
    push r19
    push r20
    push r21
    push r22
    push r23
    push r24
    push r25
    push r26
    push r27
    push r28
    push r29
    push r30
    push r31
    
    in   r16, _SFR_IO_ADDR(SPL)
    in   r17, _SFR_IO_ADDR(SPH)
    sts  SavedSP, r16
    sts  SavedSP+1, r17
.endm

; --- Hilfsmakro zum Wiederherstellen ---
.macro RESTORE_CONTEXT
    lds  r16, SavedSP
    lds  r17, SavedSP+1
    out  _SFR_IO_ADDR(SPH), r17
    out  _SFR_IO_ADDR(SPL), r16
    
    pop  r31
    pop  r30
    pop  r29
    pop  r28
    pop  r27
    pop  r26
    pop  r25
    pop  r24
    pop  r23
    pop  r22
    pop  r21
    pop  r20
    pop  r19
    pop  r18
    pop  r17
    pop  r16
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  r7
    pop  r6
    pop  r5
    pop  r4
    pop  r3
    pop  r2
    pop  r1

    pop  r0
    out  _SFR_IO_ADDR(SREG), r0
    pop  r0
.endm

; --- Der eigentliche Interrupt-Einstieg ---
TCA0_OVF_vect:
    SAVE_CONTEXT
    call Schedule
    RESTORE_CONTEXT
    reti

; --- Einstiegspunkt für den allerersten Taskstart ---
Port_RestoreContext:
    RESTORE_CONTEXT
    reti