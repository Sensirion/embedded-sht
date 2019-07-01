# Changelog

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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

[Unreleased]: https://github.com/Sensirion/embedded-sht/compare/4.0.0...master
[4.0.0]: https://github.com/Sensirion/embedded-sht/compare/3.0.0...4.0.0
[3.0.0]: https://github.com/Sensirion/embedded-sht/compare/2.1.0...3.0.0
[2.1.0]: https://github.com/Sensirion/embedded-sht/compare/2.0.0...2.1.0
[2.0.0]: https://github.com/Sensirion/embedded-sht/releases/tag/2.0.0
