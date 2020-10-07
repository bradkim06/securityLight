#include <msp430.h>

#include "hal_system.h"

static void initialize(void);

/**
 * main.c
 */
int main(void) {
    // Stop WDT
    WDTCTL = WDTPW + WDTHOLD;

    initialize();

    return 0;
}

//*****************************************************************************
//
//! This Function Initializes all the ports on the MCU
//!
//! \return none
//
// *****************************************************************************
static void initialize(void) { hal_system_Init(); }
