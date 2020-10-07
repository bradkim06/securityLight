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

//#############################################################################
//
//! \file   hal_adc.c
//!
//! \brief  Hardware Abstraction Layer for Sigma-Delta 24 (SD24)
//!
//!  Group:          MSP Software Development
//!  Target Device:  MSP430F6736
//!
//!  (C) Copyright 2017, Texas Instruments, Inc.
//#############################################################################

#include "hal.h"

// Status variable used to trigger background process of voltage and current
// samples
uint16_t chReadStatus;

// Variable used to trigger foreground process
volatile uint8_t phaseDataReady;

/* Arrays to store SD24 conversion results */
int16_t phAVSamples[HAL_ADC_V_ARRAY_LENGTH];
int32_t phAISamples[HAL_ADC_I_ARRAY_LENGTH];

// Phase A Sample Alignment variables
volatile int8_t phASA;
volatile uint8_t phAVIdx;
volatile uint8_t phAIIdx;
volatile uint8_t phAProc;

// Index variables to read SD24_B results
volatile uint8_t phAWriteVIdx = 0;
volatile uint8_t phAWriteIIdx = 0;

//
// Function declarations
//
int8_t HAL_ADC_getWholeSample(int16_t phaseCorrection) {
  return (phaseCorrection >> 10);
}

uint16_t HAL_ADC_getPreload(int16_t phaseCorrection, uint16_t osr) {
  return (phaseCorrection & osr);
}

void HAL_ADC_initPreload(HAL_ADC_SD24_B_Configuration *config) {
  config->adcChannelPtr[0].preload = (config->overSampleRatio + 1) >> 1;
  config->adcChannelPtr[1].preload =
      HAL_ADC_getPreload(gEmSWConfig.meterConfig->phaseConfigPtr[EM_PH_A_IDX]
                             .phaseCalibPtr->phaseCorrection,
                         config->overSampleRatio);
}

void HAL_ADC_initWholeSampleAlignment(HAL_ADC_SD24_B_Configuration *config) {
  phASA = HAL_ADC_getWholeSample(
      gEmSWConfig.meterConfig->phaseConfigPtr[EM_PH_A_IDX]
          .phaseCalibPtr->phaseCorrection);
}

void HAL_ADC_clearVariables(uint8_t phase) {
  switch (phase) {
  case EM_PH_A_IDX:
    phASA = 0;
    phAVIdx = 0;
    phAIIdx = 0;
    phAProc = 0;
    phAWriteVIdx = 0;
    phAWriteIIdx = 0;
    break;
  }
  chReadStatus = 0;
}

void HAL_ADC_initializeGroup(HAL_ADC_SD24_B_Configuration *config) {
  uint8_t i, converter;

  for (i = 0; i < config->adcChannelLength; i++) {
    // Find the convIndex of the current converter
    converter = config->adcChannelPtr[i].converter;
    // Create a param structure
    SD24_B_initConverterAdvancedParam param = {0};

    SD24_B_setConversionDelay(SD24_BASE, converter,
                              config->adcChannelPtr[i].preload);
    param.gain = config->adcChannelPtr[i].gain;
    param.converter = converter;
    param.alignment = config->adcChannelPtr[i].alignment;
    param.startSelect = SD24_B_CONVERSION_SELECT_GROUP0;
    param.conversionMode = SD24_B_CONTINUOUS_MODE;
    param.dataFormat = SD24_B_DATA_FORMAT_2COMPLEMENT;
    param.sampleDelay = SD24_B_FOURTH_SAMPLE_INTERRUPT;
    param.oversampleRatio = config->overSampleRatio;

    SD24_B_initConverterAdvanced(SD24_BASE, &param);
    // Enable the interrupt for each channel
    SD24_B_enableInterrupt(SD24_BASE, converter, SD24_B_CONVERTER_INTERRUPT);
  }
}

