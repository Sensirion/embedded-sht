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

#include "shtc1.h"
#include "system.h"
#include <stdbool.h>

static void led_init(void);
static void led_blue(bool on);
static void led_green(bool on);

int main(void) {
    /* Initialize the i2c bus for the current platform */
    sensirion_i2c_init();

    led_init();

    /* Busy loop for initialization, because the main loop does not work without
     * a sensor.
     */
    while (shtc1_probe() != STATUS_OK) {
        /* Blink LED as long as probing fails */
        led_green(true);
        sensirion_sleep_usec(100000);
        led_green(false);
        sensirion_sleep_usec(100000);
    }

    /* probe sucessfull -> green LED on */
    led_green(true);

    while (1) {
        int32_t temperature, humidity;
        /* Measure temperature and relative humidity and store into variables
         * temperature, humidity (each output multiplied by 1000).
         */
        int8_t ret = shtc1_measure_blocking_read(&temperature, &humidity);
        if (ret == STATUS_OK) {
            led_green(true);
            /* if the Relative Humidity is over 50% light up the blue LED */
            led_blue(humidity > 50000);
        } else {
            /* error -> green LED off */
            led_green(false);
        }
        /* wait 1 second */
        sensirion_sleep_usec(1000000);
    }
}

/* -- adapt this code for your platform -- */
static void led_init(void) {
    RCC->APB2ENR |= 0x00000010; /* I/O port C clock enabled */
    GPIOC->CRH &= 0xFFFFFF00;   /* set general purpose output mode for LEDs */
    GPIOC->CRH |= 0x00000011;
    GPIOC->BSRR = 0x03000000; /* LEDs off */
}

/* -- adapt this code for your platform -- */
static void led_blue(bool on) {
    if (on) {
        GPIOC->BSRR = 0x00000100;
    } else {
        GPIOC->BSRR = 0x01000000;
    }
}

/* -- adapt this code for your platform -- */
static void led_green(bool on) {
    if (on) {
        GPIOC->BSRR = 0x00000200;
    } else {
        GPIOC->BSRR = 0x02000000;
    }
}
