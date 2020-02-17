sht_driver_dir ?= ..
sensirion_common_dir ?= ${sht_driver_dir}/embedded-common
sht_utils_dir ?= ${sht_driver_dir}/utils

CFLAGS ?= -Os -Wall -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC
CFLAGS += -I${sht_utils_dir} -I${sensirion_common_dir}

sensirion_humidity_conversion_sources = \
    ${sht_utils_dir}/sensirion_humidity_conversion.h \
    ${sht_utils_dir}/sensirion_humidity_conversion.c

sensirion_temperature_unit_conversion_sources = \
    ${sht_utils_dir}/sensirion_temperature_unit_conversion.h \
    ${sht_utils_dir}/sensirion_temperature_unit_conversion.c
