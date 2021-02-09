#include "metering.h"
#include "meter.h"
#include "driverlib.h"
#include "uart.h"
#include "timer.h"
#include "shell.h"
#include "EM_userConfig.h"
#include <string.h>

extern uint8_t gemResultStatus;

struct receive meterRx;

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

static uint8_t calcEVchecksum(uint8_t *from, int size)
{
	uint8_t checksum = 0;
	// size - 2 -> Not Calculate ETX, Checksum
	for (int i = 0; i < size - 2; i++) {
		checksum += *(from + i);
	}

	return checksum;
}

void req_read_meter(void)
{
	elecMeterReq_t *rx = (elecMeterReq_t *)meterRx.buf;

	if (rx->stx != TERMINAL_STX) {
		print("Invalid STX\n");
		return;
	}

	if (rx->etx != TERMINAL_ETX) {
		print("Invalid ETX\n");
		return;
	}

	if (rx->checkSum != calcEVchecksum((uint8_t *)&rx->stx, sizeof(elecMeterReq_t))) {
		print("Invalid checkSum\n");
		return;
	}

	HAL_ADC_startConversion(&halAdcConfig);
	EM_foregroundProc(&gemResultStatus);

	resp_read_meter();
}

void resp_read_meter(void)
{
	int len = sizeof(elecMeterResp_t);
	elecMeterResp_t data;
	memset(&data, 0, len);

	data.stx = METER_STX;
	data.msgType = RESP_READ_METER;
	memcpy(&data.results, &gEmSWResult.phaseResults[EM_PH_A_IDX], sizeof(EM_Phase_Results));
	data.results = gEmSWResult.phaseResults[EM_PH_A_IDX];
	data.checkSum = calcEVchecksum((uint8_t *)&data.stx, len);
	data.etx = METER_ETX;

	uint8_t *txBuf = &data.stx;

	for (int i = 0; i < len; i++) {
		EUSCI_A_UART_transmitData(METER_PORT, *(txBuf + i));
	}

	print(RED_FONT "RESP_READ_METER \n\r");
	print(WHITE_FONT);

	for (int i = 0; i < len; i++) {
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

void checkMessage(void)
{
	print("checkMsg\n");

	for (int i = 0; i < meterRx.index; i++) {
		print("meterRx.buf[%d] = %x\n", i, meterRx.buf[i]);
	}

	switch (meterRx.buf[1]) {
	case REQ_READ_METER:
		print("req read meter\n");
		req_read_meter();
		break;
	}

	memset(&meterRx, 0, sizeof(meterRx));
}

//******************************************************************************
//
// This is the Meter Communication interrupt vector service routine.
//
//******************************************************************************
#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	switch (__even_in_range(UCA1IV, USCI_UART_UCTXIFG)) {
	case USCI_UART_UCRXIFG: {
		meterRx.buf[meterRx.index] = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);

		if (meterRx.buf[0] == TERMINAL_STX) {
			if (meterRx.index == 0) {
				meterRx.index++;
				startTimer(TIMER_METER_RX);
			} else if (meterRx.index >= LEN_MAX_METER_RX) {
				Timer_A_stop(TIMER_A0_BASE);
				memset(&meterRx, 0, sizeof(meterRx));
			} else {
				meterRx.index++;
			}
		}
		break;
	}

	case USCI_UART_UCTXIFG: {
		break;
	}

	default:
		break;
	}
}
