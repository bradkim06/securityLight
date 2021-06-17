/* --COPYRIGHT--,BSD
 * Copyright (c) 2020, Texas Instruments Incorporated
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

#ifndef _EMSWLIB_H_
#define _EMSWLIB_H_

//#############################################################################
//
//! \file   emSwLib.h
//!
//! \brief  Contains all Energy Measurement SW Library enums, structs, macros,
//!            function and global variables defintions
//!
//
//  Group:          MSP
//  Target Device:  MSP430F677x1A, MSP430F673xA and MSP430i20xx
//
//  (C) Copyright 2017, Texas Instruments, Inc.
//#############################################################################
// TI Release: MSP430_ENERGY_MEASUREMENT
// Release Date: 29Jul2020
//#############################################################################

//*****************************************************************************
// includes
//*****************************************************************************
#include <msp430.h>

#include <stdint.h>
#include <stdbool.h>
#include "IQmathLib.h"
#include "QmathLib.h"

#ifdef __cplusplus

extern "C" {
#endif

//*****************************************************************************
// defines
//*****************************************************************************

//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the VRMS is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_VRMS BIT0
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the IRMS is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_IRMS BIT1
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the V-PEAK is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_V_PEAK BIT2
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the I-PEAK is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_I_PEAK BIT3
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the POWER FACTOR is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_POWER_FACTOR BIT4
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the Frequency is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_FREQ BIT5
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the ACTIVE POWER is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_ACTIVE_POWER BIT6
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the REACTIVE POWER is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_REACTIVE_POWER BIT7
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the APPARENT POWER is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_APPARENT_POWER BIT8
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the ACTIVE ENERGY is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_ACTIVE_ENERGY BIT9
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the REACTIVE ENERGY is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_REACTIVE_ENERGY BIT10
//! \brief This define can be used to mask the EM_Phase->phaseResultsSupported variable
//! and determine if the APPARENT ENERGY is calculated by the EM_processSample().
#define EM_PHASE_RESULT_CALC_APPARENT_ENERGY BIT11

//! \brief This define can be used to mask the EM_System_Configuration->pulseGeneration
//! variable . This is used by the EM_genPulseIndPhase() to generate pulses for Phase A
#define EM_PULSE_GEN_PHASE_NONE (0x00)
//! \brief This define can be used to mask the EM_System_Configuration->pulseGeneration
//! variable . This is used by the EM_genPulseIndPhase() to generate pulses for Phase A
#define EM_PULSE_GEN_PHASE_A (0x01)
//! \brief This define can be used to mask the EM_System_Configuration->pulseGeneration
//! variable . This is used by the EM_genPulseIndPhase() to generate pulses for Phase B
#define EM_PULSE_GEN_PHASE_B (0x02)
//! \brief This define can be used to mask the EM_System_Configuration->pulseGeneration
//! variable . This is used by the EM_genPulseIndPhase() to generate pulses for Phase C
#define EM_PULSE_GEN_PHASE_C (0x04)
//! \brief This define can be used to mask the EM_System_Configuration->pulseGeneration
//! variable . This is used by the EM_genPulseIndPhase() to generate pulses for Phase D
#define EM_PULSE_GEN_PHASE_D (0x08)
//! \brief This define can be used to mask the EM_System_Configuration->pulseGeneration
//! variable . This is used by the EM_genPulseIndPhase() to generate pulses for Phase E
#define EM_PULSE_GEN_PHASE_E (0x10)
//! \brief This define can be used to mask the EM_System_Configuration->pulseGeneration
//! variable . This is used by the EM_genPulseIndPhase() to generate pulses for Phase F
#define EM_PULSE_GEN_PHASE_F (0x20)
//! \brief This define can be used to mask the EM_System_Configuration->pulseGeneration
//! variable . This is used by the EM_genPulseIndPhase() to generate pulses for Phase
//! neutral.
#define EM_PULSE_GEN_PHASE_NEUTRAL (0x40)
//! \brief This define can be used to mask the EM_System_Configuration->pulseGeneration
//! variable . This is used by the EM_genPulseTotalPhases() to generate pulses for the
//! aggregate total.
#define EM_PULSE_GEN_PHASE_AGGREGATE (0x80)

//*****************************************************************************
// typedefs
//*****************************************************************************

//! \brief The following enumeration contain all possible EM SW Library return
//!         message codes.
//!
typedef enum _EM_message_code_ {

	EM_message_code_no_error = 0,
	//!< This message code is returned when no error was detected while
	//!< invoking the API.

	// 1-20 reserved for system configuration messages
	EM_message_code_pulseArray_error = 1,
	//!< This error is returned if pulseArrayLength is greater than 0
	//!< and pulseArray is 0
	EM_message_code_indPhaseMeter_error = 2,
	//!< This error is returned if pulseArrayLength is greater than 0
	//!< and indPhaseMeterConstant is 0
	EM_message_code_indPhasePulseDuration_error = 3,
	//!< This error is returned if pulseArrayLength is greater than 0
	//!< and indPhasePulseDuration is 0
	EM_message_code_totalPhaseMeter_error = 4,
	//!< This error is returned if pulseArrayLength is greater than 0,
	//!< totalPhaseMeterConstant is 0 and the pulseGeneration has
	//!< the EM_PULSE_GEN_PHASE_AGGREGATE flag set
	EM_message_code_totalPhasePulseDuration_error = 5,
	//!< This error is returned if pulseArrayLength is greater than 0
	//!< and totalPhasePulseDuration is 0 and the pulseGeneration has
	//!< the EM_PULSE_GEN_PHASE_AGGREGATE flag set

	// 21-40 reserved for algorithm configuration messages
	EM_message_code_samplingFrequency_error = 21,
	//!< This error is returned if the sampling frequency is not greater than 0
	EM_message_code_reactPowLength_error = 22,
	//!< This error is returned if the reactPowIIRTable ptr is null and
	//!< the reactPowIIRLength variable is greater than 0
	EM_message_code_reactPowIIRTable_error = 23,
	//!< This error is returned if the reactPowIIRLength is greater than 0
	//!< and the reactPowIIRTable ptr is equal to 0

	// 41-59 reserved for meter configuration messages
	EM_message_code_phaseLength_error = 41,
	//!< This error is returned if the phaseLength is equal to 0
	EM_message_code_phaseConfig_error = 41,
	//!< This error is returned if the phaseConfigPtr is equal to 0

	// 60-249 reserved for calibration messages
	EM_message_code_calibrate_invalidPhase_error = 81,
	//!< This error is returned if the phase number is invalid

	//240-255 error
	EM_message_code_error_occurred = 255
	//!< This is a generic error this is error that is returned if an unhandled
	//!< error occurs
} EM_message_code;

//! \brief Selects appropriate EM Sensor for the corresponding channel
//!
typedef enum _EM_Current_Sensor_ {
	EM_Phase_Sensor_CT = 0,
	//!< Current Transformer (CT)
	EM_Phase_Sensor_Shunt = 1,
	//!< Shunt
	EM_Phase_Sensor_RC = 2,
	//!< Rogowski Coil (RC)
} EM_Current_Sensor;

//! \brief Type for Power variables
//!
typedef int64_t EM_Power_t;

//! \brief Type for Energy variables
//!
typedef uint64_t EM_Energy_t;

//! \brief Selects appropiate phase to provide results for
//!
typedef enum _EM_Phase_ {
	EM_Phase_A = 0x01,
	//!< - Results requested for phase A
	EM_Phase_B = 0x02,
	//!< - Results requested for phase B
	EM_Phase_C = 0x04,
	//!< - Results requested for phase C
	EM_Phase_D = 0x08,
	//!< - Results requested for phase D
	EM_Phase_E = 0x10,
	//!< - Results requested for phase E
	EM_Phase_F = 0x20,
	//!< - Results requested for phase F
	EM_Phase_Neutral = 0x40
	//!< - Results requested for phase Neutral
} EM_Phase;

//! \brief Contains EM SW Lib Phase Calibration
//!
typedef struct _EM_Pulse_Settings_ {
	uint8_t activePulseGpioPort;
	//!< Each phase uses a GPIO port used to generate a pulse used for calibration
	//!< Possible options for each port:
	//!< - GPIO_PORT_P1
	//!< - GPIO_PORT_P2
	//!< - GPIO_PORT_P3
	//!< - GPIO_PORT_P4
	//!< - GPIO_PORT_P5
	//!< - GPIO_PORT_P6
	//!< - GPIO_PORT_P7
	//!< - GPIO_PORT_P8
	//!< - GPIO_PORT_P9
	uint8_t reactivePulseGpioPort;
	//!< Each phase uses a GPIO port used to generate a pulse used for calibration
	//!< Possible options for each port:
	//!< - GPIO_PORT_P1
	//!< - GPIO_PORT_P2
	//!< - GPIO_PORT_P3
	//!< - GPIO_PORT_P4
	//!< - GPIO_PORT_P5
	//!< - GPIO_PORT_P6
	//!< - GPIO_PORT_P7
	//!< - GPIO_PORT_P8
	//!< - GPIO_PORT_P9
	uint16_t activePulseGpioPin;
	//!< Each phase uses a GPIO pin used to generate a pulse used for calibration
	//!< Possible options for each pin:
	//!< - GPIO_PIN0
	//!< - GPIO_PIN1
	//!< - GPIO_PIN2
	//!< - GPIO_PIN3
	//!< - GPIO_PIN4
	//!< - GPIO_PIN5
	//!< - GPIO_PIN6
	//!< - GPIO_PIN7
	uint16_t reactivePulseGpioPin;
	//!< Each phase uses a GPIO pin used to generate a pulse used for calibration
	//!< Possible options for each pin:
	//!< - GPIO_PIN0
	//!< - GPIO_PIN1
	//!< - GPIO_PIN2
	//!< - GPIO_PIN3
	//!< - GPIO_PIN4
	//!< - GPIO_PIN5
	//!< - GPIO_PIN6
	//!< - GPIO_PIN7
	uint64_t activeEnergyCounter;
	//!< Ongoing Active Energy Sum
	uint16_t activeEnergyPulseRemaining;
	//!< Counter used to keep track of remaining ADC samples to keep pulse high
	uint64_t reactiveEnergyCounter;
	//!< Ongoing Reactive Energy Sum
	uint16_t reactiveEnergyPulseRemaining;
	//!< Counter used to keep track of remaining ADC samples to keep pulse high
} EM_Pulse_Settings;

//! \brief Contains EM SW Lib System Configuration
//!
typedef struct _EM_System_Configuration_ {
	EM_Pulse_Settings *pulseArray;
	//!< Pointer to the location where the pulse configuration is stored
	//!< (one for each phase)
	uint8_t pulseArrayLength;
	//!< Length of the pulseArray configuration
	uint64_t indPhaseMeterConstant;
	//!< Number of pulses for each / kWH (active energy) or number of pulses
	//!< for each / kVarh (reactive energy) for each phase
	uint16_t indPhasePulseDuration;
	//!< Pulse duration in ADC counts for each active / reactive energy
	uint64_t totalPhaseMeterConstant;
	//!< Number of pulses for each / kWH for the cumulative total active
	//!< energy or number of pulses for each / kVarh for the cumulative
	//!< reactive energy
	uint16_t totalPhasePulseDuration;
	//!< Pulse duration in ADC counts for the cumulative total of active
	//!< / reactive energy
	uint8_t pulseGeneration;
	//!< Register used to enable / disable the phase pulse flag:
	//!< - EM_PULSE_GEN_PHASE_A
	//!< - EM_PULSE_GEN_PHASE_B
	//!< - EM_PULSE_GEN_PHASE_C
	//!< - EM_PULSE_GEN_PHASE_D
	//!< - EM_PULSE_GEN_PHASE_E
	//!< - EM_PULSE_GEN_PHASE_F
	//!< - EM_PULSE_GEN_PHASE_NEUTRAL
	//!< - EM_PULSE_GEN_PHASE_AGGREGATE
} EM_System_Configuration;

//! \brief Contains EM SW Lib Phase Calibration
//!
typedef struct _EM_Phase_Calibration_ {
	uint32_t voltageSF;
	//!< Voltage Scaling Factor - used to calculate VRMS
	uint32_t currentSF;
	//!< Current Scaling Factor - used to calculate IRMS
	uint32_t activePowerSF;
	//!< Active Power Scaling Factor
	uint32_t reactivePowerSF;
	//!< Active Power Scaling Factor
	int16_t phaseCorrection;
	//!< Phase Correction between voltage and current (register units)

} EM_Phase_Calibration;

//! \brief Contains EM SW Lib System Configuration
//!
typedef struct _EM_Phase_Configuration_ {
	int32_t *currentSamplePtr;
	//!< Pointer to the current sample
	int16_t *voltageSamplePtr;
	//!< Pointer to the voltage sample
	EM_Current_Sensor currentSensor;
	//!< Sensor connected to the current (I) channel
	uint16_t phaseResultsSupported;
	//!< Register used to read what results will be calculated for this phase
	//!< Note: This is a Read-Only register.
	//!< Possible mask values are:
	//!< - EM_PHASE_RESULT_CALC_VRMS
	//!< - EM_PHASE_RESULT_CALC_IRMS
	//!< - EM_PHASE_RESULT_CALC_V_PEAK
	//!< - EM_PHASE_RESULT_CALC_I_PEAK
	//!< - EM_PHASE_RESULT_CALC_POWER_FACTOR
	//!< - EM_PHASE_RESULT_CALC_FREQ
	//!< - EM_PHASE_RESULT_CALC_ACTIVE_POWER
	//!< - EM_PHASE_RESULT_CALC_REACTIVE_POWER
	//!< - EM_PHASE_RESULT_CALC_APPARENT_POWER
	//!< - EM_PHASE_RESULT_CALC_ACTIVE_ENERGY
	//!< - EM_PHASE_RESULT_CALC_REACTIVE_ENERGY
	//!< - EM_PHASE_RESULT_CALC_APPARENT_ENERGY
	EM_Phase_Calibration *phaseCalibPtr;
	//!< Pointer to EM_Phase_Calibration struct
} EM_Phase_Configuration;

//! \brief Contains EM SW Lib Meter Configuration
//!
typedef struct _EM_Meter_Configuration_ {
	EM_Phase_Configuration *phaseConfigPtr;
	//!< Pointer to the EM_Phase_Configuration struct buffer
	uint8_t phaseLength;
	//!< Length of the emPhasePtr buffer
	uint8_t *meterName;
	//!< Pointer to meter name string
	uint8_t *libVersionNumber;
	//!< Pointer to library version number string
	uint8_t *meterPW;
	//~< Pointer to meter password string
} EM_Meter_Configuration;

//! \brief Contains EM SW Lib Algorithm Configuration
//!
typedef struct _EM_Algorithm_Configuration_ {
	EM_Power_t residualPowerCutoff;
	//!< Residual Power Cutoff per each phase. This threshold is used as the minimum
	//!< value of power to store. Power values below this value will not be stored.
	EM_Power_t totalResidualPowerCutoff;
	//!< Residual Power Cutoff for the sum of all the phases. If the sum of all phase
	//!< power values is below this value, the total power value will not be updated.
	uint8_t mainNomFreq;
	//!< Nominal Freq value for the system (Hz)
	_iq29 sqrtTwoConst;
	//!< Square of 2 constant used for Vpeak and Ipeak calculations
	_iq30 energyCalcConst;
	//!< One dividing by (3600 x sampling frequency)
	uint16_t freqSampling;
	//!< Sampling Frequency (Hz)
	uint16_t reactPowIIRMinFreq;
	//!< Min Frequency for the IIR coefficient table
	uint8_t reactPowIIRShift;
	//!< Number of shifts required to calculated index for IIR table
	uint16_t reactPowIIRLength;
	//!< Length of the IIR coefficient table
	_iq30 *reactPowIIRTable;
} EM_Algorithm_Configuration;

//! \brief Contains EM SW Lib Config Handle
//!
typedef struct _EM_SW_Lib_Config_Handle_ {
	EM_System_Configuration *systemConfig;
	//!< Pointer to EM_System_Configuration struct
	EM_Algorithm_Configuration *algorithmConfig;
	//!< Pointer to EM_Algorithm_Configuration struct
	EM_Meter_Configuration *meterConfig;
	//!< Pointer to EM_Meter_Configuration struct
} EM_SW_Lib_Config_Handle;

typedef struct _EM_Metrology_ {
	int16_t sampleCount;
	//!< Number of samples being accumulated in the block of processed
	//!< signals being gathered used for the oneDivSC calculation.
	int64_t actPowDotProd;
	//!< Accumulated multiplication of the phase corrected current and voltage (I.V)
	//!< used for the active power calculation.
	int64_t reactPowDotProd;
	//!< Accumulated multiplication of the phase corrected current and voltage 90 (I.V90)
	//!< used for the reactive power calculation.
	int16_t vDotProd[3];
	//!< Accumulated squares of the phase corrected voltage (V.V). These are used to
	//!< calculate the RMS voltage, and the power factor. We need separate accumulation
	//!< for each current sensor, as we accumulate the phase corrected voltage which matches
	//!< the current sensor's phase shift.
	int64_t iDotProd;
	//!< Accumulated squares of the phase corrected current (I.I). These are used to
	//!< calculate the RMS current, and the power factor. We need separate accumulation
	//!< for each current sensor, as we accumulate the phase corrected voltage which matches
	//!< the current sensor's phase shift.
	uint16_t cycleSampleCount;
	//!< This is a count of the number of samples to date in the current mains cycle. This
	//!< is used to calculate the mainPeriod.
	_iq30 reactPowGConst;
	//!< G Constant used for the IIR filter to calculate Reactive Power
	uint32_t mainPeriod;
	//!< This a running average of the length of a mains cycle. (samples/cycle * 256)
	uint32_t prevMainPeriod;
	//!< This a running average of the length of a mains cycle. (samples/cycle * 256)
	_iq30 oneDivSC;
	//!< Constant used to calculate the VRMS,IRMS, power and energy values (1/sample_count)
	uint16_t prevSampleCount;
	//!< Stores the prev sample count used to calculate oneDivSC. This is used to avoid
	//!< calculating 1/SC each cycle.
	uint8_t cycleCount;
	//!< Number of zero crossings. Once this number has reached the # of nominal freq.
	//!< For example for 50.2 Hz, once it reaches 50 cycles then the foreground is triggered.
} EM_Metrology;

//! \brief Contains EM Phase Background Results
//!
typedef struct _EM_Phase_BG_Results_ {
	int32_t vDCEstimate;
	//!< DC Estimate for Voltage
	int16_t iDCEstimate[3];
	//!< DC Estimate for Current
	int16_t reactPowPrevV90;
	//!< Previous V90 voltage used for the IIR filter in the reactive power calculation.
	int16_t vPrevSample;
	//!< Previous Voltage sample used for the IIR filter in the reactive power calculation.
	int32_t rogowskiPrevI90;
	//!< Previous I90 current used for the IIR filter in the Rogowski ADC current calculation.
	int32_t iPrevSample;
	//!< Previous Current sample used for the IIR filter in the Rogowski ADC current calculation.
} EM_Phase_BG_Results;

//! \brief Contains EM Phase Result
//!
typedef struct _EM_Phase_Results_ {
	uint32_t VRMS;
	//!< VRMS for previous frame (mV)
	uint32_t IRMS;
	//!< IRMS for previous frame (uA)
	uint32_t vPeak;
	//!< VPeak since system initialization (mV)
	uint32_t iPeak;
	//!< IPeak since system initialization (uA)
	int32_t powerFactor;
	//!< Power Factor calculated when the EM_powerFactor
	uint16_t frequency;
	//!< The Frequency is stored in 0.01Hz steps. A 16 bit unsigned number supports the range
	//!< 0.01Hz to 655.35Hz, which is clearly more than we will ever need for the fundamental.
	EM_Power_t activePower;
	//!< Active Power for previous frame (uW)
	EM_Power_t reactivePower;
	//!< Reactive Power for previous frame (uVAr)
	EM_Power_t apparentPower;
	//!< Apparent Power for previous frame (uVA)
	EM_Energy_t activeEnergy;
	//!< Active Energy for previous frame (uWh)
	EM_Energy_t reactiveEnergy;
	//!< Reactive Energy for previous frame (uVArh)
	EM_Energy_t apparentEnergy;
	//!< Apparent Energy for previous frame (uVAh)
	EM_Phase phaseID;
	//!< Present Phase ID Name
} EM_Phase_Results;

//! \brief Contains EM Total Results
//!
typedef struct _EM_Total_Results_ {
	EM_Power_t activePower;
	//!< Total Active Power since system initialization (uW)
	EM_Power_t reactivePower;
	//!< Total Reactive Power since system initialization (uVAr)
	EM_Power_t apparentPower;
	//!< Total Apparent Power since system initialization (uVA)
	EM_Energy_t activeEnergy;
	//!< Total Active Energy consumed since system initialization (uWh)
	EM_Energy_t reactiveEnergy;
	//!< Total Reactive Energy consumed since system initialization (uVArh)
	EM_Energy_t apparentEnergy;
	//!< Total Apparent Energy consumed since system initialization (UVAh)
} EM_Total_Results;

//! \brief Contains EM SW Lib Result Handle
//!
typedef struct _EM_SW_Lib_Result_Handle_ {
	EM_Total_Results *emTotalResult;
	//!< Pointer to EM_Total_Results struct
	EM_Phase_Results *phaseResults;
	//!< Pointer to EM_Phase_Result struct buffer
	EM_Phase_BG_Results *phaseBGResults;
	//!<  Pointer to the EM_Phase_BG_Results struct buffer
	EM_Metrology **phaseMetrologyPing;
	//!< Pointer to EM_Metrology struct Ping buffer
	EM_Metrology **phaseMetrologyPong;
	//!< Pointer to EM_Metrology struct Pong buffer
	uint8_t phaseResultsSupportedLen;
	//!< Length of the phaseResultsSupportedPtr buffer
} EM_SW_Lib_Result_Handle;

//*****************************************************************************
// globals
//*****************************************************************************
//! \brief The following variable is used as a maximum ADC count to detect frequency.
extern uint16_t *EMSWLIB_EM_HAL_ADC_maxFreqCount;

//! \brief The following variable is used as a minimum ADC count to detect frequency.
extern uint16_t *EMSWLIB_EM_HAL_ADC_minFreqCount;

//*****************************************************************************
// the function prototypes
//*****************************************************************************

/*******************************************************************************
 * EM SW LIBRARY CONFIGURATION APIS
 ******************************************************************************/
