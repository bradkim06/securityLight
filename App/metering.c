#include "metering.h"
#include "meter.h"
#include "driverlib.h"
#include "uart.h"
#include "timer.h"
#include "shell.h"
#include "EM_userConfig.h"
#include "lcd.h"
#include <string.h>

extern uint8_t gemResultStatus;
light_t light;

struct receive meterRx;

static uint8_t calcEVchecksum(uint8_t *from, int size)
{
	// size - 2 -> Not Calculate ETX, Checksum
	size = size - 2;
	uint8_t checksum = 0;

	for (int i = 0; i < size; i++) {
		checksum += *(from + i);
	}

	return checksum;
}

void req_read_meter(void)
{
	elecMeterReq_t *rx = (elecMeterReq_t *)meterRx.buf;

	if (rx->stx != TERMINAL_STX) {
		print("Invalid STX 0x%02X\n", rx->stx);
		return;
	}

	if (rx->etx != TERMINAL_ETX) {
		print("Invalid ETX 0x%02X\n", rx->etx);
		return;
	}

	if (rx->checkSum != calcEVchecksum((uint8_t *)&rx->stx, sizeof(elecMeterReq_t))) {
		print("Invalid checkSum\n");
		return;
	}

	memcpy(&light, &rx->light, sizeof(light_t));

	resp_read_meter();

	Calendar currentTime;

	if (rx->time.year) {
		currentTime.Year = rx->time.year;
		currentTime.Month = rx->time.mon;
		currentTime.DayOfMonth = rx->time.day;
		currentTime.Hours = rx->time.hour;
		currentTime.Minutes = rx->time.min;
		currentTime.Seconds = rx->time.sec;

		RTC_C_initCalendar(RTC_C_BASE, &currentTime, RTC_C_FORMAT_BINARY);
		RTC_C_setCalendarEvent(RTC_C_BASE, RTC_C_CALENDAREVENT_NOON);
		RTC_C_enableInterrupt(RTC_C_BASE, RTC_C_CLOCK_READ_READY_INTERRUPT +
							  RTC_C_TIME_EVENT_INTERRUPT);
		RTC_C_startClock(RTC_C_BASE);
	}

	LCD_SelectMode(currentLCDMode);
}

static void req_lcd(void)
{
	lcdReq_t *rx = (lcdReq_t *)meterRx.buf;

	if (rx->stx != TERMINAL_STX) {
		print("Invalid STX 0x%02X\n", rx->stx);
		return;
	}

	if (rx->etx != TERMINAL_ETX) {
		print("Invalid ETX 0x%02X\n", rx->etx);
		return;
	}

	if (rx->checkSum != calcEVchecksum((uint8_t *)&rx->stx, sizeof(lcdReq_t))) {
		print("Invalid checkSum\n");
		return;
	}

	runLcdCmd(rx->lcdCmd);
}

void req_lightFix(void)
{
	int len = sizeof(lightfixReq_t);
	lightfixReq_t data;
	memset(&data, 0, len);

	data.stx = METER_STX;
	data.msgType = RESP_METER_CONFIG;

	// light status
	memcpy(&data.light, &light, sizeof(light_t));

	data.checkSum = calcEVchecksum((uint8_t *)&data.stx, len);
	data.etx = METER_ETX;

	uint8_t *txBuf = &data.stx;
	for (int i = 0; i < len; i++) {
		EUSCI_A_UART_transmitData(METER_PORT, *(txBuf + i));
	}
}

void resp_read_meter(void)
{
	int len = sizeof(elecMeterResp_t);
	elecMeterResp_t data;
	memset(&data, 0, len);

	data.stx = METER_STX;
	data.msgType = RESP_READ_METER;

	// metering data
	data.status = 0; //TODO
	data.VRMS = gEmSWResult.phaseResults[EM_PH_A_IDX].VRMS;
	data.IRMS = gEmSWResult.phaseResults[EM_PH_A_IDX].IRMS / 1000;
	data.shortIRMS = gEmSWResult.phaseResults[EM_PH_NEUTRAL_IDX].IRMS / 1000;
	data.power = (int32_t)(gEmSWResult.phaseResults[EM_PH_A_IDX].activePower / 1000);
	data.cumPower = (uint32_t)(gEmSWResult.phaseResults[EM_PH_A_IDX].activeEnergy / 1000000);

	data.checkSum = calcEVchecksum((uint8_t *)&data.stx, len);
	data.etx = METER_ETX;

	uint8_t *txBuf = &data.stx;

#ifdef __TEST__
	print_EMResult();

	print("[ VRMStest       ] : %luV\n\r", (gEmSWResult.phaseResults[EM_PH_A_IDX].VRMS) / 1000);
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

	print("[ STX              ] : 0x%02X\n", data.stx);
	print("[ MSG Type         ] : 0x%02X\n", data.msgType);

	print("[ VRMS             ] : %lumV ", data.VRMS);
	print("[ Hex ] : 0x%lX\n", data.VRMS);

	print("[ IRMS             ] : %lumA ", data.IRMS);
	print("[ Hex ] : 0x%lX\n", data.IRMS);

	print("[ shortIRMS        ] : %lumA ", data.shortIRMS);
	print("[ Hex ] : 0x%lX\n", data.shortIRMS);

	print("[ Active Power     ] : %lumW ", data.power);
	print("[ Hex ] : 0x%lX\n", data.power);

	print("[ Cumulative Power ] : %lumW ", data.cumPower);
	print("[ Hex ] : 0x%lX\n", data.cumPower);
#endif
}

void checkMessage(void)
{
	switch (meterRx.buf[1]) {
	case REQ_READ_METER:
		req_read_meter();
		break;
	case REQ_LCD:
		req_lcd();
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
