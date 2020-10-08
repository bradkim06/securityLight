#include "comm.h"

#include "uart.h"
#include "uart_config.h"

void Setup_comm(void) {
  EUSCI_A_UART_initParam Comm_uartPort = Get_BaudrateConfig(BAUDRATE_250K);
  Init_Uart(UART__EUSCI_A_PERIPHERAL, &Comm_uartPort);
}
