/*
 * shell.c
 *
 *  Created on: 2020-10-12 Mon 11:34
 *      Author: Kim Junsu
 */
#include "shell.h"
#include <msp430.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "eusci_a_uart.h"
#include "flash.h"
#include "flashctl.h"
#include "gpio.h"
#include "microrl.h"
#include "uart.h"
#include "rtc.h"

#include "EM_userConfig.h"

#define PRINT_BUF_SIZE 150

#define TX_STATE_IDLE 0
#define TX_STATE_BUSY 1

extern uint8_t gemResultStatus;

struct shellTx {
	char buf[PRINT_BUF_SIZE];
	int len;
	int index;
	bool status;
};

static volatile struct shellTx tx;
volatile bool isShellActive = false;

// create microrl object and pointer on it
static microrl_t rl;
static microrl_t *prl = &rl;

#define _MSP_DEMO_VER "Security Light v1.0"

// definition commands word
#define _CMD_HELP1 "?"
#define _CMD_HELP2 "help"
#define _CMD_VER "version"
#define _CMD_LIST "list"
#define _CMD_CLEAR "clear"
#define _CMD_EXIT "exit"

#define _CMD_RESULT "result"
#define _CMD_PARAMETER "params"
#define _CMD_CALIBRATION "calib"

#define _NUM_OF_CMD 9
// sub commands for version command
#define _NUM_OF_VER_SCMD 2

#define _SCMD_MRL "microrl"
#define _SCMD_DEMO "demo"

// version subcommands
static char *ver_keyworld[] = { _SCMD_MRL, _SCMD_DEMO };

// sub commands for calibration command
#define _SCMD_VOLT "voltage"
#define _SCMD_CURRENT "current"
#define _SCMD_ACTIVE "active"
#define _SCMD_REACTIVE "reactive"
#define _SCMD_PHASE "phase"

// calibration subcommands
static char *calib_keyworld[] = { _SCMD_VOLT, _SCMD_CURRENT, _SCMD_ACTIVE, _SCMD_REACTIVE,
				  _SCMD_PHASE };

#define _NUM_OF_CALIB_SCMD 5

// available  commands
static char *keyworld[] = { _CMD_HELP1, _CMD_HELP2,  _CMD_VER,	       _CMD_LIST,     _CMD_CLEAR,
			    _CMD_EXIT,	_CMD_RESULT, _CMD_CALIBRATION, _CMD_PARAMETER };

// array for comletion
static char *compl_world[_NUM_OF_CMD + 1];

// Initialize shell uart
static void print_help(void);
static void print_Parameters(void);

void initShell(void)
{
	memset((void *)&tx, 0, sizeof(struct shellTx));
	// call init with ptr to microrl instance and print callback
	microrl_init(prl, print);
	// set callback for execute
	microrl_set_execute_callback(prl, execute);

	// set callback for completion (optionally)
	microrl_set_complete_callback(prl, complet);

	print("\033[2J"); // ESC seq for clear entire screen
	print("\033[H"); // ESC seq for move cursor at left-top corner
	print("If you want to use shell, Press Ctrl + C\n\r"); // Help
}

//*****************************************************************************

void print(const char *__restrict format, ...)
{
	memset((void *)&tx, 0, sizeof(struct shellTx));

	va_list arg;
	va_start(arg, format);
	tx.len = vsnprintf(NULL, 0, format, arg);
	if (tx.len > PRINT_BUF_SIZE) {
		print(RED_FONT "Print Size(%d) Overflow\n\r", PRINT_BUF_SIZE);
		return;
	}

	vsprintf((void *)tx.buf, format, arg);
	va_end(arg);

	for (int i = 0; i < tx.len; i++) {
		EUSCI_A_UART_transmitData(SHELL_PORT, tx.buf[tx.index++]);
	}
}

extern void print_EMResult(void)
{
	printf("=============================================================\n");
	print(RED_FONT);
	print("EM Result print\n\r");
	print(WHITE_FONT);
	print("[ VRMS           ] : %.3fV\n\r",
	      ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].VRMS) / 1000);
	print("[ IRMS           ] : %.3fmA\n\r",
	      ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].IRMS / 1000));
	print("[ vPeak          ] : %.3fV\n\r",
	      ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].vPeak / 1000));
	print("[ iPeak          ] : %.3fmA\n\r",
	      ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].iPeak / 1000));
	print("[ powerFactor    ] : %ld\n\r", gEmSWResult.phaseResults[EM_PH_A_IDX].powerFactor);
	print("[ frequency      ] : %.2fHz\n\r",
	      ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].frequency) / 100);
	print("[ activePower    ] : %llduW\n\r", gEmSWResult.phaseResults[EM_PH_A_IDX].activePower);
	print("[ reactivePower  ] : %llduVAr\n\r",
	      gEmSWResult.phaseResults[EM_PH_A_IDX].reactivePower);
	print("[ apparentPower  ] : %llduVA\n\r",
	      gEmSWResult.phaseResults[EM_PH_A_IDX].apparentPower);
	print("[ activeEnergy   ] : %lluuWh\n\r",
	      gEmSWResult.phaseResults[EM_PH_A_IDX].activeEnergy);
	print("[ reactiveEnergy ] : %lluuVArh\n\r",
	      gEmSWResult.phaseResults[EM_PH_A_IDX].reactiveEnergy);
	print("[ apparentEnergy ] : %lluuVAh\n\r",
	      gEmSWResult.phaseResults[EM_PH_A_IDX].apparentEnergy);
	printf("=============================================================\n");
}

