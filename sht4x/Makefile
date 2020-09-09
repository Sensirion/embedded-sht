# See user_config.inc for build customization
-include user_config.inc
include default_config.inc


.PHONY: all clean

all: sht4x_example_usage

sht4x_example_usage: clean
	$(CC) $(CFLAGS) -o $@ ${sht4x_sources} ${${CONFIG_I2C_TYPE}_sources} ${sht4x_dir}/sht4x_example_usage.c

clean:
	$(RM) sht4x_example_usage
