/*
 * Copyright (c) 2018, Sensirion AG
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
 * \brief Sensirion SHT3x driver implementation
 *
 * This module provides access to the SHT3x functionality over a generic I2C
 * interface. It supports measurements without clock stretching only.
 */

#include "sht3x.h"
#include "sensirion_arch_config.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"

/* all measurement commands return T (CRC) RH (CRC) */
#if USE_SENSIRION_CLOCK_STRETCHING
#define SHT3X_CMD_MEASURE_HPM 0x2C06
#define SHT3X_CMD_MEASURE_LPM 0x2C10
#else /* USE_SENSIRION_CLOCK_STRETCHING */
#define SHT3X_CMD_MEASURE_HPM 0x2400
#define SHT3X_CMD_MEASURE_LPM 0x2416
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
static const uint16_t SHT3X_CMD_READ_STATUS_REG = 0xF32D;
static const uint16_t SHT3X_CMD_READ_SERIAL_ID = 0x3780;
static const uint16_t SHT3X_CMD_DURATION_USEC = 1000;
#ifdef SHT_ADDRESS
static const uint8_t SHT3X_ADDRESS = SHT_ADDRESS;
#else
static const uint8_t SHT3X_ADDRESS = 0x44;
#endif

static uint16_t sht3x_cmd_measure = SHT3X_CMD_MEASURE_HPM;

int16_t sht3x_measure_blocking_read(int32_t* temperature, int32_t* humidity) {
    int16_t ret = sht3x_measure();
    if (ret == STATUS_OK) {
#if !defined(USE_SENSIRION_CLOCK_STRETCHING) || !USE_SENSIRION_CLOCK_STRETCHING
        sensirion_sleep_usec(SHT3X_MEASUREMENT_DURATION_USEC);
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
        ret = sht3x_read(temperature, humidity);
    }
    return ret;
}

int16_t sht3x_measure(void) {
    return sensirion_i2c_write_cmd(SHT3X_ADDRESS, sht3x_cmd_measure);
}

int16_t sht3x_read(int32_t* temperature, int32_t* humidity) {
    uint16_t words[2];
    int16_t ret = sensirion_i2c_read_words(SHT3X_ADDRESS, words,
                                           SENSIRION_NUM_WORDS(words));
    /**
     * formulas for conversion of the sensor signals, optimized for fixed point
     * algebra: Temperature = 175 * S_T / 2^16 - 45
     * Relative Humidity = * 100 * S_RH / 2^16
     */
    *temperature = ((21875 * (int32_t)words[0]) >> 13) - 45000;
    *humidity = ((12500 * (int32_t)words[1]) >> 13);

    return ret;
}

int16_t sht3x_probe(void) {
    uint16_t status;
    return sensirion_i2c_delayed_read_cmd(SHT3X_ADDRESS,
                                          SHT3X_CMD_READ_STATUS_REG,
                                          SHT3X_CMD_DURATION_USEC, &status, 1);
}

void sht3x_enable_low_power_mode(uint8_t enable_low_power_mode) {
    sht3x_cmd_measure =
        enable_low_power_mode ? SHT3X_CMD_MEASURE_LPM : SHT3X_CMD_MEASURE_HPM;
}

int16_t sht3x_read_serial(uint32_t* serial) {
    int16_t ret;
    uint8_t serial_bytes[4];

    ret = sensirion_i2c_write_cmd(SHT3X_ADDRESS, SHT3X_CMD_READ_SERIAL_ID);
    if (ret)
        return ret;

    sensirion_sleep_usec(SHT3X_CMD_DURATION_USEC);

    ret = sensirion_i2c_read_words_as_bytes(SHT3X_ADDRESS, serial_bytes,
                                            SENSIRION_NUM_WORDS(serial_bytes));
    *serial = sensirion_bytes_to_uint32_t(serial_bytes);
    return ret;
}

const char* sht3x_get_driver_version(void) {
    return SHT_DRV_VERSION_STR;
}

uint8_t sht3x_get_configured_address(void) {
    return SHT3X_ADDRESS;
}
