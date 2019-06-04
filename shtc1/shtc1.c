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
 * \brief Sensirion SHTC1 (and compatible) driver implementation
 *
 * This module provides access to the SHTC1 functionality over a generic I2C
 * interface. It supports measurements without clock stretching only.
 *
 * SHTC1 compatible sensors: SHTW1, SHTW2, SHTC3
 */

#include "shtc1.h"
#include "sensirion_arch_config.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"

/* all measurement commands return T (CRC) RH (CRC) */
#if USE_SENSIRION_CLOCK_STRETCHING
static const uint8_t CMD_MEASURE_HPM[] = {0x7C, 0xA2};
static const uint8_t CMD_MEASURE_LPM[] = {0x64, 0x58};
#else
static const uint8_t CMD_MEASURE_HPM[] = {0x78, 0x66};
static const uint8_t CMD_MEASURE_LPM[] = {0x60, 0x9C};
static const uint16_t MEASUREMENT_DURATION_USEC = 14400;
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
static const uint8_t CMD_READ_ID_REG[] = {0xef, 0xc8};
static const uint8_t COMMAND_SIZE = sizeof(CMD_MEASURE_HPM);

static const uint8_t SHTC3_CMD_SLEEP[] = {0xB0, 0x98};
static const uint8_t SHTC3_CMD_WAKEUP[] = {0x35, 0x17};
#ifdef SHT_ADDRESS
static const uint8_t SHTC1_ADDRESS = SHT_ADDRESS;
#else
static const uint8_t SHTC1_ADDRESS = 0x70;
#endif

static const uint16_t SHTC1_PRODUCT_CODE_MASK = 0x001F;
static const uint16_t SHTC1_PRODUCT_CODE = 0x0007;
static const uint16_t SHTC3_PRODUCT_CODE_MASK = 0x083F;
static const uint16_t SHTC3_PRODUCT_CODE = 0x0807;

static const uint8_t *cmd_measure = CMD_MEASURE_HPM;

static int8_t shtc1_read_ticks(uint8_t address, int32_t *temperature_ticks,
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

static int8_t shtc1_read_measurement(uint8_t address, int32_t *temperature,
                                     int32_t *humidity) {
    int8_t ret = shtc1_read_ticks(address, temperature, humidity);
    /**
     * formulas for conversion of the sensor signals, optimized for fixed point
     * algebra: Temperature       = 175 * S_T / 2^16 - 45 Relative Humidity =
     * 100 * S_RH / 2^16
     */
    *temperature = ((21875 * *temperature) >> 13) - 45000;
    *humidity = ((12500 * *humidity) >> 13);

    return ret;
}
/**
 * PM_SLEEP is equivalent to
 * if (ret) {
 *     (void)shtc1_sleep(); // attempting to sleep but ignore return value
 * } else {
 *     (ret) = shtc1_sleep();
 * }, (ret)
 */
#define PM_SLEEP(ret) ((ret) ? (shtc1_sleep(), (ret)) : ((ret) = shtc1_sleep()))

/**
 * PM_WAKE wakes the sensor if the command is successful (ret == 0).
 * The macro code is equivalent to:
 * (ret) = shtc1_wakeup();
 * if ((ret) == 0) {
 *     (ret) = (cmd);
 *     if ((ret) != 0)
 *          shtc1_sleep(); // cmd failed, go back to sleep
 * }, (ret)
 */
#define PM_WAKE(ret, cmd)                                                      \
    (((ret) = shtc1_wakeup())                                                  \
         ? (ret) /* ret = STATUS_WAKEUP_FAILED */                              \
         : (((ret) = (cmd))                                                    \
             ? shtc1_sleep(), (ret) /* ret = cmd failed (ret != 0),
                                       sensor potentially asleep */            \
             : (ret)) /* ret = STATUS_OK and sensor is awake */)

static uint8_t supports_sleep = 1;
static uint8_t sleep_enabled = 1;

static int8_t shtc1_sleep() {
    int8_t ret;

    if (!supports_sleep || !sleep_enabled)
        return STATUS_OK;

    ret = sensirion_i2c_write(SHTC1_ADDRESS, SHTC3_CMD_SLEEP, COMMAND_SIZE);
    if (ret != STATUS_OK)
        return STATUS_SLEEP_FAILED;
    return STATUS_OK;
}

static int8_t shtc1_wakeup() {
    int8_t ret;

    if (!supports_sleep || !sleep_enabled)
        return STATUS_OK;

    ret = sensirion_i2c_write(SHTC1_ADDRESS, SHTC3_CMD_WAKEUP, COMMAND_SIZE);
    if (ret != STATUS_OK)
        return STATUS_WAKEUP_FAILED;
    return STATUS_OK;
}

int8_t shtc1_measure_blocking_read(int32_t *temperature, int32_t *humidity) {
    int8_t ret;

    PM_WAKE(ret, shtc1_measure());
#if !defined(USE_SENSIRION_CLOCK_STRETCHING) || !USE_SENSIRION_CLOCK_STRETCHING
    sensirion_sleep_usec(MEASUREMENT_DURATION_USEC);
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
    ret = shtc1_read(temperature, humidity);
    return PM_SLEEP(ret);
}

int8_t shtc1_measure(void) {
    int8_t ret;

    return PM_WAKE(
        ret, sensirion_i2c_write(SHTC1_ADDRESS, cmd_measure, COMMAND_SIZE));
}

int8_t shtc1_read(int32_t *temperature, int32_t *humidity) {
    int8_t ret = shtc1_read_measurement(SHTC1_ADDRESS, temperature, humidity);

    return PM_SLEEP(ret);
}

int8_t shtc1_probe(void) {
    uint16_t id;
    uint8_t data[3];
    int8_t ret;

    supports_sleep = 1;
    sleep_enabled = 1;

    (void)shtc1_wakeup(); /* Try to wake up the sensor, ignore return value */
    ret = sensirion_i2c_write(SHTC1_ADDRESS, CMD_READ_ID_REG, COMMAND_SIZE);
    if (ret)
        return ret;

    ret = sensirion_i2c_read(SHTC1_ADDRESS, data, sizeof(data));
    if (ret)
        return ret;

    ret = sensirion_common_check_crc(data, 2, data[2]);
    if (ret)
        return ret;

    id = ((uint16_t)data[0] << 8) | data[1];
    if ((id & SHTC3_PRODUCT_CODE_MASK) == SHTC3_PRODUCT_CODE) {
        return shtc1_sleep();
    }

    if ((id & SHTC1_PRODUCT_CODE_MASK) == SHTC1_PRODUCT_CODE) {
        supports_sleep = 0;
        return STATUS_OK;
    }

    return STATUS_UNKNOWN_DEVICE;
}

int8_t shtc1_disable_sleep(uint8_t disable_sleep) {
    if (!supports_sleep)
        return STATUS_FAIL;

    sleep_enabled = !disable_sleep;

    if (disable_sleep)
        return shtc1_wakeup();

    return shtc1_sleep();
}

void shtc1_enable_low_power_mode(uint8_t enable_low_power_mode) {
    cmd_measure = enable_low_power_mode ? CMD_MEASURE_LPM : CMD_MEASURE_HPM;
}

const char *shtc1_get_driver_version(void) {
    return SHT_DRV_VERSION_STR;
}

uint8_t shtc1_get_configured_address(void) {
    return SHTC1_ADDRESS;
}
