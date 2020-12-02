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
#include "gpio.h"
#include "microrl.h"
#include "uart.h"

#include "EM_userConfig.h"

#define PRINT_BUF_SIZE 150
#define SHELL_PORT     EUSCI_A0_BASE

extern uint8_t gemResultStatus;

union converter {
  unsigned long hex;
  float         var;
};

volatile bool isShellActive = false;

// create microrl object and pointer on it
microrl_t  rl;
microrl_t* prl = &rl;

static volatile uint8_t rxData = 0;

#define _MSP_DEMO_VER "Security Light v1.0"

// definition commands word
#define _CMD_HELP1 "?"
#define _CMD_HELP2 "help"
#define _CMD_VER   "version"
#define _CMD_LIST  "list"
#define _CMD_CLEAR "clear"
#define _CMD_EXIT  "exit"

#define _CMD_RESULT "result"

// sub commands for version command
#define _SCMD_MRL  "microrl"
#define _SCMD_DEMO "demo"

#define _NUM_OF_CMD      7
#define _NUM_OF_VER_SCMD 2

// available  commands
char* keyworld[] = {_CMD_HELP1, _CMD_HELP2, _CMD_VER,   _CMD_LIST,
                    _CMD_CLEAR, _CMD_EXIT,  _CMD_RESULT};
// version subcommands
char* ver_keyworld[] = {_SCMD_MRL, _SCMD_DEMO};

// array for comletion
char* compl_world[_NUM_OF_CMD + 1];

// Initialize shell uart
static void initShellPort(void);
static void print_help(void);

void initShell(void) {
  initShellPort();
  // call init with ptr to microrl instance and print callback
  microrl_init(prl, print);
  // set callback for execute
  microrl_set_execute_callback(prl, execute);

  // set callback for completion (optionally)
  microrl_set_complete_callback(prl, complet);

  print("\033[2J");  // ESC seq for clear entire screen
  print("\033[H");   // ESC seq for move cursor at left-top corner
  print("If you want to use shell, Press Ctrl + C\n\r");  // Help
}

//*****************************************************************************

void print(const char* __restrict format, ...) {
  static char pbuf[PRINT_BUF_SIZE];

  va_list arg;
  va_start(arg, format);
  int len = vsnprintf(NULL, 0, format, arg);
  if (len > PRINT_BUF_SIZE) {
    print(RED_FONT "Print Size(%d) Overflow\n\r", PRINT_BUF_SIZE);
  }

  vsprintf(pbuf, format, arg);
  va_end(arg);

  for (int i = 0; i < len; i++) {
    EUSCI_A_UART_transmitData(SHELL_PORT, pbuf[i]);
  }
}

float convertHex2Float(unsigned long input) {
  union converter My;
  My.hex = input;
  return My.var;
}

void print_EMResult(void) {
  print(RED_FONT "EM Result print\n\r");
  print(WHITE_FONT "VRMS : %.3fV\n\r",
        ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].VRMS) / 1000);
  print("IRMS : %.3fmA\n\r",
        ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].IRMS / 1000));
  print("vPeak : %.3fV\n\r",
        ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].vPeak / 1000));
  print("iPeak : %.3fmA\n\r",
        ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].iPeak / 1000));
  print("powerFactor : %ld\n\r",
        gEmSWResult.phaseResults[EM_PH_A_IDX].powerFactor);
  print("frequency : %.2fHz\n\r",
        ((float)gEmSWResult.phaseResults[EM_PH_A_IDX].frequency) / 100);
  print("activePower : %llduW\n\r",
        gEmSWResult.phaseResults[EM_PH_A_IDX].activePower);
  print("reactivePower : %llduVAr\n\r",
        gEmSWResult.phaseResults[EM_PH_A_IDX].reactivePower);
  print("apparentPower : %llduVA\n\r",
        gEmSWResult.phaseResults[EM_PH_A_IDX].apparentPower);
  print("activeEnergy : %lluuWh\n\r",
        gEmSWResult.phaseResults[EM_PH_A_IDX].activeEnergy);
  print("reactiveEnergy : %lluuVArh\n\r",
        gEmSWResult.phaseResults[EM_PH_A_IDX].reactiveEnergy);
  print("apparentEnergy : %lluuVAh\n\r",
        gEmSWResult.phaseResults[EM_PH_A_IDX].apparentEnergy);
}

