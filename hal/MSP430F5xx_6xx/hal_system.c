//#############################################################################
//
//! \file   hal_system.c
//!
//! \brief  Hardware Abstraction Layer for MSP system including clocks,
//!         watchdog, and GPIOs
//!
//!  Target Device:  MSP430F6736
//!  Author: Kim Junsu
//!  Date: 2020-10-07 Wed 11:13
//!
//#############################################################################

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"
#include "hal.h"
#include "lcd_c.h"
#include "msp430f6736a.h"

#define GPIO_PRIMARY_MODULE_FUNCTION (0x01)
#define GPIO_SECONDARY_MODULE_FUNCTION (0x02)
#define GPIO_TERNARY_MODULE_FUNCTION (0x03)

//
// Local function prototypes
//
static void hal_system_ClockInit(uint32_t mclkFreq);
static void test_clockinit(void);
static void gpio_init(void);

//
// Function declarations
//
void initSystem(void)
{
	gpio_init();
	hal_system_ClockInit(EM_SMCLK_FREQ_IN_HZ);
	/* test_clockinit(); */
}

//
// Local Function declarations
//
//*****************************************************************************
//
//! Initializes the system clocks
//!
//! This function initializes crystals, DCO and ACLK,SMCLK,MCLK
//!
//! \return none
//
// *****************************************************************************
static void gpio_init(void)
{
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN6);
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
						  GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
	GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
						  GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
	GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
						  GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
						  GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
	GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0);
	GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN6 | GPIO_PIN7);
	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3);

	GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN6);
	GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
						     GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
						     GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
						     GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
						     GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0);
	GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6 | GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3);
}

static void test_clockinit(void)
{
	// Setup LFXT1
	UCSCTL6 &= ~(XT1OFF); // XT1 On
	UCSCTL6 |= XCAP_3; // Internal load cap

	// Loop until XT1 fault flag is cleared
	do {
		UCSCTL7 &= ~XT1LFOFFG; // Clear XT1 fault flags
	} while (UCSCTL7 & XT1LFOFFG); // Test XT1 fault flag

	// Set external clock frequency to 32.768 KHz
	UCS_setExternalClockSource(32768, 0);

	PMM_setVCore(PMM_CORE_LEVEL_2);

	// Setup UCS
	UCSCTL3 |= SELREF_0; // Set DCO FLL reference = REFO
	UCSCTL4 |= SELA_0; // Set ACLK = REFO

	__bis_SR_register(SCG0); // Disable the FLL control loop
	UCSCTL0 = 0x0000; // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_5; // Select DCO range 16MHz operation

	UCSCTL2 = FLLD_1 | 511; // Set DCO Multiplier for 16777216Hz
	__bic_SR_register(SCG0); // Enable the FLL control loop
	__delay_cycles(524300);

	// Loop until XT1, XT2 & DCO fault flag is cleared
	do {
		UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG);
		// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG; // Clear fault flags
	} while (SFRIFG1 & OFIFG); // Test oscillator fault flag
}

static void hal_system_ClockInit(uint32_t mclkFreq)
{
	PMM_setVCore(PMM_CORE_LEVEL_3);

	UCS_turnOnLFXT1(UCS_XT1_DRIVE_3, UCS_XCAP_3);
	UCS_setExternalClockSource(32768, 0);

	UCS_initClockSignal(UCS_FLLREF, UCS_XT1CLK_SELECT, UCS_CLOCK_DIVIDER_1);

	UCS_initClockSignal(UCS_ACLK, UCS_XT1CLK_SELECT, UCS_CLOCK_DIVIDER_1);

	UCS_initFLLSettle(mclkFreq / 1000, mclkFreq / 32768);
}
