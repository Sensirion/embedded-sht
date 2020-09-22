include ../embedded-common/test-config/base_config.inc
sht_driver_dir := ${driver_dir}/embedded-sht
include ${sht_driver_dir}/sht3x/default_config.inc
include ${sht_driver_dir}/sht4x/default_config.inc
include ${sht_driver_dir}/shtc1/default_config.inc

sht3x_test_binaries := sht3x-test-hw_i2c sht3x-test-sw_i2c
sht4x_test_binaries := sht4x-test-hw_i2c sht4x-test-sw_i2c
shtc1_test_binaries := shtc1-test-hw_i2c shtc1-test-sw_i2c

.PHONY: all clean prepare test

all: clean prepare test

prepare:
	cd ${sht_driver_dir} && $(MAKE) prepare

sht3x-test-hw_i2c: CONFIG_I2C_TYPE := hw_i2c
sht3x-test-hw_i2c: sht3x-test.cpp ${sht3x_sources} ${hw_i2c_sources} ${sensirion_test_sources}
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

sht3x-test-sw_i2c: CONFIG_I2C_TYPE := sw_i2c
sht3x-test-sw_i2c: sht3x-test.cpp ${sht3x_sources} ${sw_i2c_sources} ${sensirion_test_sources}
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

sht4x-test-hw_i2c: CONFIG_I2C_TYPE := hw_i2c
sht4x-test-hw_i2c: sht4x-test.cpp ${sht4x_sources} ${hw_i2c_sources} ${sensirion_test_sources}
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

sht4x-test-sw_i2c: CONFIG_I2C_TYPE := sw_i2c
sht4x-test-sw_i2c: sht4x-test.cpp ${sht4x_sources} ${sw_i2c_sources} ${sensirion_test_sources}
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

shtc1-test-hw_i2c: CONFIG_I2C_TYPE := hw_i2c
shtc1-test-hw_i2c: shtc1-test.cpp ${shtc1_sources} ${hw_i2c_sources} ${sensirion_test_sources}
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

shtc1-test-sw_i2c: CONFIG_I2C_TYPE := sw_i2c
shtc1-test-sw_i2c: shtc1-test.cpp ${shtc1_sources} ${sw_i2c_sources} ${sensirion_test_sources}
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) ${sht4x_test_binaries} ${sht3x_test_binaries} ${shtc1_test_binaries}

test: prepare ${sht4x_test_binaries} ${sht3x_test_binaries} ${shtc1_test_binaries}
	set -ex; for test in ${sht3x_test_binaries}; do echo $${test}; ./$${test}; echo; done;
	set -ex; for test in ${sht4x_test_binaries}; do echo $${test}; ./$${test}; echo; done;
	set -ex; for test in ${shtc1_test_binaries}; do echo $${test}; ./$${test}; echo; done;
