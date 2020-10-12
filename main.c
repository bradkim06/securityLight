/**
 * @file main.c
 * @brief
 * @author Kim Junsu
 * @date 2020-10-07
 */

#include <msp430.h>

#include "EMLibApp.h"
#include "comm.h"
#include "hal_system.h"
#include "shell.h"
#include "wdt_a.h"

static void initialize(void);

//*****************************************************************************
//
//! This Function Initializes all the ports on the MCU
//!
//! \return none
//
// *****************************************************************************
static void initialize(void) {
  initSystem();
  initComm();
  initShell();
}

/**
 * main.c
 */
int main(void) {
  // Stop WDT
  WDT_A_hold(WDT_A_BASE);
  /** WDTCTL = WDTPW + WDTHOLD; */

  initialize();

  initEMLibGUIApp();

  while (true) { }

  return 0;
}