//*****************************************************************************
//
//! \addtogroup configuration_api
//! @{
//
//******************************************************************************
//! \brief The following API initializes the variables needed by the library at
//! run time. This includes the G constant value used for the IIR filter,  1 over
//! sample count and 1 / (3600 * sampling frequency) which are used to calculate
//! metrology results. This function also checks for configuration errors.
//!
//! \param[in] config  is a pointer to the EM SW Library user configuration
//! \param[in] result  is a pointer to the EM SW Result configuration
//!
//! \return EM_message_code
//!
extern EM_message_code EM_configureEnergyMeasurement(EM_SW_Lib_Config_Handle *config,
						     EM_SW_Lib_Result_Handle *result);

//! \brief The following API initializes the voltage / current DC filters to 0.
//!
//! \param[in] config  is a pointer to the EM SW Library user configuration
//! \param[in] result  is a pointer to the EM SW Result configuration
//!
//! \return None
//!
extern void EM_initDCFilters(EM_SW_Lib_Config_Handle *config, EM_SW_Lib_Result_Handle *result);

//******************************************************************************
//
// Close the Doxygen group.
//! @}
//
//******************************************************************************

/*******************************************************************************
 * EM SW LIBRARY CALIBRATION APIS
 ******************************************************************************/
//*****************************************************************************
//
//! \addtogroup calibration_api
//! @{
//
//******************************************************************************
//! \brief    The following API is used to copies calibration values from
//!           flapFlashCalibration to pWorkingCalibration if the flash has been
//!           written to during the calibration process. If the flash
//!           buffer has not been written to, then the default values stored in
//!           pWorkingCalibration will be used at run time.
//!
//! \param[in] pWorkingCalibration  is a pointer to the working calibration buffer
//! \param[in] pFlashCalibration  is a pointer to the flash calibration buffer
//!
//! \return None
//!
extern void EM_initalizeCalibrationValues(uint8_t *pWorkingCalibration, uint8_t *pFlashCalibration);

