#include "comm.h"

#include "gpio.h"
#include "uart.h"

#define COMM_PORT EUSCI_A0_BASE

void initComm(void) {
  /* P1.2: UCA0 UART RX & P1.3: UCA0 UART TX  */
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                             GPIO_PIN2 | GPIO_PIN3);
  EUSCI_A_UART_initParam Comm_uartPort = Get_BaudrateConfig(BAUDRATE_250K);
  Init_Uart(COMM_PORT, &Comm_uartPort);
}

//******************************************************************************
//
// This is the comm interrupt vector service routine.
//
//******************************************************************************
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
  switch (__even_in_range(UCA1IV, USCI_UART_UCTXIFG)) {
    case USCI_NONE: break;

    case USCI_UART_UCRXIFG: break;

    case USCI_UART_UCTXIFG: break;

    default: break;
  }
}
