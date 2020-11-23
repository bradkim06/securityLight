/**
 * @file uart.c
 * @brief
 * @author Kim Junsu
 * @date 2020-10-07
 */

#include "uart.h"

void Init_Uart(uint16_t baseAddress, EUSCI_A_UART_initParam* params) {
  EUSCI_A_UART_disable(baseAddress);

  EUSCI_A_UART_init(baseAddress, params);
  EUSCI_A_UART_enable(baseAddress);
  EUSCI_A_UART_clearInterrupt(baseAddress, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
  EUSCI_A_UART_enableInterrupt(baseAddress, EUSCI_A_UART_RECEIVE_INTERRUPT);
}

EUSCI_A_UART_initParam Get_BaudrateConfig(int baudrate) {
  EUSCI_A_UART_initParam config;

  config.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
  config.parity            = EUSCI_A_UART_NO_PARITY;
  config.msborLsbFirst     = EUSCI_A_UART_LSB_FIRST;
  config.numberofStopBits  = EUSCI_A_UART_ONE_STOP_BIT;
  config.uartMode          = EUSCI_A_UART_MODE;

  switch (baudrate) {
    // 250000 Baud Rate @ 25MHz
    case BAUDRATE_250K:
      config.clockPrescalar = 0x06;
      config.firstModReg    = 0x03;
      config.secondModReg   = 0x02;
      config.overSampling   = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
      break;
    // 115200 Baud Rate @ 25MHz
    case BAUDRATE_115200:
      config.clockPrescalar = 4;
      config.firstModReg    = 7;
      config.secondModReg   = 4;
      config.overSampling   = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
      break;

    default: break;
  }

  return config;
}
