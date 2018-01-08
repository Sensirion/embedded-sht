sensirion_common_dir := ./embedded-common
CFLAGS := -Wall -Werror -I. -I${sensirion_common_dir}

common_objects := sht_common.o example_usage.o
sw_objects := ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c.o ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c_implementation.o
hw_objects := ${sensirion_common_dir}/hw_i2c/sensirion_hw_i2c_implementation.o
all_objects := ${common_objects} ${hw_objects} ${sw_objects} shtc1.o sht3x.o \
	test_projects/firefly/release/RHT*-*.flow test_projects/firefly/release/AuxSensorDesigner_*.c

binaries := example_usage_shtc1_sw example_usage_shtc1_hw example_usage_sht3x_sw example_usage_sht3x_hw

.PHONY: firefly release

# all: ${binaries} firefly release
all: ${binaries} release

${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c.o: ${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c.c sht.h \
	${sensirion_common_dir}/sensirion_i2c.h \
	${sensirion_common_dir}/sw_i2c/sensirion_sw_i2c_gpio.h

shtc1.o: shtc1.c sht.h ${sensirion_common_dir}/sensirion_i2c.h

sht3x.o: sht3x.c sht.h ${sensirion_common_dir}/sensirion_i2c.h

example_usage.o: example_usage.c sht.h

example_usage_shtc1_sw: ${common_objects} ${sw_objects} shtc1.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

example_usage_sht3x_sw: ${common_objects} ${sw_objects} sht3x.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

example_usage_shtc1_hw: ${common_objects} ${hw_objects} shtc1.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

example_usage_sht3x_hw: ${common_objects} ${hw_objects} sht3x.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

firefly:
	./test_projects/firefly/assemble_sensor_studio_project.py --driver sht3x --compensation --airtouch
	$(CC) -fsyntax-only -DDRIVER=sht3x test_projects/firefly/check-env.c
	$(CC) -fsyntax-only test_projects/firefly/CompensationSensor.c
	$(CC) -fsyntax-only test_projects/firefly/AirTouchSensor.c

release:
	./release.py
	for directory in release/*; do \
		echo $${directory}; \
		cd $${directory}; \
		$(CC) $(LDFLAGS) -o example_usage_$$(basename $${directory}) *.c; \
		$(RM) example_usage_$$(basename $${directory}); \
		cd -; \
	done

clean:
	rm -rf ${all_objects} ${binaries} release
