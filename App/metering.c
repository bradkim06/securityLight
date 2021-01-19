#include "metering.h"
#include "meter.h"
#include "driverlib.h"
#include "uart.h"
#include "shell.h"
#include "EM_userConfig.h"
#include <string.h>

#define RX_MAX_SIZE 100

char rxBuf[RX_MAX_SIZE];
char *txBuf;

/* union converter { */
/* 	unsigned long hex; */
/* 	float var; */
/* }; */

/* static float convertHex2Float(unsigned long input) */
/* { */
/* 	union converter My; */
/* 	My.hex = input; */
/* 	return My.var; */
/* } */

/* static float convertuint32tohex(unsigned long input) */
/* { */
/* 	union converter My; */
/* 	My.var = input; */
/* 	uint8_t *test; */

/* 	test = (uint8_t *)&My.hex; */
/* 	print("%ld\n", input); */
/* 	print("0x%lX\n", My.hex); */

/* 	for (int i = 0; i < 4; i++) { */
/* 		print("0x%02X ", *(test + i)); */
/* 	} */

/* 	return My.hex; */
/* } */

static uint8_t calc_checksum(uint8_t *from, int size)
{
	uint8_t checksum = 0;
	for (int i = 0; i < size; i++) {
		checksum += *(from + i);
	}

	return checksum;
}

void resp_read_meter(void)
{
	int len = sizeof(elecMeterResp_t);
	elecMeterResp_t data;

	memset(&data, 0, len);
	data.stx = METER_STX;
	data.msgType = RESP_READ_METER;
	memcpy(&data.results, &gEmSWResult.phaseResults[EM_PH_A_IDX],
	       sizeof(EM_Phase_Results));
	data.results = gEmSWResult.phaseResults[EM_PH_A_IDX];
	data.checkSum = calc_checksum((uint8_t *)&data.stx, len - 1);
	data.etx = METER_ETX;

	uint8_t *txBuf = &data.stx;

	print(RED_FONT "RESP_READ_METER \n\r");
	print(WHITE_FONT);

	for (int i = 0; i < len; i++) {
		EUSCI_A_UART_transmitData(METER_PORT, *(txBuf + i));
		print("0x%02x ", *(txBuf + i));
		if ((i + 1) % 10 == 0) {
			print("\n");
		}
	}

	print("\n\n");

	print(RED_FONT "EM Result data print\n\r");
	print(WHITE_FONT);

	txBuf = &data.stx;
	print("STX : 0x%02X\n", *txBuf);

	txBuf = &data.msgType;
	print("MSG Type : 0x%02X\n", *txBuf);

	txBuf = (uint8_t *)&data.results.VRMS;
	print("VRMS : %lumV ", data.results.VRMS);
	print("Hex : 0x%lX\n", *((uint32_t *)txBuf));

	txBuf = (uint8_t *)&data.results.IRMS;
	print("IRMS : %luuA ", data.results.IRMS);
	print("Hex : 0x%lX\n", *((uint32_t *)txBuf));

	txBuf = (uint8_t *)&data.results.vPeak;
	print("vPeak : %lumV ", data.results.vPeak);
	print("Hex : 0x%lX\n", *((uint32_t *)txBuf));

	txBuf = (uint8_t *)&data.results.iPeak;
	print("iPeak : %luuA ", data.results.iPeak);
	print("Hex : 0x%lX\n", *((uint32_t *)txBuf));

	txBuf = (uint8_t *)&data.results.powerFactor;
	print("powerFactor : %lu ", data.results.powerFactor);
	print("Hex : 0x%lX\n", *((int32_t *)txBuf));

	txBuf = (uint8_t *)&data.results.frequency;
	print("Frequency : %uHz ", data.results.frequency);
	print("Hex : 0x%lX\n", *((uint16_t *)txBuf));

	txBuf = (uint8_t *)&data.results.activePower;
	print("activePower : %llduW ", data.results.activePower);
	print("Hex : 0x%llX\n", *((int64_t *)txBuf));

	txBuf = (uint8_t *)&data.results.reactivePower;
	print("reactivePower : %llduVAr ", data.results.reactivePower);
	print("Hex : 0x%llX\n", *((int64_t *)txBuf));

	txBuf = (uint8_t *)&data.results.apparentPower;
	print("apparentPower : %llduVA ", data.results.apparentPower);
	print("Hex : 0x%llX\n", *((int64_t *)txBuf));

	txBuf = (uint8_t *)&data.results.activeEnergy;
	print("activeEnergy : %lluuWh ", data.results.activeEnergy);
	print("Hex : 0x%llX\n", *((uint64_t *)txBuf));

	txBuf = (uint8_t *)&data.results.reactiveEnergy;
	print("reactiveEnergy : %lluuVArh ", data.results.reactiveEnergy);
	print("Hex : 0x%llX\n", *((uint64_t *)txBuf));

	txBuf = (uint8_t *)&data.results.apparentEnergy;
	print("apparentEnergy : %lluuVAh ", data.results.apparentEnergy);
	print("Hex : 0x%llX\n", *((uint64_t *)txBuf));

	txBuf = (uint8_t *)&data.results.phaseID;
	print("phaseID : %d ", data.results.phaseID);
	print("Hex : 0x%d\n", *((int *)txBuf));
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