//*****************************************************************************
// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute(int argc, const char* const* argv) {
  int i = 0;
  // just iterate through argv word and compare it with your commands
  while (i < argc) {
    if (strcmp(argv[i], _CMD_HELP1) == 0) {
      print("microrl v");
      print(MICRORL_LIB_VER);
      print(_MSP_DEMO_VER);
      print("\n\r");
      print_help();  // print help
    } else if (strcmp(argv[i], _CMD_HELP2) == 0) {
      print("microrl v");
      print(MICRORL_LIB_VER);
      print(_MSP_DEMO_VER);
      print("\n\r");
      print_help();  // print help
    } else if (strcmp(argv[i], _CMD_VER) == 0) {
      if (++i < argc) {
        if (strcmp(argv[i], _SCMD_DEMO) == 0) {
          print("demo v 1.0\n\r");
        } else if (strcmp(argv[i], _SCMD_MRL) == 0) {
          print("microrl v 1.2\n\r");
        } else {
          print((char*)argv[i]);
          print(" wrong argument, see help\n\r");
        }
      } else {
        print("version needs 1 parametr, see help\n\r");
      }
    } else if (strcmp(argv[i], _CMD_LIST) == 0) {
      print("available command:\n");  // print all command per line
      for (int i = 0; i < _NUM_OF_CMD; i++) {
        print("\t");
        print(keyworld[i]);
        print("\n\r");
      }
    } else if (strcmp(argv[i], _CMD_CLEAR) == 0) {
      print("\033[2J");  // ESC seq for clear entire screen
      print("\033[H");   // ESC seq for move cursor at left-top corner
    } else if (strcmp(argv[i], _CMD_EXIT) == 0) {
      print(RED_FONT " [ EXIT SHELL ] ");
      print(WHITE_FONT "\n\r");
      print("If you want to use shell, Press Ctrl + C\n\r");  // Help
      isShellActive = false;
    } else if (strcmp(argv[i], _CMD_RESULT) == 0) {
      print_EMResult();
    } else {
      print("command: '");
      print((char*)argv[i]);
      print("' Not found.\n\r");
    }
    i++;
  }
  return 0;
}

#ifdef _USE_COMPLETE
//*****************************************************************************
// completion callback for microrl library
char** complet(int argc, const char* const* argv) {
  int j = 0;

  compl_world[0] = NULL;

  // if there is token in cmdline
  if (argc == 1) {
    // get last entered token
    char* bit = (char*)argv[argc - 1];
    // iterate through our available token and match it
    for (int i = 0; i < _NUM_OF_CMD; i++) {
      // if token is matched (text is part of our token starting from 0 char)
      if (strstr(keyworld[i], bit) == keyworld[i]) {
        // add it to completion set
        compl_world[j++] = keyworld[i];
      }
    }
  } else if ((argc > 1) && (strcmp(argv[0], _CMD_VER) ==
                            0)) {  // if command needs subcommands
    // iterate through subcommand for command _CMD_VER array
    for (int i = 0; i < _NUM_OF_VER_SCMD; i++) {
      if (strstr(ver_keyworld[i], argv[argc - 1]) == ver_keyworld[i]) {
        compl_world[j++] = ver_keyworld[i];
      }
    }
  } else {  // if there is no token in cmdline, just print all available token
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

static void initShellPort(void) {
  // RX,TX
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                             GPIO_PIN2 | GPIO_PIN3);

  shell_uartPort = Get_BaudrateConfig(EM_SMCLK_FREQ_IN_HZ);
  Init_Uart(SHELL_PORT, &shell_uartPort);
}

//*****************************************************************************
static void print_help(void) {
  print("Use TAB key for completion\n\rCommand:\n\r");
  print("\t?                   - help command\n\r");
  print("\thelp                - help command\n\r");
  print("\tversion {microrl | demo} - print version\n\r");
  print("\tlist                - list all commands in tree\n\r");
  print("\tclear               - clear screen\n\r");
  print("\texit                - exit shell\n\r");
}

//******************************************************************************
//
// This is the Shell interrupt vector service routine.
//
//******************************************************************************
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
  switch (__even_in_range(UCA0IV, USCI_UART_UCTXIFG)) {
    case USCI_UART_UCRXIFG:
      rxData = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
      if (isShellActive) { microrl_insert_char(prl, rxData); }
      // Ctrl + C
      if (rxData == 0x03 || rxData == 240) {
        prl->print(prl->prompt_str);
        isShellActive = true;
      }
      break;

    case USCI_UART_UCTXIFG: break;

    default: break;
  }
}
