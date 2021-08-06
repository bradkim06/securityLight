/**
 * @file uart.c
 * @brief
 * @author Kim Junsu
 * @date 2020-10-07
 */

#include "uart.h"
#include "EM_userConfig.h"
#include "shell.h"
#include "gpio.h"

static bool Init_UartDev(uint16_t baseAddress, EUSCI_A_UART_initParam *params)
{
	bool retVal = STATUS_SUCCESS;

	EUSCI_A_UART_disable(baseAddress);
	retVal = EUSCI_A_UART_init(baseAddress, params);
	EUSCI_A_UART_enable(baseAddress);
	EUSCI_A_UART_clearInterrupt(baseAddress, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
	EUSCI_A_UART_enableInterrupt(baseAddress, EUSCI_A_UART_RECEIVE_INTERRUPT);

	return retVal;
}

void initUart(void)
{
	// Shell Uart
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3);

	EUSCI_A_UART_initParam params;
	params = Get_BaudrateConfig(EM_SMCLK_FREQ_IN_HZ, SHELL_BAUDRATE);
	Init_UartDev(SHELL_PORT, &params);

	// Meter
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN4 | GPIO_PIN5);
	params = Get_BaudrateConfig(EM_SMCLK_FREQ_IN_HZ, METER_BAUDRATE);
	Init_UartDev(METER_PORT, &params);
}

EUSCI_A_UART_initParam Get_BaudrateConfig(uint32_t smclk, uint32_t baudrate)
{
	EUSCI_A_UART_initParam config;

	config.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
	config.parity = EUSCI_A_UART_NO_PARITY;
	config.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
	config.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
	config.uartMode = EUSCI_A_UART_MODE;

	switch (smclk) {
	// smclk 25165824Hz
	case CLK_25MHZ:
		switch (baudrate) {
		case 115200:
			config.clockPrescalar = 13;
			config.firstModReg = 10;
			config.secondModReg = 0;
			config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
			break;
		case 38400:
			config.clockPrescalar = 40;
			config.firstModReg = 15;
			config.secondModReg = 0;
			config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
			break;
		case 9600:
			config.clockPrescalar = 163;
			config.firstModReg = 13;
			config.secondModReg = 0;
			config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
			break;
		default:
			print("Baudrate Setting Error Out of acceptable range\n\r");
			break;
		}
		break;
	// smclk 16MHz
	case CLK_16MHZ:
		switch (baudrate) {
		case 115200:
			config.clockPrescalar = 9;
			config.firstModReg = 2;
			config.secondModReg = 0;
			config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
			break;
		default:
			print("Baudrate Setting Error Out of acceptable range\n\r");
			break;
		}
		break;

	// smclk 8MHz
	case CLK_8MHZ:
		switch (baudrate) {
		case 115200:
			config.clockPrescalar = 4;
			config.firstModReg = 7;
			config.secondModReg = 4;
			config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
			break;
		default:
			print("Baudrate Setting Error Out of acceptable range\n\r");
			break;
		}
		break;

	default:
		print("Clock Setting Error Out of acceptable range\n\r");
		break;
	}

	return config;
}
