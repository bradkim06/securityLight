/**
 * @file main.c
 * @brief
 * @author Kim Junsu
 * @date 2020-10-07
 */

#include <msp430.h>

#include "comm.h"
#include "hal_system.h"

static void initialize(void);

//*****************************************************************************
//
//! This Function Initializes all the ports on the MCU
//!
//! \return none
//
// *****************************************************************************
static void initialize(void) {
  system_Init();
  Setup_comm();
}

/**
 * main.c
 */
int main(void) {
  // Stop WDT
  WDTCTL = WDTPW + WDTHOLD;

  initialize();

  return 0;
}

