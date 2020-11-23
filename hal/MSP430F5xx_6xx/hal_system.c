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

//
// Local function prototypes
//
static void hal_system_ClockInit(uint32_t mclkFreq);
static void hal_system_GPIOInit(void);

//
// Function declarations
//
void initSystem(void) {
  /** hal_system_GPIOInit(); */
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
static void hal_system_ClockInit(uint32_t mclkFreq) {
  PMM_setVCore(PMM_CORE_LEVEL_3);
  /** UCS_turnOnLFXT1(UCS_XT1_DRIVE_0, UCS_XCAP_0); */
  /** UCSCTL3 |= SELREF_2;  // Set DCO FLL reference = REFO */
  /** UCSCTL4 |= SELA_2;    // Set ACLK = REFO */
  /** UCS_initFLLSettle(mclkFreq / 1000, mclkFreq / 32768); */

  // Setup P1.0 output, J1.0 SMCLK, J1.1 MCLK, J1.3 ACLK
  P1DIR |= BIT0;                // Set P1.0 to output direction
  P1OUT &= ~BIT0;               // Clear P1.0
  PJDIR |= BIT0 | BIT1 | BIT3;  // ACLK, MCLK, SMCLK set out to pins
  PJSEL |= BIT0 | BIT1 | BIT3;  // PJ.0,1,3 for debugging purposes.

  // Setup UCS
  UCSCTL3 |= SELREF_2;  // Set DCO FLL reference = REFO
  UCSCTL4 |= SELA_2;    // Set ACLK = REFO

  __bis_SR_register(SCG0);  // Disable the FLL control loop
  UCSCTL0 = 0x0000;         // Set lowest possible DCOx, MODx
  UCSCTL1 = DCORSEL_5;      // Select DCO range 16MHz operation
  /** UCSCTL2 = FLLD_1 | 243;   // Set DCO Multiplier for 8MHz */
  UCSCTL2 = FLLD_1 | 255;   // Set DCO Multiplier for 8MHz
                            // (N + 1) * FLLRef = Fdco
                            // (243 + 1) * 32768 = 8MHz
                            // Set FLL Div = fDCOCLK/2
  __bic_SR_register(SCG0);  // Enable the FLL control loop

  // Worst-case settling time for the DCO when the DCO range bits have been
  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
  // UG for optimization.
  // 32 x 32 x 8 MHz / 32,768 Hz = 250000 = MCLK cycles for DCO to settle
  __delay_cycles(1000000);

  // Loop until XT1, XT2 & DCO fault flag is cleared
  do {
    UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG);
    // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;        // Clear fault flags
  } while (SFRIFG1 & OFIFG);  // Test oscillator fault flag
}

//*****************************************************************************
//
//! Initializes GPIOs
//!
//! \return none
//
// *****************************************************************************
static void hal_system_GPIOInit(void) {
  uint8_t i;

  // LED 1,2 Init
  for (i = 0; i < gEmSWConfig.systemConfig->pulseArrayLength; i++) {
    GPIO_setAsOutputPin(
        gEmSWConfig.systemConfig->pulseArray[i].activePulseGpioPort,
        gEmSWConfig.systemConfig->pulseArray[i].activePulseGpioPin);

    GPIO_setAsOutputPin(
        gEmSWConfig.systemConfig->pulseArray[i].reactivePulseGpioPort,
        gEmSWConfig.systemConfig->pulseArray[i].reactivePulseGpioPin);

    GPIO_setOutputHighOnPin(
        gEmSWConfig.systemConfig->pulseArray[i].activePulseGpioPort,
        gEmSWConfig.systemConfig->pulseArray[i].activePulseGpioPin);

    GPIO_setOutputHighOnPin(
        gEmSWConfig.systemConfig->pulseArray[i].reactivePulseGpioPort,
        gEmSWConfig.systemConfig->pulseArray[i].reactivePulseGpioPin);
  }
}
