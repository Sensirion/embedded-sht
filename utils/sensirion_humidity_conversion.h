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

#ifndef HUMIDITY_CONVERSION_H
#define HUMIDITY_CONVERSION_H
#include "sensirion_arch_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * sensirion_calc_absolute_humidity() - Calculate absolute humidity from
 *                                      temperature and relative humidity
 *
 * @param temperature_milli_celsius The temperature measurement in milli Degree
 *                                  Celsius, i.e. degree celsius multiplied by
 *                                  1000.
 * @param humidity_milli_percent    The relative humidity measurement in
 *                                  milli Percent, i.e.  percent relative
 *                                  humidity, multiplied by 1000 (0-100000)
 *
 * @return                          The absolute humidity in mg/m^3
 */
uint32_t sensirion_calc_absolute_humidity(int32_t temperature_milli_celsius,
                                          int32_t humidity_milli_percent);

#ifdef __cplusplus
}
#endif

#endif /* HUMIDITY_CONVERSION_H */
