# Changelog

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]


## [5.3.0] - 2021-03-16

 * [`added`] SHT3x driver added macros to easily parse the STATUS register relevant bits
 * [`added`] SHT3x driver support for alert thresholds configuration (read and write commands)
 * [`added`] SHT3x driver support for clear status register
 * [`added`] SHT3x driver allows use of 2 sensors in parallel (addresses 0x44 & 0x45)

## [5.2.1] - 2020-12-14

 * [`changed`] Makefile to only include needed files from embedded-common
 * [`changed`] Updated embedded-common to 0.1.0 to improve compatibility when
               using multiple embedded drivers

## [5.2.0] - 2020-09-10

 * [`added`]    SHT4x Support

## [5.1.0] - 2020-06-12

 * [`changed`]  Cleanup and better document stm32 sample code
 * [`changed`]  Use configuration independent endianness conversions. No more
                need to correctly set `SENSIRION_BIG_ENDIAN`

## [5.0.0] - 2020-04-29

 * [`changed`]  Replace SHTC3's auto-sleeping with manual sleeping for better
                control of the sensor and easier-to-read code.
                - Adds the functions `shtc1_sleep()` and `shtc1_wake_up()`
                  Despite the name, the functions only work on the SHTC3.
                - Remove `shtc1_disable_sleeping()`.
 * [`removed`]  Remove the `AUTHORS` file from the driver and the
                `embedded-common` submodule, as it adds more noise than benefit.
                The contributors can be found in the git log.
 * [`fixed`]    Copy correct `CHANGELOG.md` and `LICENSE` files to target
                locations when running the `release` target of the driver's root
                Makefile.
 * [`fixed`]    Fix uVision compilation warnings (#1295-D: Deprecated
                declaration of shtc1_sleep - give arg types)
 * [`added`]    Add convenience function convert relative to absolute humidity
 * [`added`]    Add convenience functions convert between Celsius and Fahrenheit

## [4.1.0] - 2019-09-13

 * [`fixed`]    Fix warnings about sign conversion
 * [`fixed`]    Improved compatibility with C++ compilers
 * [`added`]    Add functions to read out the serial id:
                - `sht3x_read_serial_id(int32_t *serial_id)`
                - `shtc1_read_serial_id(int32_t *serial_id)`
 * [`changed`]  Split out `default_config.inc` from Makefile to configure paths
                and CFLAGS for both SHTC1 and SHT3X drivers
 * [`changed`]  Only one example with either `hw_i2c` or `sw_i2c` is built,
                depending on `CONFIG_I2C_TYPE`. Defaults to `hw_i2c`.
 * [`changed`]  Move the defined constants `SHT3X_MEASUREMENT_DURATION_USEC`
                and `SHTC1_MEASUREMENT_DURATION_USEC` to their respective
                header files.

## [4.0.0] - 2019-07-01

 * [`changed`]  Return types are now `int16_t` instead of `int8_t` (in line with
                other embedded drivers).
 * [`changed`]  Functions are now dedicated per sensor (e.g. `sht3x_probe()`
                instead of `sht_probe()`)
 * [`added`]    New return values for wakeup/sleep failed
                (`STATUS_WAKEUP_FAILED`, `STATUS_SLEEP_FAILED`)
 * [`fixed`]    Fix SHTC3 issues when calling `probe()` multiple times
 * [`changed`]  Move the i2c init call out of `probe()` and into the example
 * [`changed`]  Rename `git_version.[ch]` to `sht_git_version.[ch]`

## [3.0.0] - 2019-05-14

 * [`changed`]  Use stdint types, e.g. `uint16_t` instead of `u16`

## [2.1.0] - 2018-06-27

 * [`added`]    Support for SHTC3's sleep mode (included in shtc1 package)
 * [`added`]    Support low power mode on SHTC1 (and family)

## [2.0.0] - 2018-05-14

 * First public release

[Unreleased]: https://github.com/Sensirion/embedded-sht/compare/5.3.0...master
[5.3.0]: https://github.com/Sensirion/embedded-sht/compare/5.2.1...5.3.0
[5.2.1]: https://github.com/Sensirion/embedded-sht/compare/5.2.0...5.2.1
[5.2.0]: https://github.com/Sensirion/embedded-sht/compare/5.1.0...5.2.0
[5.1.0]: https://github.com/Sensirion/embedded-sht/compare/5.0.0...5.1.0
[5.0.0]: https://github.com/Sensirion/embedded-sht/compare/4.1.0...5.0.0
[4.1.0]: https://github.com/Sensirion/embedded-sht/compare/4.0.0...4.1.0
[4.0.0]: https://github.com/Sensirion/embedded-sht/compare/3.0.0...4.0.0
[3.0.0]: https://github.com/Sensirion/embedded-sht/compare/2.1.0...3.0.0
[2.1.0]: https://github.com/Sensirion/embedded-sht/compare/2.0.0...2.1.0
[2.0.0]: https://github.com/Sensirion/embedded-sht/releases/tag/2.0.0