void print_rtc(void)
{
	print("Date %u-%u-%u, Time %u:%u:%u\n", rtcData.Year, rtcData.Month + 1, rtcData.DayOfMonth,
	      rtcData.Hours, rtcData.Minutes, rtcData.Seconds);
}

static void print_Parameters(void)
{
	print("voltageSF = %lu\n\r", g_emPhaseCalibration[0].voltageSF);
	print("currentSF = %lu\n\r", g_emPhaseCalibration[0].currentSF);
	print("activePowerSF = %lu\n\r", g_emPhaseCalibration[0].activePowerSF);
	print("reactivePowerSF = %lu\n\r", g_emPhaseCalibration[0].reactivePowerSF);
	print("phaseCorrection = %d\n\r", g_emPhaseCalibration[0].phaseCorrection);
}

void calibrateFactor(const char *const *argv)
{
	// if (gEmSWResult.phaseResults[EM_PH_A_IDX].VRMS < 10000) {
	//   print("Can't calibration because Input Voltage is lower than 10V
	//   %luuV\n\r",
	//         gEmSWResult.phaseResults[EM_PH_A_IDX].VRMS);
	//   return;
	// }

	float calibRate;

	if (strcmp(argv[1], "voltage") == 0) {
		calibRate = (atof(argv[2]) * 1000) / gEmSWResult.phaseResults[EM_PH_A_IDX].VRMS;
		calibRate = (float)g_emPhaseCalibration[0].voltageSF * calibRate;
		g_emPhaseCalibration[0].voltageSF = (uint32_t)calibRate;
	} else if (strcmp(argv[1], "current") == 0) {
		calibRate = (atof(argv[2]) * 1000) / gEmSWResult.phaseResults[EM_PH_A_IDX].IRMS;
		calibRate = (float)g_emPhaseCalibration[0].currentSF * calibRate;
		g_emPhaseCalibration[0].currentSF = (uint32_t)calibRate;
	} else if (strcmp(argv[1], "active") == 0) {
		calibRate = (atof(argv[2]) * 1000000) /
			    gEmSWResult.phaseResults[EM_PH_A_IDX].activePower;
		calibRate = (float)g_emPhaseCalibration[0].activePowerSF * calibRate;
		g_emPhaseCalibration[0].activePowerSF = (uint32_t)calibRate;
	} else if (strcmp(argv[1], "reactive") == 0) {
		calibRate = (atof(argv[2]) * 1000000) /
			    gEmSWResult.phaseResults[EM_PH_A_IDX].reactivePower;
		calibRate = (float)g_emPhaseCalibration[0].reactivePowerSF * calibRate;
		g_emPhaseCalibration[0].reactivePowerSF = (uint32_t)calibRate;
	} else if (strcmp(argv[1], "phase") == 0) {
		print("phase corretion not working yet\n\r");
		return;
	} else {
		print("argument is wrong\n\r");
		return;
	}

	memcpy(&flash.calib, &g_emPhaseCalibration, sizeof(EM_Phase_Calibration));
	writeFlashCalib();
}