//! \brief    The following API is used  store working (RAM) pWorkingCalibration
//!           values to the pFlashCalibration structure stored in flash. When
//!           there is a power cycle this values will be loaded to the working
//!           calibration buffer by using the EM_initalizeCalibrationValues API.
//!
//! \param[in] pWorkingCalibration  is a pointer to the working calibration buffer
//! \param[in] pFlashCalibration  is a pointer to the flash calibration buffer
//!
//! \return None
//!
extern void EM_updateFlashCalibrationValues(uint8_t *pWorkingCalibration,
					    uint8_t *pFlashCalibration);

//******************************************************************************
//
// Close the Doxygen group.
//! @}
//
//******************************************************************************

/*******************************************************************************
 * EM SW LIBRARY RESULT APIS
 ******************************************************************************/
//*****************************************************************************
//
//! \addtogroup result_api
//! @{
//
//******************************************************************************
//! \brief The following API is intended for the removal of the DC content from
//! 16 bit 50Hz/60Hz mains signals. All samples should be passed through the
//! filter, in sequence, to obtain a signal free of DC content. The estimation
//! is based on a noise shaped single pole LPF.The cutoff frequency of this
//! filter is set very low, so its gain is essentially flat from 45Hz upwards.
//! This means the filter can take several seconds to stabilize when the
//! signal is initially applied.
//!
//! \param[in] p  is a pointer to the filter state variable
//! \param[in] x  is the next signal sample
//!
//! \return None
//!
extern int16_t EM_voltageDCFilter(int32_t *p, int16_t x);

