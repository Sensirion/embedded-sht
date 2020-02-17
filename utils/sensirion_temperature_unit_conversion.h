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

#ifndef SENSIRION_TEMPERATURE_UNIT_CONVERSION_H
#define SENSIRION_TEMPERATURE_UNIT_CONVERSION_H
#include "sensirion_arch_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * sensirion_celsius_to_fahrenheit() - Convert temperature in degree Celsius
 *                                     (Centigrade) to degree Fahrenheit
 *
 * Note that inputs not in the range -291degC <= degC <= 291 will result in
 * invalid results.
 *
 * @param temperature_milli_celsius     The temperature measurement in milli
 *                                      degree Celsius, i.e. degree Celsius
 *                                      multiplied by 1000.
 *
 * @return                              The temperature measurement in milli
 *                                      degree Fahrenheit, i.e. degree
 *                                      Fahrenheit multiplied by 1000.
 */
int32_t sensirion_celsius_to_fahrenheit(int32_t temperature_milli_celsius);

/**
 * sensirion_fahrenheit_to_celsius() - Convert temperature in degree Fahrenheit
 *                                     to degree Celsius (Centigrade)
 *
 * Note that inputs not in the range -3571degF <= degF <= 3635degF will result
 * in invalid results.
 *
 * @param temperature_milli_fahrenheit  The temperature measurement in milli
 *                                      degree Fahrenheit, i.e. degree
 *                                      Fahrenheit multiplied by 1000.
 *
 * @return                              The temperature measurement in milli
 *                                      degree Celsius, i.e. degree Celsius
 *                                      multiplied by 1000.
 */
int32_t sensirion_fahrenheit_to_celsius(int32_t temperature_milli_fahrenheit);

#ifdef __cplusplus
}
#endif

#endif /* SENSIRION_TEMPERATURE_UNIT_CONVERSION_H */