//*****************************************************************************
// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute(int argc, const char *const *argv)
{
	int i = 0;
	// just iterate through argv word and compare it with your commands
	while (i < argc) {
		if (strcmp(argv[i], _CMD_HELP1) == 0) {
			print("microrl v");
			print(MICRORL_LIB_VER);
			print(_MSP_DEMO_VER);
			print("\n\r");
			print_help(); // print help
		} else if (strcmp(argv[i], _CMD_HELP2) == 0) {
			print("microrl v");
			print(MICRORL_LIB_VER);
			print(_MSP_DEMO_VER);
			print("\n\r");
			print_help(); // print help
		} else if (strcmp(argv[i], _CMD_VER) == 0) {
			if (++i < argc) {
				if (strcmp(argv[i], _SCMD_DEMO) == 0) {
					print("demo v 1.0\n\r");
				} else if (strcmp(argv[i], _SCMD_MRL) == 0) {
					print("microrl v 1.2\n\r");
				} else {
					print((char *)argv[i]);
					print(" wrong argument, see help\n\r");
				}
			} else {
				print("version needs 1 parametr, see help\n\r");
			}
		} else if (strcmp(argv[i], _CMD_LIST) == 0) {
			print("available command:\n"); // print all command per line
			for (int i = 0; i < _NUM_OF_CMD; i++) {
				print("\t");
				print(keyworld[i]);
				print("\n\r");
			}
		} else if (strcmp(argv[i], _CMD_CLEAR) == 0) {
			print("\033[2J"); // ESC seq for clear entire screen
			print("\033[H"); // ESC seq for move cursor at left-top corner
		} else if (strcmp(argv[i], _CMD_EXIT) == 0) {
			print(RED_FONT " [ EXIT SHELL ] ");
			print(WHITE_FONT "\n\r");
			print("If you want to use shell, Press Ctrl + C\n\r"); // Help
			isShellActive = false;
		} else if (strcmp(argv[i], _CMD_RESULT) == 0) {
			print_EMResult();
		} else if (strcmp(argv[i], _CMD_CALIBRATION) == 0) {
			calibrateFactor(argv);
			i += argc;
		} else if (strcmp(argv[i], _CMD_PARAMETER) == 0) {
			print_Parameters();
		} else {
			print("command: '");
			print((char *)argv[i]);
			print("' Not found.\n\r");
			break;
		}
		i++;
	}
	return 0;
}

#ifdef _USE_COMPLETE
//*****************************************************************************
// completion callback for microrl library
char **complet(int argc, const char *const *argv)
{
	int j = 0;

	compl_world[0] = NULL;

	// if there is token in cmdline
	if (argc == 1) {
		// get last entered token
		char *bit = (char *)argv[argc - 1];
		// iterate through our available token and match it
		for (int i = 0; i < _NUM_OF_CMD; i++) {
			// if token is matched (text is part of our token starting from 0 char)
			if (strstr(keyworld[i], bit) == keyworld[i]) {
				// add it to completion set
				compl_world[j++] = keyworld[i];
			}
		}
	} else if ((argc > 1) && (strcmp(argv[0], _CMD_VER) == 0)) {
		// iterate through subcommand for command _CMD_VER array
		for (int i = 0; i < _NUM_OF_VER_SCMD; i++) {
			if (strstr(ver_keyworld[i], argv[argc - 1]) == ver_keyworld[i]) {
				compl_world[j++] = ver_keyworld[i];
			}
		}
	} else if ((argc > 1) && (strcmp(argv[0], _CMD_CALIBRATION) == 0)) {
		for (int i = 0; i < _NUM_OF_CALIB_SCMD; i++) {
			if (strstr(calib_keyworld[i], argv[argc - 1]) == calib_keyworld[i]) {
				compl_world[j++] = calib_keyworld[i];
			}
		}
	} else { // if there is no token in cmdline, just print all available token
		for (; j < _NUM_OF_CMD; j++) {
			compl_world[j] = keyworld[j];
		}
	}

	// note! last ptr in array always must be NULL!!!
	compl_world[j] = NULL;
	// return set of variants
	return compl_world;
}
#endif
EUSCI_A_UART_initParam shell_uartPort;

//*****************************************************************************
static void print_help(void)
{
	print("Use TAB key for completion\n\rCommand:\n\r");
	print("\t?                   - help command\n\r");
	print("\thelp                - help command\n\r");
	print("\tversion {microrl | demo} - print version\n\r");
	print("\tlist                - list all commands in tree\n\r");
	print("\tclear               - clear screen\n\r");
	print("\texit                - exit shell\n\r");
	print("\tresult              - all measuring result\n\r");
	print("\tcalib               - [calib] [param1](voltage,current... "
	      "etc) [param2](value)\n\r");
	print("\tparams              - parameter value\n\r");
}

//******************************************************************************
//
// This is the Shell interrupt vector service routine.
//
//******************************************************************************
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
	switch (__even_in_range(UCA0IV, USCI_UART_UCTXIFG)) {
	case USCI_UART_UCRXIFG: {
		volatile uint8_t rxData = 0;

		rxData = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
		if (isShellActive) {
			microrl_insert_char(prl, rxData);
		}
		// Ctrl + C
		else if (rxData == 0x03) {
			prl->print(prl->prompt_str);
			isShellActive = true;
		}
		break;
	}

	case USCI_UART_UCTXIFG: {
		/* if (tx.index < tx.len) { */
		/* 	EUSCI_A_UART_transmitData(SHELL_PORT, */
		/* 				  tx.buf[tx.index++]); */
		/* 	tx.status = TX_STATE_BUSY; */
		/* } else { */
		/* 	tx.status = TX_STATE_IDLE; */
		/* } */
		break;
	}

	default:

		break;
	}
}
