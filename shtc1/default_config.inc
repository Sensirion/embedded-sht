sht_driver_dir ?= ..
sensirion_common_dir ?= ${sht_driver_dir}/embedded-common
sht_common_dir ?= ${sht_driver_dir}/sht-common
shtc1_dir ?= ${sht_driver_dir}/shtc1
CONFIG_I2C_TYPE ?= hw_i2c

sw_i2c_impl_src ?= ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c_implementation.c
hw_i2c_impl_src ?= ${sensirion_common_dir}/hw_i2c/sensirion_hw_i2c_implementation.c

CFLAGS ?= -Os -Wall -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC
CFLAGS += -I${sensirion_common_dir} -I${sht_common_dir} -I${shtc1_dir} \
          -I${sensirion_common_dir}/${CONFIG_I2C_TYPE}

sensirion_common_sources = ${sensirion_common_dir}/sensirion_arch_config.h \
                           ${sensirion_common_dir}/sensirion_i2c.h \
                           ${sensirion_common_dir}/sensirion_common.h \
                           ${sensirion_common_dir}/sensirion_common.c

sht_common_sources = ${sht_common_dir}/sht_git_version.h \
                     ${sht_common_dir}/sht_git_version.c

shtc1_sources = ${sensirion_common_sources} ${sht_common_sources} \
                ${shtc1_dir}/shtc1.h ${shtc1_dir}/shtc1.c

hw_i2c_sources = ${hw_i2c_impl_src}
sw_i2c_sources = ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c_gpio.h \
                 ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c.c \
                 ${sw_i2c_impl_src}
