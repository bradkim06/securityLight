/**
 * @file uart.c
 * @brief
 * @author Kim Junsu
 * @date 2020-10-07
 */

#include "uart.h"
#include "EM_userConfig.h"
#include "shell.h"

void Init_Uart(uint16_t baseAddress, EUSCI_A_UART_initParam* params) {
  EUSCI_A_UART_disable(baseAddress);

  EUSCI_A_UART_init(baseAddress, params);
  EUSCI_A_UART_enable(baseAddress);
  EUSCI_A_UART_clearInterrupt(baseAddress, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
  EUSCI_A_UART_enableInterrupt(baseAddress, EUSCI_A_UART_RECEIVE_INTERRUPT);
}

EUSCI_A_UART_initParam Get_BaudrateConfig(uint32_t smclk) {
  EUSCI_A_UART_initParam config;

  config.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
  config.parity            = EUSCI_A_UART_NO_PARITY;
  config.msborLsbFirst     = EUSCI_A_UART_LSB_FIRST;
  config.numberofStopBits  = EUSCI_A_UART_ONE_STOP_BIT;
  config.uartMode          = EUSCI_A_UART_MODE;

  switch (smclk) {
    // 115200 Baud Rate @ 25MHz
    case CLK_25MHZ:
      config.clockPrescalar = 13;
      config.firstModReg    = 10;
      config.secondModReg   = 0;
      config.overSampling   = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
      break;
    // 115200 Baud Rate @ 16MHz
    case CLK_16MHZ:
      config.clockPrescalar = 9;
      config.firstModReg    = 2;
      config.secondModReg   = 0;
      config.overSampling   = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
      break;

    // 115200 Baud Rate @ 16MHz
    case CLK_8MHZ:
      config.clockPrescalar = 4;
      config.firstModReg    = 7;
      config.secondModReg   = 4;
      config.overSampling   = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
      break;

    default:
      print("Baudrate Setting Error Out of acceptable clock range\n\r");
      break;
  }

  return config;
}
