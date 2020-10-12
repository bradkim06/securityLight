//#############################################################################
//
//! \file   hal_system.c
//!
//! \brief  Hardware Abstraction Layer for MSP system including clocks,
//!         watchdog, and GPIOs
//!
//!
//!  Group:          MSP Embedded Software Development
//!  Target Device:  MSP430F6736
//!
//!  (C) Copyright 2017, Texas Instruments, Inc.
//#############################################################################

#ifndef HAL_SYSTEM_H_
#define HAL_SYSTEM_H_

#include <stdbool.h>
#include <stdint.h>

/* Function prototypes */
//*****************************************************************************
//
//! Initialize the System
//! Includes initialization of clocks, watchdog and GPIOs
//!
//! \return none
//
// *****************************************************************************
extern void initSystem(void);

#endif /* HAL_SYSTEM_H_ */
