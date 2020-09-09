# embedded-sht [![CircleCI](https://circleci.com/gh/Sensirion/embedded-sht.svg?style=shield)](https://circleci.com/gh/Sensirion/embedded-sht) [![GitHub license](https://img.shields.io/badge/license-BSD3-blue.svg)](https://raw.githubusercontent.com/Sensirion/embedded-sht/master/LICENSE)
This repository contains the embedded driver sources for Sensirion's
SHT product line.

## Download the Latest Driver Release
**Download the latest ready-to-use driver bundle from the [releases
page](https://github.com/Sensirion/embedded-sht/releases/)**

We strongly recommend to use the driver from the release bundle instead of
cloning the repository.

## Clone this repository
```
 git clone --recursive https://github.com/Sensirion/embedded-sht.git
```

## Repository content
* `embedded-common` submodule repository for the common embedded driver HAL
* `sht-common` common files for all SHTxx drivers, humidity conversion functions
* `sht4x` SHT4 driver
* `sht3x` SHT3x/SHT8x driver
* `shtc1` SHTC3/SHTC1/SHTW1/SHTW2 driver
* `utils` Conversion functions (Centigrade to Fahrenheit, %RH relative humidity
          to aboslute humidity)

## Collecting resources
```
make release
```
This will create the `release` folder with the necessary driver files in it,
including a Makefile. That way, you have just ONE folder with all the sources
ready to build your driver for your platform.

## Files to adjust (from embedded-common)
You only need to touch the following files:

* `sensirion_arch_config.h` (architecture specifics, you need to specify
  the integer sizes)

and depending on your i2c implementation either of the following:

* `embedded-common/hw_i2c/sensirion_hw_i2c_implementation.c`
  functions for hardware i2c communication if your platform supports that
* `embedded-common/sw_i2c/sensirion_sw_i2c_implementation.c`
  functions for software i2c communication via GPIOs

## Building the driver
1. Adjust `sensirion_arch_config.h` if you don't have the `<stdint.h>` header
   file available
2. Implement necessary functions in one of the `*_implementation.c` files
   described above
3. make

---

Please check the [embedded-common](https://github.com/Sensirion/embedded-common)
repository for further information and sample implementations.

---
