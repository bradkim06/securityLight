/**
 * @file uart.h
 * @brief
 * @author Kim Junsu
 * @date 2020-10-07
 */

#ifndef __UART_H__
#define __UART_H__

#include <stdbool.h>
#include <stdint.h>

#include "eusci_a_uart.h"

#define SHELL_PORT EUSCI_A0_BASE
#define METER_PORT EUSCI_A1_BASE

//*****************************************************************************
// COMM RAM Buffer Definitions and Global Variables
//*****************************************************************************

//! \def Receive queue buffer size
#define COMM_RX_BUFF_SZ (64)

//! \def Transmit buffer size
#define COMM_TX_BUFF_SZ (64)

//! \var The transmit buffer
extern uint8_t Comm_transmitBuffer[COMM_TX_BUFF_SZ];

//! \var The receive buffer
extern uint8_t Comm_receiveBuffer[COMM_RX_BUFF_SZ];

//*****************************************************************************
//
//! \enum tUARTStates enumerates the possible UART driver states.
//
//*****************************************************************************
enum tUARTStates { eUARTIsClosed, eUARTIsIdle, eUARTIsTransmitting };
//*****************************************************************************
//
//! \typedef tUARTPort defines the port definition that the user must
//! provide when instantiating this driver.  This structure is read-only from
//! the context of the driver, and may be placed in read-only memory, such as
//! the C CONST section, if desired.
//
//*****************************************************************************
typedef struct {
	//
	//! Receive Callback.  This function will be called
	//! after each UART byte is received.  This fxn is called
	//! from the foreground (ISR), and as such it should be kept
	//! as short as possible.
	//
	bool (*pbReceiveCallback)(uint8_t);

	//
	//! Error Callback.  This function will be called after
	//! a UART error is detected.  The passed tUARTErrors
	//! parameter indicates what caused the error.
	//
	bool (*pbErrorCallback)(uint8_t);

	//
	//! eUSCI_A initialization parameters.  This structure
	//! is defined in the MSP430 peripheral driver library.
	//
	EUSCI_A_UART_initParam peripheralParameters;

} tUARTPort;

/* ==========================================================================*/
/**
 * @brief
 */
/* ==========================================================================*/
void initUart(void);
/* ==========================================================================*/
/**
 * @brief 설정된 Baudrate에 맞는 EUSCI Init Config 반환.
 *
 * @Param baudrate(unit K 250 == 250K)
 *
 * @Returns EUSCI_A_UART_initParam config
 */
/* ==========================================================================*/
EUSCI_A_UART_initParam Get_BaudrateConfig(uint32_t smclk, uint32_t baudrate);

#endif
