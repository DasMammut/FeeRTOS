#include "Timer.h"
 
struct TTimerStruct{
	TTimerFunction OverflowFunction;
	void *         UserDataOverflow;
	TTimerMode     TimerMode;
	float          InterruptTime;
};
 
typedef struct TTimerStruct * TTimer;
 
typedef enum{
	TIMER_PRESCALER_1,
	TIMER_PRESCALER_2,
	TIMER_PRESCALER_4,
	TIMER_PRESCALER_8,
	TIMER_PRESCALER_16,
	TIMER_PRESCALER_64,
	TIMER_PRESCALER_256,
	TIMER_PRESCALER_1024
} TTimerPrescaler;
 
unsigned int TimerPrescaler[] = {
	1, 2, 4, 8, 16, 64, 256, 1024
};
 
TTimer Timer[TIMER_NO_LAST] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
 
static bool TimerInitA_16(unsigned long	aCpuClk, TTimerMode	aTimerMode, float aInterruptTime);
static bool TimerA_16CalculatePeriod(unsigned long aCpuclk, float aInterruptTime, unsigned int* aPeriod, TTimerPrescaler* aPrescaler);
 
bool TimerInit(TTimerNo aTimerNo, unsigned long aCpuClk, TTimerMode aTimerMode, float aInterruptTime){
	if (aTimerNo >= TIMER_NO_LAST || Timer[aTimerNo] != NULL)
	return false;

	Timer[aTimerNo] = calloc(1, sizeof(struct TTimerStruct));
	if (!Timer[aTimerNo]) return false;

	memset(Timer[aTimerNo], 0, sizeof(struct TTimerStruct));

	switch (aTimerNo) {
		case TIMER_NO_A0_16:
			if (!TimerInitA_16(aCpuClk, aTimerMode, aInterruptTime)) {
				TimerDone(aTimerNo);
				return false;
			}
			break;

		default:
			TimerDone(aTimerNo);
			return false;
	}

	sei();
	return true;
}

void TimerDone(TTimerNo aTimerNo){
	if(aTimerNo >= TIMER_NO_LAST || Timer[aTimerNo] == NULL) return;
	switch(aTimerNo){
		case TIMER_NO_A0_16:
			TCA0.SINGLE.INTCTRL = 0;
			break;
		default:
			return;
	}
	free(Timer[aTimerNo]);
	Timer[aTimerNo] = NULL;
}


bool TimerSetOverflowFunction(TTimerNo aTimerNo, TTimerFunction aTimerFunction, void* aUserData){
	if (aTimerNo >= TIMER_NO_LAST || Timer[aTimerNo] == NULL)
	return false;

	Timer[aTimerNo]->OverflowFunction = aTimerFunction;
	Timer[aTimerNo]->UserDataOverflow = aUserData;
	return true;
}

void TimerSetPwmChannels(TTimerNo aTimerNo, TTimerCmpChNo aCmpChannels){
	TCA0.SINGLE.CTRLB &= ~(7<<4);
	switch(aTimerNo){
		case TIMER_NO_A0_16:
			TCA0.SINGLE.CTRLB |= (aCmpChannels << 4);
			break;
		default:
			break;
	}
}

void TimerSetDutyCycle(TTimerNo aTimerNo, TTimerCmpChNo aCmpChannel, float aDutyCycle){
	if(aDutyCycle > 1 || aDutyCycle < 0) return;
	switch(aTimerNo){
		case TIMER_NO_A0_16:
			if (aCmpChannel == TIMER_CMP_NO_0){
				TCA0.SINGLE.CMP0 = aDutyCycle * TCA0.SINGLE.PER;
			}
			else if(aCmpChannel == TIMER_CMP_NO_1){
				TCA0.SINGLE.CMP1 = aDutyCycle * TCA0.SINGLE.PER;
			}
			else if(aCmpChannel == TIMER_CMP_NO_2){
				TCA0.SINGLE.CMP2 = aDutyCycle * TCA0.SINGLE.PER;
			}
			break;
		default:
			break;
	}
}


static bool TimerInitA_16(unsigned long	aCpuClk, TTimerMode	aTimerMode, float aInterruptTime){
	unsigned int period;
	TTimerPrescaler prescaler;
	if( !TimerA_16CalculatePeriod(aCpuClk, aInterruptTime, &period, &prescaler)) return false;
	
	switch (aTimerMode){
		case TIMER_MODE_NORMAL:
			TCA0.SINGLE.PER	= period;
			TCA0.SINGLE.CTRLA = prescaler << 1;
			TCA0.SINGLE.INTCTRL |= (1<<0);
			break;
		case TIMER_MODE_PWM_SINGLE:
			TCA0.SINGLE.PER	= period;
			TCA0.SINGLE.CTRLA = prescaler << 1;
			TCA0.SINGLE.CTRLB = 0x03;
			break;
		default:
			return false;
	}

	TCA0.SINGLE.CTRLA |= (1<<0);
	return true;
}


static bool TimerA_16CalculatePeriod(unsigned long aCpuclk, float aInterruptTime, unsigned int* aPeriod, TTimerPrescaler* aPrescaler){
	if (!aPeriod || !aPrescaler || aInterruptTime <= 0 || aCpuclk == 0)
	return false;

	unsigned long top;
	int i;
	for (i = 0; i < 8; i++) {
		top = (unsigned long)((aInterruptTime * aCpuclk) / TimerPrescaler[i]) - 1;
		if (top <= 65535) break;
	}

	if (i > 7) {
		*aPeriod = 65535;
		*aPrescaler = TIMER_PRESCALER_1024;
		return false;
	}

	*aPeriod = (unsigned int)top;
	*aPrescaler = i;
	return true;
}



// Weak: kann von FeeRTOS ueberschrieben werden
void TCA0_OVF_vect(void) __attribute__((weak));

ISR(TCA0_OVF_vect){
	if (Timer[TIMER_NO_A0_16] && Timer[TIMER_NO_A0_16]->OverflowFunction)
		Timer[TIMER_NO_A0_16]->OverflowFunction(Timer[TIMER_NO_A0_16]->UserDataOverflow);

	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}
