//*****************************************************************************
// uart_config.h
//
//! This is the UART port driver definitions file.  This file sets
//! the configuration parameters for the UART port.
//
//! \author MSP Microcontroller Products
//
// Release Date: 2020-10-07 Wed 02:35
//
//*****************************************************************************
//*****************************************************************************
//! \addtogroup UART
//! @{
//*****************************************************************************

#ifndef UART_DEFINITIONS_H_
#define UART_DEFINITIONS_H_

#include "EM_userConfig.h"
#include "driverlib.h"

//*****************************************************************************
//
//! def UART__ENABLE controls whether the UART port is included
//! during build.  To use the UART port, set this definition to true.
//
//*****************************************************************************
// NOTE: Rather than enabling UART here, the enable is controlled in
// CAPT_CommConfig.
// #define UART__ENABLE (true)

//*****************************************************************************
//
//! def UART__EUSCI_A_PERIPHERAL defines the MSP430 base address of the
//! eUSCI_A instance being used with this UART port.
//
//*****************************************************************************
#define UART__EUSCI_A_PERIPHERAL (EUSCI_A0_BASE)

//*****************************************************************************
//
//! def I2CSLAVE__LPMx_bits defines the low power mode to enter
//! when pending on a resource.
//
//*****************************************************************************
#define UART__LPMx_bits (LPM0_bits)

//*****************************************************************************
//
//! def UART__SAMPLING_MODE defines the eUSCI_A LF or HF mode.
//
//! def UART__PRESCALER defines the eUSCI_A pre-scaler.
//
//! def UART__FIRST_STAGE_MOD defines the eUSCI_A first stage modulation.
//
//! def UART__SECOND_STAGE_MOD defines the eUSCI_A second stage modulation.
//
//*****************************************************************************
#define UART__SAMPLING_MODE (EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION)

#if (EM_SMCLK_FREQ_IN_HZ == 25165824)

// 250K Baud Rate @ 25MHz
#define UART__PRESCALER        (0x06)
#define UART__FIRST_STAGE_MOD  (0x03)
#define UART__SECOND_STAGE_MOD (0x02)

#elif (EM_SMCLK_FREQ_IN_HZ == 16777216)

// 250K Baud Rate @ 16MHz
#define UART__PRESCALER        (0x04)
#define UART__FIRST_STAGE_MOD  (0x03)
#define UART__SECOND_STAGE_MOD (0x00)

#elif (EM_SMCLK_FREQ_IN_HZ == 8388608)

// 250K Baud Rate @ 8MHz
#define UART__PRESCALER        (0x02)
#define UART__FIRST_STAGE_MOD  (0x01)
#define UART__SECOND_STAGE_MOD (0x02)

#else

// #error "SMCLK frequency not supported by the UART driver"

#endif

//*****************************************************************************
//! Close the doxygen group
//! @}
//*****************************************************************************

#endif /* UART_DEFINITIONS_H_ */
