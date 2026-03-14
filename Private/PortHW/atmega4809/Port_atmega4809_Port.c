#include "Port_atmega4809_Port.h"

static volatile uint16_t ForcedYieldSavedCNT = 0;

void Port_InitializeStack(TFeeRTOS_StackHandle aStack, void (*aTaskFunction)(void* aUserData), void* aUserData) {
    uint8_t byte;
    uint16_t pc = (uint16_t)(uintptr_t)aTaskFunction;
    byte = (uint8_t)(pc & 0xFF); // PC Low-Byte
    FeeRTOS_StackPush(aStack, &byte, 1);
    byte = (uint8_t)((pc >> 8) & 0xFF); // PC High-Byte
    FeeRTOS_StackPush(aStack, &byte, 1);

    // R1–R31 (UserData in R24:R25 — AVR Calling Convention)
    for (uint8_t r = 0; r <= 31 + 1; r++) {
        if (r == 1) // SREG // Interupts enabled
            byte = 0x80;
        else if (r == 24 + 1) // R24
            byte = (uint8_t)((uint16_t)(uintptr_t)aUserData & 0xFF);
        else if (r == 25 + 1) // R25
            byte = (uint8_t)(((uint16_t)(uintptr_t)aUserData >> 8) & 0xFF);
        else
            byte = 0x00;
        FeeRTOS_StackPush(aStack, &byte, 1);
    }
}

void Port_Yield(void) {
    ForcedYieldSavedCNT = TCA0.SINGLE.CNT; // Counter sichern
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // Pending OVF clearen
    TCA0.SINGLE.CNT = TCA0.SINGLE.PER;
}

void Port_Schedule(void) {
    if(ForcedYield) {
        TCA0.SINGLE.CNT = ForcedYieldSavedCNT; // Counter wiederherstellen
    } 
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}