//! \brief The following API is intended for the removal of the DC content from
//! 24 bit 50Hz/60Hz mains signals. All samples should be passed through the
//! filter, in sequence, to obtain a signal free of DC content. The estimation
//! is based on a noise shaped single pole LPF.The cutoff frequency of this
//! filter is set very low, so its gain is essentially flat from 45Hz upwards.
//! This means the filter can take several seconds to stabilize when the
//! signal is initially applied.
//!
//! \param[in] p  is a pointer to the filter state variable
//! \param[in] x  is the next signal sample
//!
//! \return None
//!
extern int32_t EM_currentDCFilter(int16_t p[3], int32_t x);

//! \brief    The following API is used update the previous voltage
//! value for a phase and to increment the number of samples processed
//! in the ISR. This API must be called inside the ISR after all the background
//! APIs are executed for a phase.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Ping Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//! \param[in] bgData  is a pointer to the EM RP Phase Results structure
//!
//! \return None
//!
extern void EM_perSample(EM_Metrology *metro, EM_Phase_Configuration *phase,
			 EM_Phase_BG_Results *bgData);

//! \brief    The following API is used to swap the metrology ping and pong
//! pointers. This API must be executed after the Frequency Function
//! has processed N number of cycles (where N = config->algoConfig->mainNomFreq).
//! The Metrology Ping struc is the buffer updated in the ISR, and Metrology Pong
//! struct is used by the foreground APIs.
//!
//! \param[in] config  is a pointer to the EM SW Library user configuration
//! \param[in] result  is a pointer to the EM SW Library result structure
//! \param[in] phaseIdx  is the index for the phase
//!
//! \return None
//!
extern void EM_perDataChunk(EM_SW_Lib_Config_Handle *config, EM_SW_Lib_Result_Handle *result,
			    uint16_t phaseIdx);

