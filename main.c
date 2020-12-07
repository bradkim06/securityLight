/**
 * @file main.c
 * @brief
 * @author Kim Junsu
 * @date 2020-10-07
 */

#include <msp430.h>
#include <string.h>

#include "EMLibApp.h"
#include "EM_userConfig.h"
#include "flash.h"
#include "gpio.h"
#include "hal_system.h"
#include "shell.h"
#include "wdt_a.h"

#ifdef __TEST__
#include "test_clock.h"
#include "test_led.h"
#include "test_uart.h"
#endif

uint8_t gemResultStatus;

static void initialize(void);
static void readFlash(void);

/**
 * main.c
 */
int main(void) {
  // Stop WDT
  WDT_A_hold(WDT_A_BASE);

  initialize();

  initEMLibGUIApp();

  WDT_A_initWatchdogTimer(WDT_A_BASE, WDT_A_CLOCKSOURCE_VLOCLK,
                          WDT_A_CLOCKDIVIDER_512K);
  WDT_A_start(WDT_A_BASE);

  while (true) {
    WDT_A_resetTimer(WDT_A_BASE);
    EM_foregroundProc(&gemResultStatus);

#ifdef __TEST__
    if (gemResultStatus) {
      //     print_EMResult();
      /** test_led(); */
    }
#endif
  }
}

//*****************************************************************************
//
//! This Function Initializes all the ports on the MCU
//!
//! \return none
//
// *****************************************************************************
static void initialize(void) {
  initSystem();
  initShell();
  readFlash();
#ifdef __TEST__
  test_clock();
#endif
}

static void readFlash(void) {
  if (confirmChecksum(FLASH_D_ADDR, sizeof(flash_t) - 1)) {
    memcpy(&flash, FLASH_D_ADDR, sizeof(flash_t));
    memcpy(&g_emPhaseCalibration, &flash.calib, sizeof(EM_Phase_Calibration));
#ifdef __TEST__
    print("read flash!\n\r");
    print("copy Calibration Factor!\n\r");
#endif
  }
};

