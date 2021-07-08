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
//! \file   emReactivePowerIIRCoeff.h
//!
//! \brief  This file contains EM Reactive Power Coefficients
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

#ifndef _EMREACTIVEPOWERIIRCOEFF_H_
#define _EMREACTIVEPOWERIIRCOEFF_H_
// IIR G Constant Table
// Samp. Freq=8192Hz, Min Freq=58Hz, Max Freq=62Hz, Resolution=0.04Hz
#define IIR_COEFF_MIN_FREQ 5800
#define IIR_COEFF_MAX_FREQ 6200
#define IIR_COEFF_TABLE_LEN 101
#define IIR_COEFF_SHIFT 2

const _iq30 G_CONSTANT[IIR_COEFF_TABLE_LEN] = {
	0xC2C91B0F, /* Freq 58.00 */
	0xC2C99642, /* Freq 58.04 */
	0xC2CA1174, /* Freq 58.08 */
	0xC2CA8CA5, /* Freq 58.12 */
	0xC2CB07D5, /* Freq 58.16 */
	0xC2CB8304, /* Freq 58.20 */
	0xC2CBFE33, /* Freq 58.24 */
	0xC2CC7960, /* Freq 58.28 */
	0xC2CCF48D, /* Freq 58.32 */
	0xC2CD6FB8, /* Freq 58.36 */
	0xC2CDEAE3, /* Freq 58.40 */
	0xC2CE660C, /* Freq 58.44 */
	0xC2CEE135, /* Freq 58.48 */
	0xC2CF5C5D, /* Freq 58.52 */
	0xC2CFD784, /* Freq 58.56 */
	0xC2D052AA, /* Freq 58.60 */
	0xC2D0CDCF, /* Freq 58.64 */
	0xC2D148F3, /* Freq 58.68 */
	0xC2D1C416, /* Freq 58.72 */
	0xC2D23F39, /* Freq 58.76 */
	0xC2D2BA5A, /* Freq 58.80 */
	0xC2D3357A, /* Freq 58.84 */
	0xC2D3B09A, /* Freq 58.88 */
	0xC2D42BB9, /* Freq 58.92 */
	0xC2D4A6D6, /* Freq 58.96 */
	0xC2D521F3, /* Freq 59.00 */
	0xC2D59D0F, /* Freq 59.04 */
	0xC2D6182A, /* Freq 59.08 */
	0xC2D69344, /* Freq 59.12 */
	0xC2D70E5D, /* Freq 59.16 */
	0xC2D78975, /* Freq 59.20 */
	0xC2D8048C, /* Freq 59.24 */
	0xC2D87FA2, /* Freq 59.28 */
	0xC2D8FAB8, /* Freq 59.32 */
	0xC2D975CC, /* Freq 59.36 */
	0xC2D9F0E0, /* Freq 59.40 */
	0xC2DA6BF2, /* Freq 59.44 */
	0xC2DAE704, /* Freq 59.48 */
	0xC2DB6215, /* Freq 59.52 */
	0xC2DBDD24, /* Freq 59.56 */
	0xC2DC5833, /* Freq 59.60 */
	0xC2DCD341, /* Freq 59.64 */
	0xC2DD4E4E, /* Freq 59.68 */
	0xC2DDC95A, /* Freq 59.72 */
	0xC2DE4466, /* Freq 59.76 */
	0xC2DEBF70, /* Freq 59.80 */
	0xC2DF3A79, /* Freq 59.84 */
	0xC2DFB582, /* Freq 59.88 */
	0xC2E03089, /* Freq 59.92 */
	0xC2E0AB90, /* Freq 59.96 */
	0xC2E12695, /* Freq 60.00 */
	0xC2E1A19A, /* Freq 60.04 */
	0xC2E21C9E, /* Freq 60.08 */
	0xC2E297A1, /* Freq 60.12 */
	0xC2E312A3, /* Freq 60.16 */
	0xC2E38DA4, /* Freq 60.20 */
	0xC2E408A4, /* Freq 60.24 */
	0xC2E483A3, /* Freq 60.28 */
	0xC2E4FEA2, /* Freq 60.32 */
	0xC2E5799F, /* Freq 60.36 */
	0xC2E5F49B, /* Freq 60.40 */
	0xC2E66F97, /* Freq 60.44 */
	0xC2E6EA91, /* Freq 60.48 */
	0xC2E7658B, /* Freq 60.52 */
	0xC2E7E084, /* Freq 60.56 */
	0xC2E85B7C, /* Freq 60.60 */
	0xC2E8D673, /* Freq 60.64 */
	0xC2E95169, /* Freq 60.68 */
	0xC2E9CC5E, /* Freq 60.72 */
	0xC2EA4752, /* Freq 60.76 */
	0xC2EAC245, /* Freq 60.80 */
	0xC2EB3D37, /* Freq 60.84 */
	0xC2EBB829, /* Freq 60.88 */
	0xC2EC3319, /* Freq 60.92 */
	0xC2ECAE09, /* Freq 60.96 */
	0xC2ED28F7, /* Freq 61.00 */
	0xC2EDA3E5, /* Freq 61.04 */
	0xC2EE1ED2, /* Freq 61.08 */
	0xC2EE99BE, /* Freq 61.12 */
	0xC2EF14A8, /* Freq 61.16 */
	0xC2EF8F92, /* Freq 61.20 */
	0xC2F00A7C, /* Freq 61.24 */
	0xC2F08564, /* Freq 61.28 */
	0xC2F1004B, /* Freq 61.32 */
	0xC2F17B31, /* Freq 61.36 */
	0xC2F1F617, /* Freq 61.40 */
	0xC2F270FB, /* Freq 61.44 */
	0xC2F2EBDF, /* Freq 61.48 */
	0xC2F366C1, /* Freq 61.52 */
	0xC2F3E1A3, /* Freq 61.56 */
	0xC2F45C84, /* Freq 61.60 */
	0xC2F4D764, /* Freq 61.64 */
	0xC2F55243, /* Freq 61.68 */
	0xC2F5CD21, /* Freq 61.72 */
	0xC2F647FE, /* Freq 61.76 */
	0xC2F6C2DA, /* Freq 61.80 */
	0xC2F73DB5, /* Freq 61.84 */
	0xC2F7B890, /* Freq 61.88 */
	0xC2F83369, /* Freq 61.92 */
	0xC2F8AE42, /* Freq 61.96 */
	0xC2F92919, /* Freq 62.00 */
};
#endif
