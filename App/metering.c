#include "metering.h"
#include "meter.h"
#include "driverlib.h"
#include "uart.h"
#include "shell.h"

#define RX_MAX_SIZE 100

char rxBuf[RX_MAX_SIZE];
char *txBuf;

void testMeterTx(void)
{
	static unsigned int i = 0, k = 0;

	if (k++ > 30000) {
		k = 0;
		EUSCI_A_UART_transmitData(METER_PORT, 'a');
		print("testing...%d\n", i++);
	}
}

//******************************************************************************
//
// This is the Meter Communication interrupt vector service routine.
//
//******************************************************************************
#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	switch (__even_in_range(UCA0IV, USCI_UART_UCTXIFG)) {
	case USCI_UART_UCRXIFG: {
		volatile uint8_t rxData = 0;

		rxData = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
		break;
	}

	case USCI_UART_UCTXIFG: {
		break;
	}

	default:
		break;
	}
}