//! \brief    The following API is used to delay the current degree by 90 degrees.
//! It uses an IIR filter with 1 pole to shift current: (all pass 90 degree filter
//! for w = PI*f/fs, f=signal freq and fs = sampling frequency)
//! I90(IDX0 = I(IDX-1)+G*[I(IDX) - I90(IDX-1)] G = (tan(w) - 1)/(tan(w) + 1)
//! The G constant used by the filter is based on signal
//! frequency which is updated ~ once a second. It updates metro Result structure;
//! storing the temporary multiply accumulate (MAC) calculations.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Ping Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//! \param[in] bgData  is a pointer to the EM SW BG Result structure
//!
//! \return None
extern void EM_delayCurrent90Degrees(EM_Metrology *metro, EM_Phase_Configuration *phase,
				     EM_Phase_BG_Results *bgData);

//! \brief    The following API is used to generate pulses for an individual
//! phase. This API checks if the config->pulseGeneration has the result->
//! phaseID set. If is set then it will generate EM_PHASE_IMP_PER_KWHR impulses
//! per kWhr. This API requires that the pulse settings are initialized for the
//! activePulse and reactivePulse Port/Pins.
//!
//! \param[in] config  is a pointer to the EM SW Library user configuration
//! \param[in] pulse  is a pointer to the EM SW Library pulse structure
//! \param[in] result  is a pointer to the EM SW Library result structure
//!
//! \return None
//!
extern void EM_genPulseIndPhase(EM_System_Configuration *config, EM_Pulse_Settings *pulse,
				EM_Phase_Results *result);

