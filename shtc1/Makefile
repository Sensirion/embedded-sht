sensirion_common_dir := ../embedded-common
sht_common_dir := ../sht-common
sw_i2c_dir := ${sensirion_common_dir}/sw_i2c
hw_i2c_dir := ${sensirion_common_dir}/hw_i2c

CFLAGS := -Wall -Wsign-conversion -I. -I${sensirion_common_dir} -I${sht_common_dir}

sensirion_common_objects := sensirion_common.o
sht_common_objects := sht_git_version.o
shtc1_binaries := shtc1_example_usage_sw_i2c shtc1_example_usage_hw_i2c
sht_binaries += ${shtc1_binaries}

sw_objects := ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c.o ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c_implementation.o
hw_objects := ${sensirion_common_dir}/hw_i2c/sensirion_hw_i2c_implementation.o
all_objects := ${sensirion_common_objects} ${sht_common_objects} ${hw_objects} ${sw_objects} shtc1.o

.PHONY: all

all: ${sht_binaries}

sht_git_version.o: ${sht_common_dir}/sht_git_version.c
	$(CC) $(CFLAGS) -c -o $@ $^
sensirion_common.o: ${sensirion_common_dir}/sensirion_common.c
	$(CC) $(CFLAGS) -c -o $@ $^
sensirion_sw_i2c_implementation.o: ${sw_i2c_dir}/sensirion_sw_i2c_implementation.c
	$(CC) $(CFLAGS) -c -o $@ $^
sensirion_hw_i2c_implementation.o: ${hw_i2c_dir}/sensirion_hw_i2c_implementation.c
	$(CC) $(CFLAGS) -c -o $@ $^

sensirion_sw_i2c.o: ${sw_i2c_dir}/sensirion_sw_i2c.c
	$(CC) -I${sw_i2c_dir} $(CFLAGS) -c -o $@ $^

shtc1.o: ${sensirion_common_dir}/sensirion_i2c.h ${sht_common_dir}/sht_git_version.c shtc1.h shtc1.c

shtc1_example_usage_sw_i2c: ${sensirion_common_objects} ${sht_common_objects} ${sw_objects} shtc1.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS) shtc1_example_usage.c

shtc1_example_usage_hw_i2c: ${sensirion_common_objects} ${sht_common_objects} ${hw_objects} shtc1.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS) shtc1_example_usage.c

clean:
	$(RM) ${all_objects} ${sht_binaries}
