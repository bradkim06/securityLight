#include "timer.h"
#include "shell.h"
#include "metering.h"
#include "gpio.h"

#define CHECK_RXEND_PERIOD 64
#define TEST_PERIOD 32768

static uint8_t timerMode = 0;

void startTimer(uint8_t mode)
{
	uint16_t period;

	switch (mode) {
	case TIMER_METER_RX:
		period = CHECK_RXEND_PERIOD;
		break;
	case TIMER_TEST:
		period = TEST_PERIOD;
		break;
	default:
		print("Invalid Timer Mode\n");
		return;
	}

	timerMode = mode;

	//Start TIMER_A
	Timer_A_initUpModeParam initUpModeParam = { 0 };
	initUpModeParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
	initUpModeParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
	initUpModeParam.timerPeriod = period;
	initUpModeParam.timerInterruptEnable_TAIE =
		TIMER_A_TAIE_INTERRUPT_DISABLE;
	initUpModeParam.captureCompareInterruptEnable_CCR0_CCIE =
		TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
	initUpModeParam.timerClear = TIMER_A_DO_CLEAR;
	initUpModeParam.startTimer = true;
	Timer_A_initUpMode(TIMER_A0_BASE, &initUpModeParam);
}

//******************************************************************************
//
//This is the TIMER1_A0 interrupt vector service routine.
//
//***********************************// Timer 1 interrupt service routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void)
{
	volatile static int prevData = 0;

	switch (timerMode) {
	case TIMER_METER_RX: {
		if (prevData == meterRx.index) {
			Timer_A_stop(TIMER_A0_BASE);
		} else {
			prevData = meterRx.index;
		}
		break;
	}
	case TIMER_TEST: {
		print("timer test\n");
		break;
	}

	default: {
		Timer_A_stop(TIMER_A0_BASE);
		break;
	}
	}
}