//! \brief    The following API is used to generate pulses for the total
//! phase. This API checks if the config->pulseGeneration has EM_PULSE_GEN_PHASE_AGGREGATE
//!  set. If is set then it will generate EM_PHASE_IMP_PER_KWHR impulses
//! per kWhr. This API requires that the pulse settings are initialized for the
//! activePulse and reactivePulse Port/Pins.
//!
//! \param[in] config  is a pointer to the EM SW Library user configuration
//! \param[in] pulse  is a pointer to the EM SW Library pulse structure
//! \param[in] result  is a pointer to the EM SW Library result structure
//!
//! \return None
//!
extern void EM_genPulseTotalPhases(EM_System_Configuration *config, EM_Pulse_Settings *pulse,
				   EM_Total_Results *result);

//! \brief    The following API is used to calculate the 1/sample_count constant.
//! This function must be executed after EM_perDataChunk API for each phase inside
//! the foreground code. The 1/sample_count constant will be used to calculate
//! the results parameters.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//!
//! \return None
//!
extern void EM_updateOneDivSC(EM_Metrology *metro);

//! \brief    The following API is used to clear the temporary variables inside
//! the metro structure. This API must be called as the last API in the
//! foreground code.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//!
//! \return None
//!
extern void EM_clearMetrology(EM_Metrology *metro);

//! \brief    The following API is used to look up the IIR G constant used to
//! calculate the Reactive Power using the current frequency value. The
//! index of the G constant in the reactPowIIRTable is calculated:
//! index = (Current Frequency - reactPowIIRMinFreq) >> reactPowIIRShift
//! if the index is > reactPowIIRLength, then index = reactPowIIRLength
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//! \param[in] result  is a pointer to the EM SW Result structure
//! \param[in] algo  is a pointer to the EM SW Algorithm structure
//!
//! \return None
//!
extern void EM_updateReactPowConst(EM_Metrology *metro, EM_Phase_Results *result,
				   EM_Algorithm_Configuration *algo);

//! \brief    The following API is used process the samples. It is used to
//! calculate intermediate dot product results that are fed into the foreground
//! process for the calculation of metrology readings.
//!
//! \param[in] config  is a pointer to the EM SW Library user configuration
//!
//! \return EM_message_code
//!
extern EM_message_code EM_processSample(EM_SW_Lib_Config_Handle *config);

//! \brief    The following API is used to calculate the dot product and
//! accumulation of Voltage and Current. This API reads the current and voltage
//! values using the phase structure.It updates metro Result structure; storing
//! the temporary multiply accumulate (MAC) calculations. The MAC value is used
//! by the foreground function EM_calcActivePowerDiv to calculate Active Power.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Ping Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//!
//! \return None
//!
extern void EM_calcActivePowerMultAcc(EM_Metrology *metro, EM_Phase_Configuration *phase);

//! \brief    The following API is used to calculate the active power in the
//! foreground code and store activePower in the result structure. The formula used:
//! (metro->AP_dot_prod->P_accum x metro->oneDivSC) x phase->phaseCalibPtr->activePowerSF
//! If the activePower calculated is less than algo->residualPowerCutoff
//! then activePower will be set to 0.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//! \param[in] result  is a pointer to the EM SW Result structure
//! \param[in] algo  is a pointer to the EM SW Algorithm structure
//!
//! \return None
//!
extern void EM_calcActivePowerDiv(EM_Metrology *metro, EM_Phase_Configuration *phase,
				  EM_Phase_Results *result, EM_Algorithm_Configuration *algo);

//! \brief    The following API is used to calculate the active energy result.
//! The formula used to calculate activeEnergy (uWh):
//! (result->activePower x metro->sampleCount) x energyCalcConst
//! Where energyCalcConst = (1/(sampling freq  x 3600))
//! This API must be called after EM_calcActivePowerDiv.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//! \param[in] algo  is a pointer to the EM SW Algorithm structure
//! \param[in] result  is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcActiveEnergy(EM_Metrology *metro, EM_Algorithm_Configuration *algo,
				EM_Phase_Results *result);

//! \brief    The following API is used to calculate the dot product and
//! accumulation of Voltage_90 and Current. This API reads the current and voltage
//! values using the phase structure.It uses an IIR filter with 1 pole to shift the
//! the voltage 90 degrees: (all pass 90 degree filter for w = PI*f/fs, f=signal freq
//! and fs = sampling frequency)
//! V90(IDX0 = V(IDX-1)+G*[V(IDX) - V90(IDX-1)] G = (tan(w) - 1)/(tan(w) + 1)
//! The G constant used by the filter is based on signal
//! frequency which is updated ~ once a second. It updates metro Result structure;
//! storing the temporary multiply accumulate (MAC) calculations. The MAC value is
//! used by the foreground function EM_calcReactivePowerDiv to calculate Reactive Power.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Ping Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//! \param[in] bgData  is a pointer to the EM SW BG Result structure
//!
//! \return None
//!
extern void EM_calcReactivePowerMultAcc(EM_Metrology *metro, EM_Phase_Configuration *phase,
					EM_Phase_BG_Results *bgData);

