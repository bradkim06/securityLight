#include "uart.h"

void test_uart(void)
{
	EUSCI_A_UART_transmitData(EUSCI_A1_BASE, 't');
}