HAL_ADC_message_code
HAL_ADC_setClockDividers(HAL_ADC_SD24_B_Configuration *config) {
  uint32_t modFreq;
  uint32_t tempDiv;
  HAL_ADC_message_code code = HAL_ADC_message_code_no_error;

  // Calculate the modulation frequency
  modFreq =
      (uint32_t)config->sampleFreq * (uint32_t)(config->overSampleRatio + 1);

  // Calculate SMCLK / (OSR * sampleFreq)
  tempDiv = config->SMCLKFrequency / modFreq;

  // Set ClockPreDivider and ClockDivider
  switch (tempDiv) {
  case HAL_ADC_CLOCK_MULT_4:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_4;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_4;
    break;
  case HAL_ADC_CLOCK_MULT_8:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_8;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_8;
    break;
  case HAL_ADC_CLOCK_MULT_12:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_12;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_12;
    break;
  case HAL_ADC_CLOCK_MULT_16:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_16;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_16;
    break;
  case HAL_ADC_CLOCK_MULT_24:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_24;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_24;
    break;
  case HAL_ADC_CLOCK_MULT_32:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_32;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_32;
    break;
  case HAL_ADC_CLOCK_MULT_64:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_64;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_64;
    break;
  case HAL_ADC_CLOCK_MULT_96:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_96;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_96;
    break;
  case HAL_ADC_CLOCK_MULT_128:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_128;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_128;
    break;
  case HAL_ADC_CLOCK_MULT_192:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_192;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_192;
    break;
  case HAL_ADC_CLOCK_MULT_384:
    config->sd24Param->clockPreDivider = HAL_ADC_SD24PDIV_MULT_384;
    config->sd24Param->clockDivider = HAL_ADC_SD24DIV_MULT_384;
    break;
  default:
    code = HAL_ADC_message_code_invalid_clock_divider_error;
    break;
  }

  return code;
}

HAL_ADC_message_code
HAL_ADC_checkModFreq(HAL_ADC_SD24_B_Configuration *config) {
  uint32_t modFreq;
  HAL_ADC_message_code code;

  modFreq =
      (uint32_t)config->sampleFreq * (uint32_t)(config->overSampleRatio + 1);

  if (modFreq > HAL_ADC_MAX_MOD_FREQ) {
    code = HAL_ADC_message_code_invalid_mod_freq_error;
  } else {
    code = HAL_ADC_message_code_no_error;
  }

  return code;
}

HAL_ADC_message_code HAL_ADC_init(HAL_ADC_SD24_B_Configuration *config) {
  HAL_ADC_message_code code;

  // Initialize global variables
  phaseDataReady = HAL_ADC_PHASE_NO_DATA_READY;
  chReadStatus = 0;
  HAL_ADC_clearVariables(EM_PH_A_IDX);
  HAL_ADC_initPreload(config);
  HAL_ADC_initWholeSampleAlignment(config);

  // Verify the modulation frequency is less than 2MHz
  code = HAL_ADC_checkModFreq(config);
  if (code != HAL_ADC_message_code_no_error) {
    return code;
  }

  // Set the clock dividers for the desired OSR and Fsamp
  code = HAL_ADC_setClockDividers(config);
  if (code != HAL_ADC_message_code_no_error) {
    return code;
  }

  // Initialize SD24
  SD24_B_init(SD24_BASE, config->sd24Param);

  // Configure one group for the SD24
  HAL_ADC_initializeGroup(config);

  return code;
}

void HAL_ADC_startConversion(HAL_ADC_SD24_B_Configuration *config) {
  // Start conversion
  SD24_B_startGroupConversion(SD24_BASE, SD24_B_GROUP0);
}

void HAL_ADC_stopConversion(HAL_ADC_SD24_B_Configuration *config) {
  // Stops the conversion for the enabled SD24 channels
  SD24_B_stopGroupConversion(SD24_BASE, SD24_B_GROUP0);
}

void HAL_ADC_enableInterrupts(HAL_ADC_SD24_B_Configuration *config) {
  uint8_t i, converter;

  // Enable all interrupts
  for (i = 0; i < config->adcChannelLength; i++) {
    // Find the convIndex of the current converter
    converter = config->adcChannelPtr[i].converter;

    SD24_B_enableInterrupt(SD24_BASE, converter, SD24_B_CONVERTER_INTERRUPT);
  }
}