//! \brief    The following API is used to calculate the reactive power in the
//! foreground code and store reactivePower in the result structure. The formula used:
//! (metro->RP_dot_prod->P_accum x metro->oneDivSC) x phase->phaseCalibPtr->reactivePowerSF
//! If the reactivePower calculated is less than algo->residualPowerCutoff
//! then reactivePower will be set to 0.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//! \param[in] result  is a pointer to the EM SW Result structure
//! \param[in] algo  is a pointer to the EM SW Algorithm structure
//!
//! \return None
//!
extern void EM_calcReactivePowerDiv(EM_Metrology *metro, EM_Phase_Configuration *phase,
				    EM_Phase_Results *result, EM_Algorithm_Configuration *algo);

//! \brief    The following API is used to calculate the reactive energy result.
//! The formula used to calculate reactiveEnergy (uVArh):
//! (result->reactivePower x metro->sampleCount) x energyCalcConst
//! Where energyCalcConst = (1/(sampling freq  x 3600))
//! This API must be called after EM_calcReactivePowerDiv.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//! \param[in] algo  is a pointer to the EM SW Algorithm structure
//! \param[in] result  is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcReactiveEnergy(EM_Metrology *metro, EM_Algorithm_Configuration *algo,
				  EM_Phase_Results *result);

//! \brief    The following API is used to calculate apparent power.
//! The formula used to calculate apparentPower (uVA):
//! Sqrt Root (result->activePower ^ 2 + result->reactivePower ^ 2)
//! This API must be called after EM_calcActivePowerMultAcc and
//! EM_calcReactivePowerMultAcc.
//!
//! \param[in] result  is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcApparentPower(EM_Phase_Results *result);

//! \brief    The following API is used to calculate the apparent energy result.
//! The formula used to calculate apparentEnergy (uVAh):
//! (result->apparentPower x metro->sampleCount) x energyCalcConst
//! Where energyCalcConst = (1/(sampling freq  x 3600))
//! This API must be called after EM_calcApparentPower.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//! \param[in] algo  is a pointer to the EM SW Algorithm structure
//! \param[in] result  is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcApparentEnergy(EM_Metrology *metro, EM_Algorithm_Configuration *algo,
				  EM_Phase_Results *result);

//! \brief    The following API is used to calculate power factor. The formula
//! used to calculate it:
//! activePower / apparentPower
//! This API must be called after EM_calcActivePowerMultAcc and
//! EM_calcApparentPower.
//!
//! \param[in] result  is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcPowerFactor(EM_Phase_Results *result);

//! \brief    The following API is used to calculate the dot product and
//! accumulation for VRMS. This API reads the voltage value from the voltage ptr
//! in the phase structure and updates the Metro Result structure; storing
//! the temporary multiply accumulate (MAC) calculations. The MAC value is used
//! by the foreground function EM_calcVRMSDivSqrt to calculate VRMS.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Ping Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//!
//! \return None
//!
extern void EM_calcVRMSMultAcc(EM_Metrology *metro, EM_Phase_Configuration *phase);

//! \brief    The following API is used to calculate the division and
//! square root for VRMS. This API uses the dot product calculated by
//! the EM_calcVRMSMultAcc function. It updates the global Phase Result structure.
//! The result can be read inside the result structure.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//! \param[in] result  is a pointer to the EM SW Library result handle
//!
//! \return None
//!
extern void EM_calcVRMSDivSqrt(EM_Metrology *metro, EM_Phase_Configuration *phase,
			       EM_Phase_Results *result);

//! \brief    The following API is used to calculate the dot product and
//! accumulation for IRMS. This API reads the current value from the current ptr
//! in the phase structure and updates the Metro Result structure; storing
//! the temporary multiply accumulate (MAC) calculations. The MAC value is used
//! by the foreground function EM_calcIRMSDivSqrt to calculate IRMS.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Ping Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//!
//! \return None
//!
extern void EM_calcIRMSMultAcc(EM_Metrology *metro, EM_Phase_Configuration *phase);

//! \brief    The following API is used to calculate the division and
//! square root for IRMS. This API uses the dot product calculated by
//! the EM_calcIRMSMultAcc function. It updates the global Phase Result structure.
//! The result can be read inside the result structure.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//! \param[in] result  is a pointer to the EM SW Library result handle
//!
//! \return None
//!
extern void EM_calcIRMSDivSqrt(EM_Metrology *metro, EM_Phase_Configuration *phase,
			       EM_Phase_Results *result);

//! \brief    The following API is used to calculate I Peak. This API updates
//! the global Phase Result structure. The formula used to calculate:
//! result->IRMS x algo->sqrtTwoConst
//! Where algo->sqrtTwoConst = Sqrt(2)
//! The result can be read inside the result structure.
//!
//! \param[in] result  is a pointer to the EM SW Result structure
//! \param[in] algo  is a pointer to the EM SW Algorithm structure
//!
//! \return None
//!
extern void EM_calcIPeak(EM_Phase_Results *result, EM_Algorithm_Configuration *algo);

//! \brief    The following API is used to calculate V Peak. This API updates
//! the global Phase Result structure.  The formula used to calculate:
//! result->VRMS x algo->sqrtTwoConst
//! Where algo->sqrtTwoConst = Sqrt(2)
//! The result can be read inside the result structure.
//!
//! \param[in] result  is a pointer to the EM SW Result structure
//! \param[in] algo  is a pointer to the EM SW Algorithm structure
//!
//! \return None
//!
extern void EM_calcVPeak(EM_Phase_Results *result, EM_Algorithm_Configuration *algo);

