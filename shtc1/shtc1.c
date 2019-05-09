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

#include "sensirion_arch_config.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"
#include "sht.h"
#include "sht_common.h"

/* all measurement commands return T (CRC) RH (CRC) */
#if USE_SENSIRION_CLOCK_STRETCHING
static const u8 CMD_MEASURE_HPM[] = {0x7C, 0xA2};
static const u8 CMD_MEASURE_LPM[] = {0x64, 0x58};
#else
static const u8 CMD_MEASURE_HPM[] = {0x78, 0x66};
static const u8 CMD_MEASURE_LPM[] = {0x60, 0x9C};
static const u16 MEASUREMENT_DURATION_USEC = 14400;
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
static const u8 CMD_READ_ID_REG[] = {0xef, 0xc8};
static const u8 COMMAND_SIZE = sizeof(CMD_MEASURE_HPM);

static const u8 SHTC3_CMD_SLEEP[] = {0xB0, 0x98};
static const u8 SHTC3_CMD_WAKEUP[] = {0x35, 0x17};
#ifdef SHT_ADDRESS
static const u8 SHTC1_ADDRESS = SHT_ADDRESS;
#else
static const u8 SHTC1_ADDRESS = 0x70;
#endif

static const u16 SHTC1_PRODUCT_CODE_MASK = 0x001F;
static const u16 SHTC1_PRODUCT_CODE = 0x0007;
static const u16 SHTC3_PRODUCT_CODE_MASK = 0x083F;
static const u16 SHTC3_PRODUCT_CODE = 0x0807;

static const u8 *cmd_measure = CMD_MEASURE_HPM;

/**
 * PM_SLEEP is equivalent to
 * if (ret) {
 *     (void)sht_sleep(); // attempting to sleep but ignore return value
 * } else {
 *     (ret) = sht_sleep();
 * }, (ret)
 */
#define PM_SLEEP(ret) ((ret) ? (sht_sleep(), (ret)) : ((ret) = sht_sleep()))

/**
 * PM_WAKE is equivalent to
 * (ret) = sht_wakeup();
 * if (ret)
 *     (void)sht_sleep(); // attempting to sleep but ignore return value
 * else {
 *     (ret) = (cmd);
 *     if (!(ret))
 *          sht_sleep(); // attempting to sleep but ignore return value
 * }, (ret)
 */
#define PM_WAKE(ret, cmd)                                                      \
    (((ret) = sht_wakeup()) ? (sht_sleep(), (ret))                             \
                            : ((ret) = (cmd) ? sht_sleep(), (ret) : (ret)))

static u8 supports_sleep = 1;
static u8 sleep_enabled = 1;

static u8 sht_sleep() {
    if (!supports_sleep || !sleep_enabled)
        return STATUS_OK;

    return sensirion_i2c_write(SHTC1_ADDRESS, SHTC3_CMD_SLEEP, COMMAND_SIZE);
}

static u8 sht_wakeup() {
    if (!supports_sleep || !sleep_enabled)
        return STATUS_OK;

    return sensirion_i2c_write(SHTC1_ADDRESS, SHTC3_CMD_WAKEUP, COMMAND_SIZE);
}

s8 sht_measure_blocking_read(s32 *temperature, s32 *humidity) {
    s8 ret;

    PM_WAKE(ret, sht_measure());
#if !defined(USE_SENSIRION_CLOCK_STRETCHING) || !USE_SENSIRION_CLOCK_STRETCHING
    sensirion_sleep_usec(MEASUREMENT_DURATION_USEC);
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
    ret = sht_read(temperature, humidity);
    return PM_SLEEP(ret);
}

s8 sht_measure() {
    s8 ret;

    return PM_WAKE(
        ret, sensirion_i2c_write(SHTC1_ADDRESS, cmd_measure, COMMAND_SIZE));
}

s8 sht_read(s32 *temperature, s32 *humidity) {
    s8 ret = sht_common_read_measurement(SHTC1_ADDRESS, temperature, humidity);

    return PM_SLEEP(ret);
}

s8 sht_probe() {
    u8 data[3];
    u16 id;

    sensirion_i2c_init();
    s8 ret = sensirion_i2c_write(SHTC1_ADDRESS, CMD_READ_ID_REG, COMMAND_SIZE);
    if (ret) {
        /* SHTC3 that's sleeping? */
        if (sht_wakeup())
            return ret; /* ..no */
        /* ..potentially, try again */
        ret = sensirion_i2c_write(SHTC1_ADDRESS, CMD_READ_ID_REG, COMMAND_SIZE);
        if (ret)
            return ret;
    }

    ret = sensirion_i2c_read(SHTC1_ADDRESS, data, sizeof(data));
    if (ret)
        return ret;

    ret = sensirion_common_check_crc(data, 2, data[2]);
    if (ret)
        return ret;

    id = ((u16)data[0] << 8) | data[1];
    if ((id & SHTC3_PRODUCT_CODE_MASK) == SHTC3_PRODUCT_CODE) {
        supports_sleep = 1;
        return sht_sleep();
    }

    if ((id & SHTC1_PRODUCT_CODE_MASK) == SHTC1_PRODUCT_CODE) {
        supports_sleep = 0;
        return STATUS_OK;
    }

    return STATUS_UNKNOWN_DEVICE;
}

s8 sht_disable_sleep(u8 disable_sleep) {
    if (!supports_sleep)
        return STATUS_FAIL;

    sleep_enabled = !disable_sleep;

    if (disable_sleep)
        return sht_wakeup();

    return sht_sleep();
}

void sht_enable_low_power_mode(u8 enable_low_power_mode) {
    cmd_measure = enable_low_power_mode ? CMD_MEASURE_LPM : CMD_MEASURE_HPM;
}

const char *sht_get_driver_version() {
    return SHT_DRV_VERSION_STR;
}

u8 sht_get_configured_sht_address() {
    return SHTC1_ADDRESS;
}