void HAL_ADC_disableInterrupts(HAL_ADC_SD24_B_Configuration *config) {
  uint8_t i, converter;

  // Disable all interrupts
  for (i = 0; i < config->adcChannelLength; i++) {
    // Find the convIndex of the current converter
    converter = config->adcChannelPtr[i].converter;

    SD24_B_disableInterrupt(SD24_BASE, converter, SD24_B_CONVERTER_INTERRUPT);
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = SD24B_VECTOR
__interrupt void SD24BISR(void) {
#elif defined(__GNUC__)
void __attribute__((interrupt(SD24_VECTOR))) SD24_ISR(void)
#else
#error Compiler not supported!
#endif
  int16_t tempV;
  int32_t tempI;
  switch (SD24BIV) {
  case SD24BIV_SD24IFG0: // SD24MEM0 IFG
    chReadStatus |= HAL_ADC_CH0_READ;

    // Store Voltage Results
    phAVSamples[phAWriteVIdx] =
        SD24_B_getHighWordResults(SD24_BASE, SD24_B_CONVERTER_0);

    // Apply Voltage DC Filter
    tempV =
        EM_voltageDCFilter(&gEmSWResult.phaseBGResults[EM_PH_A_IDX].vDCEstimate,
                           phAVSamples[phAWriteVIdx]);
    phAVSamples[phAWriteVIdx] = tempV;

    // Increase Index and Mask
    phAWriteVIdx = (phAWriteVIdx + 1) & (HAL_ADC_V_MASK);
    break;
  case SD24BIV_SD24IFG1: // SD24MEM1 IFG
    break;
  case SD24BIV_SD24IFG2: // SD24MEM2 IFG
    chReadStatus |= HAL_ADC_CH2_READ;

    // Store Current Results
    phAISamples[phAWriteIIdx] =
        SD24_B_getResults(SD24_BASE, SD24_B_CONVERTER_2);
    tempI = phAISamples[phAWriteIIdx] >> 1;
    phAISamples[phAWriteIIdx] = tempI;

    // Apply Current DC Filter
    tempI =
        EM_currentDCFilter(gEmSWResult.phaseBGResults[EM_PH_A_IDX].iDCEstimate,
                           phAISamples[phAWriteIIdx]);
    phAISamples[phAWriteIIdx] = tempI;

    // Increase Index and Mask
    phAWriteIIdx = (phAWriteIIdx + 1) & (HAL_ADC_I_MASK);
    break;
  default:
    break;
  }

  // Check if both channels were read for all phases
  if ((chReadStatus & (HAL_ADC_CH0_READ + HAL_ADC_CH2_READ)) ==
      (HAL_ADC_CH0_READ + HAL_ADC_CH2_READ)) {
    chReadStatus = 0;

    // Phase A Data Processing
    if (phASA == 0) {
      // Voltage aligned with Current
      if (phAProc == 0) {
        phAProc = 1;
      } else {
        // Increase Current (I) Index and Mask
        phAIIdx = (phAIIdx + 1) & HAL_ADC_I_MASK;

        // Increase Voltage (V) Index and Mask
        phAVIdx = (phAVIdx + 1) & HAL_ADC_V_MASK;
      }

      // Set the Ptrs to voltage and current
      emPhaseConfiguration[EM_PH_A_IDX].voltageSamplePtr =
          &phAVSamples[phAVIdx];
      emPhaseConfiguration[EM_PH_A_IDX].currentSamplePtr =
          &phAISamples[phAIIdx];

      // Process the samples
      EM_perSampleProc(gEmSWResult.phaseMetrologyPing[EM_PH_A_IDX],
                       &gEmSWConfig.meterConfig->phaseConfigPtr[EM_PH_A_IDX],
                       &gEmSWResult.phaseBGResults[EM_PH_A_IDX]);

      // Check Energy and Generate Pulses
      EM_genPulseIndPhase(gEmSWConfig.systemConfig,
                          &gEmSWConfig.systemConfig->pulseArray[EM_PH_A_IDX],
                          &gEmSWResult.phaseResults[EM_PH_A_IDX]);

      if (gEmSWResult.phaseMetrologyPing[EM_PH_A_IDX]->cycleCount ==
          gEmSWConfig.algorithmConfig->mainNomFreq) {
        phaseDataReady |= HAL_ADC_PHASE_A_DATA_READY;
        EM_perDataChunk(&gEmSWConfig, (EM_SW_Lib_Result_Handle *)&gEmSWResult,
                        EM_PH_A_IDX);
      } else if (gEmSWResult.phaseMetrologyPing[EM_PH_A_IDX]->sampleCount >
                 (2 * EM_SAMPLING_FREQ_HZ)) {
        phaseDataReady |= HAL_ADC_PHASE_A_DATA_READY;
        EM_perDataChunk(&gEmSWConfig, (EM_SW_Lib_Result_Handle *)&gEmSWResult,
                        EM_PH_A_IDX);
      }
    } else if (phASA > 0) {
      // Voltage leading Current
      phASA = phASA - 1;
      phAIIdx = phAIIdx + 1;
    } else {
      // Voltage lagging Current
      phASA = phASA + 1;
      phAVIdx = phAVIdx + 1;
    }
  }
}
