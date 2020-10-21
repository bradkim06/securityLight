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

