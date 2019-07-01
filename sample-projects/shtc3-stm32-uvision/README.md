# SHTC3 Sample Code for STM32

This uVision sample project shows the basic usage of Sensirion's SHTC3
temperature and humidity sensor.

## Compatibility
The code herein is compatible with Sensirion's SHTC3 digital temperature and
humidity sensor.

* Sensirion SHTC3 Sensor
* STM32 Discovery board ([STM32VLDISCOVERY])
* uVision (5.25)

## Introduction

Sensirion's developer page
[developer.sensirion.com](https://developer.sensirion.com) provides more
developer resources for different platforms and products.

## Cloning this Repository

```
git clone --recursive https://github.com/Sensirion/embedded-sht.git
```

## Building from the repository

If you cloned the repository, prior to building the project you need to execute
the following script in Git Bash:

```bash
./copy_shtc1_driver.sh
```

[STM32VLDISCOVERY]: https://www.st.com/en/evaluation-tools/stm32vldiscovery.html
