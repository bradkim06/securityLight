/**
 * @file main.c
 * @brief
 * @author Kim Junsu
 * @date 2020-10-07
 */

#include <msp430.h>

#include "EMLibApp.h"
#include "EM_userConfig.h"
#include "flash.h"
#include "gpio.h"
#include "hal_system.h"
#include "lcd.h"
#include "rtc.h"
#include "shell.h"
#include "wdt_a.h"
#include "uart.h"
#include "metering.h"

#ifdef __TEST__
#include "test_clock.h"
#include "test_led.h"
#include "test_uart.h"
#endif

uint8_t gemResultStatus;

//*****************************************************************************
//! This Function Initializes all the ports on the MCU
// *****************************************************************************
static void initialize(void)
{
	initSystem();
	initUart();

	/* /1* turn on global interrupts *1/ */
	/* __enable_interrupt(); */

	initShell();
	initLCD();
	initRTC();
	readFlash();
#ifdef __TEST__
	test_clock();
#endif
}

int main(void)
{
	WDT_A_initWatchdogTimer(WDT_A_BASE, WDT_A_CLOCKSOURCE_VLOCLK,
				WDT_A_CLOCKDIVIDER_512K);
	WDT_A_start(WDT_A_BASE);

	initialize();
	initEMLibGUIApp();

	while (true) {
		WDT_A_resetTimer(WDT_A_BASE);
		EM_foregroundProc(&gemResultStatus);

#ifdef __TEST__
		if (gemResultStatus) {
			/* print_EMResult(); */
			/* LCD_SelectMode(++currentLCDMode); */
			/* print("second = %d, minute = %d\n", */
			/*       HWREG8(RTC_C_BASE + OFS_RTCTIM0_L), RTCMIN); */
			resp_read_meter();
		}
		/* rtcData = RTC_C_getCalendarTime(RTC_C_BASE); */
		// lcd_updateAll();
#endif
	}
}
