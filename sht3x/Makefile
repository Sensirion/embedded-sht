sensirion_common_dir := ../embedded-common
sht_common_dir := ../sht-common
sw_i2c_dir := ${sensirion_common_dir}/sw_i2c
hw_i2c_dir := ${sensirion_common_dir}/hw_i2c

CFLAGS := -Wall -I. -I${sensirion_common_dir} -I${sht_common_dir}

sensirion_common_objects := sensirion_common.o
sht_common_objects := sht_common.o git_version.o
sht3x_binaries := sht3x_example_usage_sw_i2c sht3x_example_usage_hw_i2c
sht_binaries += ${sht3x_binaries}

sw_objects := ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c.o ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c_implementation.o
hw_objects := ${sensirion_common_dir}/hw_i2c/sensirion_hw_i2c_implementation.o
all_objects := ${sensirion_common_objects} ${sht_common_objects} ${hw_objects} ${sw_objects} sht3x.o

.PHONY: all

all: ${sht_binaries}

git_version.o: ${sht_common_dir}/git_version.c
	$(CC) $(CFLAGS) -c -o $@ $^
sht_common.o: ${sht_common_dir}/sht_common.c
	$(CC) $(CFLAGS) -c -o $@ $^
sensirion_common.o: ${sensirion_common_dir}/sensirion_common.c
	$(CC) $(CFLAGS) -c -o $@ $^
sensirion_sw_i2c_implementation.o: ${sw_i2c_dir}/sensirion_sw_i2c_implementation.c
	$(CC) $(CFLAGS) -c -o $@ $^
sensirion_hw_i2c_implementation.o: ${hw_i2c_dir}/sensirion_hw_i2c_implementation.c
	$(CC) $(CFLAGS) -c -o $@ $^

sensirion_sw_i2c.o: ${sw_i2c_dir}/sensirion_sw_i2c.c
	$(CC) -I${sw_i2c_dir} $(CFLAGS) -c -o $@ $^

sht3x.o: ${sensirion_common_dir}/sensirion_i2c.h ${sht_common_dir}/git_version.c ${sht_common_dir}/sht.h sht3x.c

sht3x_example_usage_sw_i2c: ${sensirion_common_objects} ${sht_common_objects} ${sw_objects} sht3x.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS) ${sht_common_dir}/example_usage.c

sht3x_example_usage_hw_i2c: ${sensirion_common_objects} ${sht_common_objects} ${hw_objects} sht3x.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS) ${sht_common_dir}/example_usage.c

clean:
	$(RM) ${all_objects} ${sht_binaries}
