/*
 * Copyright (c) 2019, Sensirion AG
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

#include "sw_i2c/sensirion_sw_i2c_gpio.h"
#include "system.h"

//-- Defines for IO-Pins -------------------------------------------------------
// I2C IO-Pins                         /* -- adapt the defines for your uC -- */

// SDA on port B, bit 9
#define SDA_LOW() (GPIOB->BSRR = 0x02000000)   // set SDA to low
#define SDA_OPEN() (GPIOB->BSRR = 0x00000200)  // set SDA to open-drain
#define SDA_READ() (GPIOB->IDR & 0x0200)       // read SDA

// SCL on port B, bit 8                /* -- adapt the defines for your uC -- */
#define SCL_LOW() (GPIOB->BSRR = 0x01000000)   // set SCL to low
#define SCL_OPEN() (GPIOB->BSRR = 0x00000100)  // set SCL to open-drain
#define SCL_READ() (GPIOB->IDR & 0x0100)       // read SCL

/**
 * Initialize all hard- and software components that are needed to set the
 * SDA and SCL pins.
 */
void sensirion_init_pins(void) {
    RCC->APB2ENR |= 0x00000008;  // I/O port B clock enabled

    SDA_OPEN();  // I2C-bus idle mode SDA released
    SCL_OPEN();  // I2C-bus idle mode SCL released

    // SDA on port B, bit 9
    // SCL on port B, bit 8
    GPIOB->CRH &= 0xFFFFFF00;  // set open-drain output for SDA and SCL
    GPIOB->CRH |= 0x00000055;  //
}

void sensirion_release_pins(void) {
    // do nothing
}

/**
 * Configure the SDA pin as an input. With an external pull-up resistor the line
 * should be left floating, without external pull-up resistor, the input must be
 * configured to use the internal pull-up resistor.
 */
void sensirion_SDA_in() {
    SDA_OPEN();
}

/**
 * Configure the SDA pin as an output and drive it low or set to logical false.
 */
void sensirion_SDA_out() {
    SDA_LOW();
}

/**
 * Read the value of the SDA pin.
 * @returns 0 if the pin is low and 1 otherwise.
 */
uint8_t sensirion_SDA_read() {
    return SDA_READ() ? 1 : 0;
}

/**
 * Configure the SCL pin as an input. With an external pull-up resistor the line
 * should be left floating, without external pull-up resistor, the input must be
 * configured to use the internal pull-up resistor.
 */
void sensirion_SCL_in() {
    SCL_OPEN();
}

/**
 * Configure the SCL pin as an output and drive it low or set to logical false.
 */
void sensirion_SCL_out() {
    SCL_LOW();
}

/**
 * Read the value of the SCL pin.
 * @returns 0 if the pin is low and 1 otherwise.
 */
uint8_t sensirion_SCL_read() {
    return SCL_READ() ? 1 : 0;
}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution approximately, but no less than, the given time.
 *
 * The precision needed depends on the desired i2c frequency, i.e. should be
 * exact to about half a clock cycle (defined in
 * `SENSIRION_I2C_CLOCK_PERIOD_USEC` in `sensirion_arch_config.h`).
 *
 * Example with 400kHz requires a precision of 1 / (2 * 400kHz) == 1.25usec.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_sleep_usec(uint32_t useconds) {
    for (uint32_t i = 0; i < useconds; i++) {
        __nop();  // nop's may be added or removed for timing adjustment
        __nop();
        __nop();
        __nop();
    }
}
