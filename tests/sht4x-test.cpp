#include "sensirion_common.h"
#include "sensirion_test_setup.h"
#include "sht4x.h"

static void sht4x_run_test() {
    int16_t ret;
    int32_t temperature;
    int32_t humidity;
    uint32_t serial;

    ret = sht4x_measure_blocking_read(&temperature, &humidity);
    CHECK_ZERO_TEXT(ret, "sht4x_measure_blocking_read");
    CHECK_TRUE_TEXT(temperature >= 5000 && temperature <= 45000,
                    "sht4x_measure_blocking_read temperature");
    CHECK_TRUE_TEXT(humidity >= 0 && humidity <= 100000,
                    "sht4x_measure_blocking_read humidity");

    ret = sht4x_measure();
    CHECK_ZERO_TEXT(ret, "sht4x_measure");

    sensirion_sleep_usec(SHT4X_MEASUREMENT_DURATION_USEC);

    ret = sht4x_read(&temperature, &humidity);
    CHECK_ZERO_TEXT(ret, "sht4x_read");
    CHECK_TRUE_TEXT(temperature >= 5000 && temperature <= 45000,
                    "sht4x_read temperature");
    CHECK_TRUE_TEXT(humidity >= 0 && humidity <= 100000, "sht4x_read humidity");

    ret = sht4x_read_serial(&serial);
    CHECK_ZERO_TEXT(ret, "sht4x_read_serial");
    printf("SHT4X serial: %u\n", serial);

    const char* version = sht4x_get_driver_version();
    printf("sht4x_get_driver_version: %s\n", version);

    uint8_t addr = sht4x_get_configured_address();
    CHECK_EQUAL_TEXT(0x44, addr, "sht4x_get_configured_address");
}

static void sht4x_test_all_power_modes() {
    int16_t ret = sht4x_probe();
    CHECK_ZERO_TEXT(ret, "sht4x_probe");

    printf("Running tests in normal mode...\n");
    sht4x_run_test();

    printf("Running tests in low power mode...\n");
    sht4x_enable_low_power_mode(1);
    sht4x_run_test();
}

static void test_teardown() {
    int16_t ret = sensirion_i2c_general_call_reset();
    CHECK_ZERO_TEXT(ret, "sensirion_i2c_general_call_reset");
    sensirion_i2c_release();
}

TEST_GROUP (SHT4X_Tests) {
    void setup() {
        sensirion_i2c_init();
        int16_t ret = sensirion_i2c_mux_set_single_channel(0x71, 7);
        CHECK_ZERO_TEXT(ret, "sensirion_i2c_mux_select_TEXT(0x71, 7)");
    }

    void teardown() {
        test_teardown();
    }
};

TEST (SHT4X_Tests, SHT4XTest) { sht4x_test_all_power_modes(); }
