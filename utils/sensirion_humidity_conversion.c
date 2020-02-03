/*
 * Copyright (c) 2020, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "sensirion_humidity_conversion.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))
#endif /* ARRAY_SIZE */

/* T_LO and T_HI parametrize the first and last temperature step of the absolute
 * humidity lookup table (at 100%RH). The lookup table entries have to be
 * linearly spaced. We provide a python script to generate look up tables based
 * on customizable T_LO/T_HI and number of steps. */

/**
 * T_LO - Towest temperature sampling point in the lookup table.
 * Temperature value in milli-degrees Centigrade
 */
#define T_LO (-20000)

/**
 * T_HI - Towest temperature sampling point in the lookup table.
 * Temperature value in milli-degrees Celsius (Centigrade)
 */
#define T_HI (70000)

/**
 * Lookup table for linearly spaced temperature points between T_LO and T_HI
 * Absolute Humidity value in mg/m^3.
 */
static const uint32_t AH_LUT_100RH[] = {1078,  2364,  4849,  9383,   17243,
                                        30264, 50983, 82785, 130048, 198277};
/**
 * T_STEP is the temperature step between the sampling points in the lookup
 * table. It is determined by T_HI, T_LO and the number of entries in
 * AH_LUT_100RH and does not have to be adapted when changing the paramters.
 */
static const uint32_t T_STEP = (T_HI - T_LO) / (ARRAY_SIZE(AH_LUT_100RH) - 1);

uint32_t sensirion_calc_absolute_humidity(int32_t temperature_milli_celsius,
                                          int32_t humidity_milli_percent) {
    uint32_t t, i, rem, ret;

    if (humidity_milli_percent <= 0)
        return 0;

    if (temperature_milli_celsius < T_LO)
        t = 0;
    else
        t = (uint32_t)(temperature_milli_celsius - T_LO);

    i = t / T_STEP;
    rem = t % T_STEP;

    if (i >= ARRAY_SIZE(AH_LUT_100RH) - 1) {
        ret = AH_LUT_100RH[ARRAY_SIZE(AH_LUT_100RH) - 1];

    } else if (rem == 0) {
        ret = AH_LUT_100RH[i];

    } else {
        ret = (AH_LUT_100RH[i] +
               ((AH_LUT_100RH[i + 1] - AH_LUT_100RH[i]) * rem / T_STEP));
    }

    // Code is mathematically (but not numerically) equivalent to
    //    return (ret * (humidity_milli_percent)) / 100000;
    // Maximum ret = 198277 (Or last entry from AH_LUT_100RH)
    // Maximum humidity_milli_percent = 119000 (theoretical maximum)
    // Multiplication might overflow with a maximum of 3 digits
    // Trick: ((ret >> 3) * (uint32_t)humidity_milli_percent) never overflows
    // Now we only need to divide by 12500, as the tripple righ shift
    // divides by 8

    return ((ret >> 3) * (uint32_t)(humidity_milli_percent)) / 12500;
}
