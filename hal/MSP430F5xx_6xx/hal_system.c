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

//
// Function declarations
//
void initSystem(void)
{
	hal_system_ClockInit(EM_SMCLK_FREQ_IN_HZ);
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
static void hal_system_ClockInit(uint32_t mclkFreq)
{
	// Initialize LFXT1
	/* UCSCTL6 &= ~(XT1OFF); // Enable XT1 */
	/* UCSCTL6 |= XCAP_3; // Internal load cap */
	/* UCSCTL6 &= XT1DRIVE_0; */
	/* UCSCTL6 &= XT1DRIVE_1; */

	/* UCSCTL4 |= SELA__XT1CLK; // Set ACLK = REFO */
	/* UCS_setExternalClockSource(32768, 0); */

	PMM_setVCore(PMM_CORE_LEVEL_3);

	// Setup UCS
	UCSCTL3 |= SELREF_2; // Set DCO FLL reference = REFO
	UCSCTL4 |= SELA_2; // Set ACLK = REFO

	__bis_SR_register(SCG0); // Disable the FLL control loop
	UCSCTL0 = 0x0000; // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_5; // Select DCO range 16MHz operation

	// Worst-case settling time for the DCO when the DCO range bits have been
	// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
	// UG for optimization.
	// 32 x 32 x 8 MHz / 32,768 Hz = 250000 = MCLK cycles for DCO to settle
	//
	// (N + 1) * FLLRef = Fdco
	// (243 + 1) * 32768 = 8MHz
	// Set FLL Div = fDCOCLK/2
	switch (mclkFreq) {
	/** Clock 25Mhz Not Working */
	case CLK_25MHZ:
		UCSCTL2 = FLLD_1 | 767; // Set DCO Multiplier for 25165824Hz
		__bic_SR_register(SCG0); // Enable the FLL control loop
		__delay_cycles(786432);
		break;
	case CLK_16MHZ:
		UCSCTL2 = FLLD_1 | 511; // Set DCO Multiplier for 16777216Hz
		__bic_SR_register(SCG0); // Enable the FLL control loop
		__delay_cycles(524288);
		break;
	case CLK_8MHZ:
		UCSCTL2 = FLLD_1 | 255; // Set DCO Multiplier for 8388608Hz
		__bic_SR_register(SCG0); // Enable the FLL control loop
		__delay_cycles(262144);
		break;
	default:
		break;
	}

	// Loop until XT1, XT2 & DCO fault flag is cleared
	do {
		UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG);
		// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG; // Clear fault flags
	} while (SFRIFG1 & OFIFG); // Test oscillator fault flag
}
