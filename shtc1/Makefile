# See user_config.inc for build customization
-include user_config.inc
include default_config.inc


.PHONY: all clean

all: shtc1_example_usage

shtc1_example_usage: clean
	$(CC) $(CFLAGS) -o $@ ${shtc1_sources} ${${CONFIG_I2C_TYPE}_sources} ${shtc1_dir}/shtc1_example_usage.c

clean:
	$(RM) shtc1_example_usage
