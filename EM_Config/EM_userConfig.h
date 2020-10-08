/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#ifndef _EMSWLIB_USERCONFIG_H_
#define _EMSWLIB_USERCONFIG_H_

//#############################################################################
//
//! \file   EM_userConfig.h
//!
//! \brief  This file contains EM SW Library configuration
//!
//
//!  Group:          MSP Embedded Software Development
//!  Target Device:  MSP430F6736
//
//  (C) Copyright 2017, Texas Instruments, Inc.
//#############################################################################
// TI Release: MSP430_ENERGY_MEASUREMENT
// Release Date: 29Jul2020
//#############################################################################

//*****************************************************************************
// includes
//*****************************************************************************
//*****************************************************************************
// includes
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>

#include "IQmathLib.h"
#include "emSwLib.h"
#include "hal_adc.h"

//*****************************************************************************
//! \addtogroup emSwLib_userConfig
//! @{
//*****************************************************************************

#ifdef __cplusplus

extern "C" {
#endif

//*****************************************************************************
// defines
//*****************************************************************************

#define EM_SMCLK_FREQ_IN_HZ (25165824)

#define EM_CHANNEL_LENGTH     (2)
#define EM_PHASE_LENGTH       (1)
#define EM_AGGREGATE_ENABLE   (0)
#define EM_PULSE_ARRAY_LENGTH (EM_PHASE_LENGTH + EM_AGGREGATE_ENABLE)

#define EM_MAIN_NOMINAL_FREQ_HZ     (60)
#define EM_SAMPLING_FREQ_HZ         (HAL_ADC_FREQ_4096_HZ)
#define EM_PHASE_IMP_PER_KWHR       (6400)
#define EM_PHASE_PULSE_DUR          (20)
#define EM_TOTAL_PHASE_IMP_PER_KWHR (6400)
#define EM_TOTAL_PHASE_PULSE_DUR    (20)
#define EM_PHASE_START_UP_POWER_mW  (0)
#define EM_TOTAL_START_UP_POWER_mW  (0)

// Indexes for the Phase Array
#define EM_PH_A_IDX (0)

//*****************************************************************************
// typedefs
//*****************************************************************************

//******************************************************************************
// DO NOT MODIFY THESE DEFINES
//******************************************************************************
#define EM_PHASE_CONSTANT \
  (3600 * 1e9 * EM_SAMPLING_FREQ_HZ / EM_PHASE_IMP_PER_KWHR)
#define EM_TOTAL_PHASE_CONSTANT \
  (3600 * 1e9 * EM_SAMPLING_FREQ_HZ / EM_TOTAL_PHASE_IMP_PER_KWHR)
#define EM_MAX_FREQ_COUNT (EM_SAMPLING_FREQ_HZ * 256L / 40)
#define EM_MIN_FREQ_COUNT (EM_SAMPLING_FREQ_HZ * 256L / 70)

#define SYSTEM_NONV_CALIB_ADDR    (0x00001980)
#define SYSTEM_NONV_SEGMENT_SIZE  (128)
#define SYSTEM_NONV_CHECKSUM_ADDR (0x000019F8)

//*****************************************************************************
// globals
//*****************************************************************************

extern EM_SW_Lib_Config_Handle gEmSWConfig;

extern EM_SW_Lib_Result_Handle gEmSWResult;

extern EM_Phase_Configuration emPhaseConfiguration[EM_PHASE_LENGTH];

#if defined(__IAR_SYSTEMS_ICC__)
#pragma location = SYSTEM_NONV_CALIB_ADDR
#endif
extern const EM_Phase_Calibration g_savedEmPhaseCalibration[EM_PHASE_LENGTH];

extern EM_Phase_Calibration g_emPhaseCalibration[EM_PHASE_LENGTH];

//*****************************************************************************
// the function prototypes
//*****************************************************************************

extern void EM_perSampleProc(EM_Metrology*           metro,
                             EM_Phase_Configuration* phase,
                             EM_Phase_BG_Results*    bgData);

extern void EM_foregroundProc(uint8_t* resultStatus);

#ifdef __cplusplus
}
#endif  // extern "C"
//@}  // ingroup

#endif  // end of  _EMSWLIB_USERCONFIG_H_ definition
