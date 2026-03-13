#include "IFeeRTOS_TickTimer.h"

static const uint16_t TimerPrescaler[] = {
	1, 2, 4, 8, 16, 64, 256, 1024
};

static bool calculatePeriod(float aInterruptTime, uint16_t* aPeriod, TFeeRTOS_TickTimerPrescaler* aPrescaler);

bool FeeRTOS_SetupTickTimer(float aTickRate) {
    if (aTickRate <= 0) return false;

    uint16_t period;
    TFeeRTOS_TickTimerPrescaler prescaler;
    if (!calculatePeriod(aTickRate, &period, &prescaler)) {
        return false; // Ungültige Periode oder Prescaler
    }
    
    // Periode setzen
    TCA0.SINGLE.PER = period;
    TCA0.SINGLE.CTRLA = (prescaler << TCA_SINGLE_CLKSEL0_bp);

    // Interrupt aktivieren (Overflow)
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;

    // Prescaler 64 und Timer starten
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;

    return true;
}

void FeeRTOS_StopTickTimer(void) {
    TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
    TCA0.SINGLE.INTCTRL &= ~TCA_SINGLE_OVF_bm;
}

static bool calculatePeriod(float aInterruptTime, uint16_t* aPeriod, TFeeRTOS_TickTimerPrescaler* aPrescaler){
	if (!aPeriod || !aPrescaler || aInterruptTime <= 0) return false;

	uint32_t top;
	int8_t i;
	for (i = 0; i < 8; i++) {
		top = (uint32_t)((aInterruptTime * F_CPU) / TimerPrescaler[i]) - 1;
		if (top <= 65535) break;
	}

	if (i > 7) {
		*aPeriod = 65535;
		*aPrescaler = TIMER_PRESCALER_1024;
		return false;
	}

	*aPeriod = (uint16_t)top;
	*aPrescaler = (TFeeRTOS_TickTimerPrescaler)i;
	return true;
}

void TCA0_OVF_vect(void) __attribute__((weak));
