## This file controls the custom user build settings.

## Choose either of hw_i2c or sw_i2c depending on whether you have a dedicated
## i2c controller (hw_i2c) or are using bit-banging on GPIOs (sw_i2c)
# CONFIG_I2C_TYPE = hw_i2c

## For hw_i2c, configure the i2c HAL implementation to use.
## Use one of the available sample-implementations or implement your own using
## the stub.
# hw_i2c_impl_src = ${sensirion_common_dir}/hw_i2c/sensirion_hw_i2c_implementation.c

## For sw_i2c, configure the GPIO implementation.
# sw_i2c_impl_src = ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c_implementation.c

##
## The items below are listed as documentation but may not need customization
##

## The build paths can also be changed here if needed
# sht_driver_dir = ..
# sensirion_common_dir = ${sht_driver_dir}/embedded-common
# sht_common_dir = ${sht_driver_dir}/sht-common
# sht4x_dir = ${sht_driver_dir}/sht4x

## If you need different CFLAGS, those can be customized as well
# CFLAGS = -Os -Wall -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC
