#include "sensirion_test_setup.h"
#include "sht3x.h"

static void sht3x_run_test() {
    int16_t ret;
    int32_t temperature;
    int32_t humidity;
    uint32_t serial;

    ret = sht3x_measure_blocking_read(&temperature, &humidity);
    CHECK_ZERO_TEXT(ret, "sht3x_measure_blocking_read");
    CHECK_TRUE_TEXT(temperature >= 5000 && temperature <= 45000,
                    "sht3x_measure_blocking_read temperature");
    CHECK_TRUE_TEXT(humidity >= 0 && humidity <= 100000,
                    "sht3x_measure_blocking_read humidity");

    ret = sht3x_measure();
    CHECK_ZERO_TEXT(ret, "sht3x_measure");

    sensirion_sleep_usec(SHT3X_MEASUREMENT_DURATION_USEC);

    ret = sht3x_read(&temperature, &humidity);
    CHECK_ZERO_TEXT(ret, "sht3x_read");
    CHECK_TRUE_TEXT(temperature >= 5000 && temperature <= 45000,
                    "sht3x_read temperature");
    CHECK_TRUE_TEXT(humidity >= 0 && humidity <= 100000,
                    "sht3x_read humidity");

    ret = sht3x_read_serial(&serial);
    CHECK_ZERO_TEXT(ret, "sht3x_read_serial");
    printf("SHT3X serial: %u\n", serial);

    const char *version = sht3x_get_driver_version();
    printf("sht3x_get_driver_version: %s\n", version);

    uint8_t addr = sht3x_get_configured_address();
    CHECK_EQUAL_TEXT(0x44, addr, "sht3x_get_configured_address");
}

static void sht3x_test_all_power_modes() {
    int16_t ret = sht3x_probe();
    CHECK_ZERO_TEXT(ret, "sht3x_probe");

    printf("Running tests in normal mode...\n");
    sht3x_run_test();

    printf("Running tests in low power mode...\n");
    sht3x_enable_low_power_mode(1);
    sht3x_run_test();
}

static void test_teardown() {
    int16_t ret = i2c_reset();
    CHECK_ZERO_TEXT(ret, "i2c_reset");
    sensirion_i2c_release();
}

TEST_GROUP(SHT31_Tests) {
    void setup() {
        sensirion_i2c_init();
        int16_t ret = sensirion_i2c_mux_set_single_channel(0x72, 0);
        CHECK_ZERO_TEXT(ret, "sensirion_i2c_mux_select_TEXT(0x72, 0)");
    }

    void teardown() {
        test_teardown();
    }
};

TEST(SHT31_Tests, SHT31Test) {
    sht3x_test_all_power_modes();
}