//! \brief    The following API is used to calculate the period of the voltage
//! signal. For systems with 2 voltages, this API must be used with only one
//! of the voltage signals. This API reads the voltage using the phase structure.
//! This API requires the initialization of the following using the function
//! count = (256 * Sampling Freq. / Fs ):
//! - EMSWLIB_EM_HAL_ADC_minFreqCount - should be calculated for Fs = 70 Hz
//! - EMSWLIB_EM_HAL_ADC_maxFreqCount - should be calculated for Fs = 40 Hz
//! For example For Sampling Freq = 4096,
//! minFreqCount  = (256*4096 / 70) = 14,980
//! maxFreqCount  = (256*4096 / 40) = 26,214
//! The Main Period is calculated using a running average of the main_period.
//! To calculate the period interpolation is used using a resolution of 256.
//! The period between two zero crossings (ZC_1 and ZC_2) is calculated using
//! the following formula:
//! Given,
//! XC_N (interpolated x value) = - 256 * Y_Prev / (Y_Act - Y_Prev)
//! Where Y_Prev and Y_Act are the Y values in the negative to positive transition
//! (e.g. Y_Prev < 0 and Y_Act >= 0 )
//! Delta_N = 256 - XC_N
//! Then,
//! Period_N = Delta_N-1 + 256*samples (not including Y_Prev and Y_Act) + XC_N
//! (e.g. for Period 2) = Delta_1 + 256*samples + XC_2
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Ping Ptr)
//! \param[in] phase  is a pointer to the EM SW Phase structure
//! \param[in] bgData  is a pointer to the EM SW BG Result structure
//!
//! \return None
//!
extern void EM_calcFreqPeriod(EM_Metrology *metro, EM_Phase_Configuration *phase,
			      EM_Phase_BG_Results *bgData);

//! \brief    The following API is used to calculate frequency. This API updates
//! the global Phase Result structure.  The formula used to calculate:
//! algo->freqSampling * 256L*100L / metro->mainPeriod
//! The result can be read inside the result structure.
//!
//! \param[in] metro  is a pointer to the EM SW Metrology structure (Pong Ptr)
//! \param[in] result  is a pointer to the EM SW Result structure
//! \param[in] algo  is a pointer to the EM SW Algorithm structure
//!
//! \return None
//!
extern void EM_calcFreq(EM_Metrology *metro, EM_Phase_Results *result,
			EM_Algorithm_Configuration *algo);

//! \brief    The following API is used to calculate total active power.
//! This API will aggregate the total power of the result->phaseResults
//! buffer using the indexes included in the phaseIdx buffer.
//!
//! \param[in] phaseIdx is a pointer to the phase indexes buffer
//! \param[in] length is the length of the phaseIdx buffer
//! \param[in] result is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcTotalActivePower(uint8_t *phaseIdx, uint8_t length,
				    EM_SW_Lib_Result_Handle *result);

//! \brief    The following API is used to calculate total reactive power.
//! This API will aggregate the total power of the result->phaseResults
//! buffer using the indexes included in the phaseIdx buffer.
//!
//! \param[in] phaseIdx is a pointer to the phase indexes buffer
//! \param[in] length is the length of the phaseIdx buffer
//! \param[in] result is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcTotalReactivePower(uint8_t *phaseIdx, uint8_t length,
				      EM_SW_Lib_Result_Handle *result);

//! \brief    The following API is used to calculate total apparent power.
//! This API will aggregate the total power of the result->phaseResults
//! buffer using the indexes included in the phaseIdx buffer.
//!
//! \param[in] phaseIdx is a pointer to the phase indexes buffer
//! \param[in] length is the length of the phaseIdx buffer
//! \param[in] result is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcTotalApparentPower(uint8_t *phaseIdx, uint8_t length,
				      EM_SW_Lib_Result_Handle *result);

//! \brief    The following API is used to calculate total active energy.
//! This API will aggregate the total energy of the result->phaseResults
//! buffer using the indexes included in the phaseIdx buffer.
//!
//! \param[in] phaseIdx is a pointer to the phase indexes buffer
//! \param[in] length is the length of the phaseIdx buffer
//! \param[in] result is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcTotalActiveEnergy(uint8_t *phaseIdx, uint8_t length,
				     EM_SW_Lib_Result_Handle *result);

//! \brief    The following API is used to calculate total reactive energy.
//! This API will aggregate the total energy of the result->phaseResults
//! buffer using the indexes included in the phaseIdx buffer.
//!
//! \param[in] phaseIdx is a pointer to the phase indexes buffer
//! \param[in] length is the length of the phaseIdx buffer
//! \param[in] result is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcTotalReactiveEnergy(uint8_t *phaseIdx, uint8_t length,
				       EM_SW_Lib_Result_Handle *result);

//! \brief    The following API is used to calculate total apparent energy.
//! This API will aggregate the total energy of the result->phaseResults
//! buffer using the indexes included in the phaseIdx buffer.
//!
//! \param[in] phaseIdx is a pointer to the phase indexes buffer
//! \param[in] length is the length of the phaseIdx buffer
//! \param[in] result is a pointer to the EM SW Result structure
//!
//! \return None
//!
extern void EM_calcTotalApparentEnergy(uint8_t *phaseIdx, uint8_t length,
				       EM_SW_Lib_Result_Handle *result);

//******************************************************************************
//
// Close the Doxygen group.
//! @}
//
//******************************************************************************

#ifdef __cplusplus
}
#endif // extern "C"

#endif // end of  _EMSWLIB_H_ definition
