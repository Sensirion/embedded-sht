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

/**
 * \file
 *
 * \brief Sensirion SHT4X driver implementation
 *
 * This module provides access to the SHT4X functionality over a generic I2C
 * interface. It supports measurements without clock stretching only.
 */

#include "sht4x.h"
#include "sensirion_arch_config.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"

/* all measurement commands return T (CRC) RH (CRC) */
#define SHT4X_CMD_MEASURE_HPM 0xFD
#define SHT4X_CMD_MEASURE_LPM 0xE0
#define SHT4X_CMD_READ_SERIAL 0x89
#define SHT4X_CMD_DURATION_USEC 1000

#define SHT4X_ADDRESS 0x44

static uint8_t sht4x_cmd_measure = SHT4X_CMD_MEASURE_HPM;
static uint16_t sht4x_cmd_measure_delay_us = SHT4X_MEASUREMENT_DURATION_USEC;

int16_t sht4x_measure_blocking_read(int32_t* temperature, int32_t* humidity) {
    int16_t ret;

    ret = sht4x_measure();
    if (ret)
        return ret;
    sensirion_sleep_usec(sht4x_cmd_measure_delay_us);
    return sht4x_read(temperature, humidity);
}

int16_t sht4x_measure(void) {
    return sensirion_i2c_write(SHT4X_ADDRESS, &sht4x_cmd_measure, 1);
}

int16_t sht4x_read(int32_t* temperature, int32_t* humidity) {
    uint16_t words[2];
    int16_t ret = sensirion_i2c_read_words(SHT4X_ADDRESS, words,
                                           SENSIRION_NUM_WORDS(words));
    /**
     * formulas for conversion of the sensor signals, optimized for fixed point
     * algebra:
     * Temperature = 175 * S_T / 65535 - 45
     * Relative Humidity = 125 * (S_RH / 65535) - 6
     */
    *temperature = ((21875 * (int32_t)words[0]) >> 13) - 45000;
    *humidity = ((15625 * (int32_t)words[1]) >> 13) - 6000;

    return ret;
}

int16_t sht4x_probe(void) {
    uint32_t serial;

    return sht4x_read_serial(&serial);
}

void sht4x_enable_low_power_mode(uint8_t enable_low_power_mode) {
    if (enable_low_power_mode) {
        sht4x_cmd_measure = SHT4X_CMD_MEASURE_LPM;
        sht4x_cmd_measure_delay_us = SHT4X_MEASUREMENT_DURATION_LPM_USEC;
    } else {
        sht4x_cmd_measure = SHT4X_CMD_MEASURE_HPM;
        sht4x_cmd_measure_delay_us = SHT4X_MEASUREMENT_DURATION_USEC;
    }
}

int16_t sht4x_read_serial(uint32_t* serial) {
    const uint8_t cmd = SHT4X_CMD_READ_SERIAL;
    int16_t ret;
    uint16_t serial_words[SENSIRION_NUM_WORDS(*serial)];

    ret = sensirion_i2c_write(SHT4X_ADDRESS, &cmd, 1);
    if (ret)
        return ret;

    sensirion_sleep_usec(SHT4X_CMD_DURATION_USEC);
    ret = sensirion_i2c_read_words(SHT4X_ADDRESS, serial_words,
                                   SENSIRION_NUM_WORDS(serial_words));
    *serial = ((uint32_t)serial_words[0] << 16) | serial_words[1];

    return ret;
}

const char* sht4x_get_driver_version(void) {
    return SHT_DRV_VERSION_STR;
}

uint8_t sht4x_get_configured_address(void) {
    return SHT4X_ADDRESS;
}
