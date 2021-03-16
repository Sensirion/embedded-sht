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
#define SHT3X_CMD_MEASURE_MPM 0x2C0D
#define SHT3X_CMD_MEASURE_LPM 0x2C10
#else /* USE_SENSIRION_CLOCK_STRETCHING */
#define SHT3X_CMD_MEASURE_HPM 0x2400
#define SHT3X_CMD_MEASURE_MPM 0x240B
#define SHT3X_CMD_MEASURE_LPM 0x2416
#endif /* USE_SENSIRION_CLOCK_STRETCHING */

#define SHT3X_HUMIDITY_LIMIT_MSK 0xFE00U
#define SHT3X_TEMPERATURE_LIMIT_MSK 0x01FFU

static const uint16_t SHT3X_CMD_READ_STATUS_REG = 0xF32D;
static const uint16_t SHT3X_CMD_CLR_STATUS_REG = 0x3041;
static const uint16_t SHT3X_CMD_READ_SERIAL_ID = 0x3780;
static const uint16_t SHT3X_CMD_DURATION_USEC = 1000;
/* read commands for the alert settings */
static const uint16_t SHT3X_CMD_READ_HIALRT_LIM_SET = 0xE11F;
static const uint16_t SHT3X_CMD_READ_HIALRT_LIM_CLR = 0xE114;
static const uint16_t SHT3X_CMD_READ_LOALRT_LIM_CLR = 0xE109;
static const uint16_t SHT3X_CMD_READ_LOALRT_LIM_SET = 0xE102;
/* write commands for the alert settings */
static const uint16_t SHT3X_CMD_WRITE_HIALRT_LIM_SET = 0x611D;
static const uint16_t SHT3X_CMD_WRITE_HIALRT_LIM_CLR = 0x6116;
static const uint16_t SHT3X_CMD_WRITE_LOALRT_LIM_CLR = 0x610B;
static const uint16_t SHT3X_CMD_WRITE_LOALRT_LIM_SET = 0x6100;

static uint16_t sht3x_cmd_measure = SHT3X_CMD_MEASURE_HPM;

int16_t sht3x_measure_blocking_read(sht3x_i2c_addr_t addr, int32_t* temperature,
                                    int32_t* humidity) {
    int16_t ret = sht3x_measure(addr);
    if (ret == STATUS_OK) {
#if !defined(USE_SENSIRION_CLOCK_STRETCHING) || !USE_SENSIRION_CLOCK_STRETCHING
        sensirion_sleep_usec(SHT3X_MEASUREMENT_DURATION_USEC);
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
        ret = sht3x_read(addr, temperature, humidity);
    }
    return ret;
}

int16_t sht3x_measure(sht3x_i2c_addr_t addr) {
    return sensirion_i2c_write_cmd(addr, sht3x_cmd_measure);
}

int16_t sht3x_read(sht3x_i2c_addr_t addr, int32_t* temperature,
                   int32_t* humidity) {
    uint16_t words[2];
    int16_t ret =
        sensirion_i2c_read_words(addr, words, SENSIRION_NUM_WORDS(words));
    /**
     * formulas for conversion of the sensor signals, optimized for fixed point
     * algebra: Temperature = 175 * S_T / 2^16 - 45
     * Relative Humidity = * 100 * S_RH / 2^16
     */
    tick_to_temperature(words[0], temperature);
    tick_to_humidity(words[1], humidity);

    return ret;
}

int16_t sht3x_probe(sht3x_i2c_addr_t addr) {
    uint16_t status;
    return sensirion_i2c_delayed_read_cmd(addr, SHT3X_CMD_READ_STATUS_REG,
                                          SHT3X_CMD_DURATION_USEC, &status, 1);
}

int16_t sht3x_get_status(sht3x_i2c_addr_t addr, uint16_t* status) {
    return sensirion_i2c_delayed_read_cmd(addr, SHT3X_CMD_READ_STATUS_REG,
                                          SHT3X_CMD_DURATION_USEC, status, 1);
}

int16_t sht3x_clear_status(sht3x_i2c_addr_t addr) {
    return sensirion_i2c_write_cmd(addr, SHT3X_CMD_CLR_STATUS_REG);
}

void sht3x_enable_low_power_mode(uint8_t enable_low_power_mode) {
    sht3x_cmd_measure =
        enable_low_power_mode ? SHT3X_CMD_MEASURE_LPM : SHT3X_CMD_MEASURE_HPM;
}

void sht3x_set_power_mode(sht3x_measurement_mode_t mode) {

    switch (mode) {
        case SHT3X_MEAS_MODE_LPM: {
            sht3x_cmd_measure = SHT3X_CMD_MEASURE_LPM;
            break;
        }
        case SHT3X_MEAS_MODE_MPM: {
            sht3x_cmd_measure = SHT3X_CMD_MEASURE_MPM;
            break;
        }
        case SHT3X_MEAS_MODE_HPM: {
            sht3x_cmd_measure = SHT3X_CMD_MEASURE_HPM;
            break;
        }
        default: {
            sht3x_cmd_measure = SHT3X_CMD_MEASURE_HPM;
            break;
        }
    }
}

