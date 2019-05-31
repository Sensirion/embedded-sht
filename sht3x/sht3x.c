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
static const uint8_t CMD_MEASURE_HPM[] = {0x2C, 0x06};
static const uint8_t CMD_MEASURE_LPM[] = {0x2C, 0x10};
#else
static const uint8_t CMD_MEASURE_HPM[] = {0x24, 0x00};
static const uint8_t CMD_MEASURE_LPM[] = {0x24, 0x16};
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
static const uint8_t CMD_READ_STATUS_REG[] = {0xF3, 0x2D};
static const uint8_t COMMAND_SIZE = sizeof(CMD_MEASURE_HPM);
#ifdef SHT_ADDRESS
static const uint8_t SHT3X_ADDRESS = SHT_ADDRESS;
#else
static const uint8_t SHT3X_ADDRESS = 0x44;
#endif

static const uint16_t MEASUREMENT_DURATION_USEC = 15000;

static const uint8_t *cmd_measure = CMD_MEASURE_HPM;

static int8_t sht3x_read_ticks(uint8_t address, int32_t *temperature_ticks,
                               int32_t *humidity_ticks) {
    uint8_t data[6];
    int8_t ret = sensirion_i2c_read(address, data, sizeof(data));
    if (ret)
        return ret;
    if (sensirion_common_check_crc(data, 2, data[2]) ||
        sensirion_common_check_crc(data + 3, 2, data[5])) {
        return STATUS_CRC_FAIL;
    }

    *temperature_ticks = (data[1] & 0xff) | ((int32_t)data[0] << 8);
    *humidity_ticks = (data[4] & 0xff) | ((int32_t)data[3] << 8);

    return STATUS_OK;
}

static int8_t sht3x_read_measurement(uint8_t address, int32_t *temperature,
                                     int32_t *humidity) {
    int8_t ret = sht3x_read_ticks(address, temperature, humidity);
    /**
     * formulas for conversion of the sensor signals, optimized for fixed point
     * algebra: Temperature       = 175 * S_T / 2^16 - 45 Relative Humidity =
     * 100 * S_RH / 2^16
     */
    *temperature = ((21875 * *temperature) >> 13) - 45000;
    *humidity = ((12500 * *humidity) >> 13);

    return ret;
}

int8_t sht3x_measure_blocking_read(int32_t *temperature, int32_t *humidity) {
    int8_t ret = sht3x_measure();
    if (ret == STATUS_OK) {
        sensirion_sleep_usec(MEASUREMENT_DURATION_USEC);
        ret = sht3x_read(temperature, humidity);
    }
    return ret;
}

int8_t sht3x_measure() {
    return sensirion_i2c_write(SHT3X_ADDRESS, CMD_MEASURE_HPM, COMMAND_SIZE);
}

int8_t sht3x_read(int32_t *temperature, int32_t *humidity) {
    return sht3x_read_measurement(SHT3X_ADDRESS, temperature, humidity);
}

int8_t sht3x_probe(void) {
    uint8_t data[3];

    int8_t ret =
        sensirion_i2c_write(SHT3X_ADDRESS, CMD_READ_STATUS_REG, COMMAND_SIZE);
    if (ret)
        return ret;

    ret = sensirion_i2c_read(SHT3X_ADDRESS, data, sizeof(data));
    if (ret)
        return ret;

    ret = sensirion_common_check_crc(data, 2, data[2]);
    if (ret)
        return ret;
    return STATUS_OK;
}

int8_t sht3x_disable_sleep(uint8_t disable_sleep) {
    return STATUS_FAIL; /* sleep mode not supported */
}

void sht3x_enable_low_power_mode(uint8_t enable_low_power_mode) {
    cmd_measure = enable_low_power_mode ? CMD_MEASURE_LPM : CMD_MEASURE_HPM;
}

const char *sht3x_get_driver_version(void) {
    return SHT_DRV_VERSION_STR;
}

uint8_t sht3x_get_configured_address(void) {
    return SHT3X_ADDRESS;
}
