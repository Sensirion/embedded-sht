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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *
 * \brief Sensirion SHTC1 driver implementation
 *
 * This module provides access to the SHTC1 functionality over a generic I2C
 * interface. It supports measurements without clock stretching only.
 */

#include "sensirion_arch_config.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"
#include "sht.h"
#include "sht_common.h"

/* all measurement commands return T (CRC) RH (CRC) */
#if USE_SENSIRION_CLOCK_STRETCHING
static const u8 CMD_MEASURE_HPM[]     = { 0x7C, 0xA2 };
#else
static const u8 CMD_MEASURE_HPM[]     = { 0x78, 0x66 };
#endif /* USE_SENSIRION_CLOCK_STRETCHING */
static const u8 CMD_READ_ID_REG[]     = { 0xef, 0xc8 };
static const u8 COMMAND_SIZE = sizeof(CMD_MEASURE_HPM);
#ifdef SHT_ADDRESS
static const u8 SHTC1_ADDRESS = SHT_ADDRESS;
#else
static const u8 SHTC1_ADDRESS = 0x70;
#endif

static const u8 ID_REG_CONTENT    = 0x07;
static const u8 ID_REG_MASK       = 0x1f;

static const u16 MEASUREMENT_DURATION_USEC = 14400;

s8 sht_measure_blocking_read(s32 *temperature, s32 *humidity)
{
    s8 ret = sht_measure();
    if (ret == STATUS_OK) {
        sensirion_sleep_usec(MEASUREMENT_DURATION_USEC);
        ret = sht_read(temperature, humidity);
    }
    return ret;
}

s8 sht_measure()
{
    return sensirion_i2c_write(SHTC1_ADDRESS, CMD_MEASURE_HPM, COMMAND_SIZE);
}

s8 sht_read(s32 *temperature, s32 *humidity)
{
    return sht_common_read_measurement(SHTC1_ADDRESS, temperature, humidity);
}

s8 sht_probe()
{
    u8 data[3];
    sensirion_i2c_init();
    s8 ret = sensirion_i2c_write(SHTC1_ADDRESS, CMD_READ_ID_REG, COMMAND_SIZE);
    if (ret)
        return ret;

    ret = sensirion_i2c_read(SHTC1_ADDRESS, data, sizeof(data));
    if (ret)
        return ret;

    ret = sensirion_common_check_crc(data, 2, data[2]);
    if (ret)
        return ret;

    if ((data[1] & ID_REG_MASK) != ID_REG_CONTENT)
        return STATUS_UNKNOWN_DEVICE;
    return STATUS_OK;
}

const char *sht_get_driver_version()
{
    return SHT_DRV_VERSION_STR;
}

u8 sht_get_configured_sht_address()
{
    return SHTC1_ADDRESS;
}
