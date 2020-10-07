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

#include "hal.h"

//
// Local function prototypes
//
static void hal_system_ClockInit(uint32_t mclkFreq);
static void hal_system_GPIOInit(void);

//
// Function declarations
//
void hal_system_Init(void) {
    hal_system_ClockInit(EM_SMCLK_FREQ_IN_HZ);
    hal_system_GPIOInit();
}

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

    UCS_turnOnLFXT1(UCS_XT1_DRIVE_0, UCS_XCAP_0);

    UCS_initClockSignal(UCS_FLLREF, UCS_XT1CLK_SELECT, UCS_CLOCK_DIVIDER_1);

    UCS_initClockSignal(UCS_ACLK, UCS_XT1CLK_SELECT, UCS_CLOCK_DIVIDER_1);

    UCS_initFLLSettle(mclkFreq / 1000, mclkFreq / 32768);
}

//
// Local Function declarations
//
//*****************************************************************************
//
//! Initializes GPIOs
//!
//! \return none
//
// *****************************************************************************
static void hal_system_GPIOInit(void) {
    uint8_t i;

    // Configure all the phase GPIOs
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

    /* P1.2: UCA0 UART RX & P1.3: UCA0 UART TX  */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN2 | GPIO_PIN3);
}