int16_t sht3x_read_serial(sht3x_i2c_addr_t addr, uint32_t* serial) {
    int16_t ret;
    uint8_t serial_bytes[4];

    ret = sensirion_i2c_write_cmd(addr, SHT3X_CMD_READ_SERIAL_ID);
    sensirion_sleep_usec(SHT3X_CMD_DURATION_USEC);

    if (ret == STATUS_OK) {

        ret = sensirion_i2c_read_words_as_bytes(
            addr, serial_bytes, SENSIRION_NUM_WORDS(serial_bytes));
        *serial = sensirion_bytes_to_uint32_t(serial_bytes);
    }
    return ret;
}

const char* sht3x_get_driver_version(void) {
    return SHT_DRV_VERSION_STR;
}

int16_t sht3x_set_alert_thd(sht3x_i2c_addr_t addr, sht3x_alert_thd_t thd,
                            uint32_t humidity, int32_t temperature) {
    int16_t ret;
    uint16_t rawT;
    uint16_t rawRH;
    uint16_t limitVal = 0U;

    temperature_to_tick(temperature, &rawT);
    humidity_to_tick(humidity, &rawRH);

    /* convert inputs to alert threshold word */
    limitVal = (rawRH & SHT3X_HUMIDITY_LIMIT_MSK);
    limitVal |= ((rawT >> 7) & SHT3X_TEMPERATURE_LIMIT_MSK);

    switch (thd) {
        case SHT3X_HIALRT_SET:
            ret = sensirion_i2c_write_cmd_with_args(
                addr, SHT3X_CMD_WRITE_HIALRT_LIM_SET, &limitVal, 1);
            break;

        case SHT3X_HIALRT_CLR:
            ret = sensirion_i2c_write_cmd_with_args(
                addr, SHT3X_CMD_WRITE_HIALRT_LIM_CLR, &limitVal, 1);
            break;

        case SHT3X_LOALRT_CLR:
            ret = sensirion_i2c_write_cmd_with_args(
                addr, SHT3X_CMD_WRITE_LOALRT_LIM_CLR, &limitVal, 1);
            break;

        case SHT3X_LOALRT_SET:
            ret = sensirion_i2c_write_cmd_with_args(
                addr, SHT3X_CMD_WRITE_LOALRT_LIM_SET, &limitVal, 1);
            break;

        default:
            ret = STATUS_ERR_INVALID_PARAMS;
            break;
    }
    return ret;
}

int16_t sht3x_get_alert_thd(sht3x_i2c_addr_t addr, sht3x_alert_thd_t thd,
                            int32_t* humidity, int32_t* temperature) {

    int16_t ret;
    uint16_t word;
    uint16_t rawT;
    uint16_t rawRH;

    switch (thd) {
        case SHT3X_HIALRT_SET:
            ret = sensirion_i2c_read_cmd(addr, SHT3X_CMD_READ_HIALRT_LIM_SET,
                                         &word, 1);
            break;

        case SHT3X_HIALRT_CLR:
            ret = sensirion_i2c_read_cmd(addr, SHT3X_CMD_READ_HIALRT_LIM_CLR,
                                         &word, 1);
            break;

        case SHT3X_LOALRT_CLR:
            ret = sensirion_i2c_read_cmd(addr, SHT3X_CMD_READ_LOALRT_LIM_CLR,
                                         &word, 1);
            break;

        case SHT3X_LOALRT_SET:
            ret = sensirion_i2c_read_cmd(addr, SHT3X_CMD_READ_LOALRT_LIM_SET,
                                         &word, 1);
            break;

        default:
            ret = STATUS_ERR_INVALID_PARAMS;
            break;
    }

    /* convert threshold word to alert settings in 10*%RH & 10*°C */
    rawRH = (word & SHT3X_HUMIDITY_LIMIT_MSK);
    rawT = ((word & SHT3X_TEMPERATURE_LIMIT_MSK) << 7);

    tick_to_humidity(rawRH, humidity);
    tick_to_temperature(rawT, temperature);

    return ret;
}

void tick_to_temperature(uint16_t tick, int32_t* temperature) {
    *temperature = ((21875 * (int32_t)tick) >> 13) - 45000;
}

void tick_to_humidity(uint16_t tick, int32_t* humidity) {
    *humidity = ((12500 * (int32_t)tick) >> 13);
}

void temperature_to_tick(int32_t temperature, uint16_t* tick) {
    *tick = (uint16_t)((temperature * 12271 + 552195000) >> 15);
}

void humidity_to_tick(int32_t humidity, uint16_t* tick) {
    *tick = (uint16_t)((humidity * 21474) >> 15);
}