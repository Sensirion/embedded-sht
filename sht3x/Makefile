# See user_config.inc for build customization
-include user_config.inc
include default_config.inc


.PHONY: all clean

all: sht3x_example_usage

sht3x_example_usage: clean
	$(CC) $(CFLAGS) -o $@ ${sht3x_sources} ${${CONFIG_I2C_TYPE}_sources} ${sht3x_dir}/sht3x_example_usage.c

clean:
	$(RM) sht3x_example_usage
