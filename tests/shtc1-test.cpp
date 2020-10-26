#include "sensirion_common.h"
#include "sensirion_test_setup.h"
#include "shtc1.h"

static void shtc1_run_test() {
    int16_t ret;
    int32_t temperature;
    int32_t humidity;
    uint32_t serial;

    ret = shtc1_measure_blocking_read(&temperature, &humidity);
    CHECK_ZERO_TEXT(ret, "shtc1_measure_blocking_read");
    CHECK_TRUE_TEXT(temperature >= 5000 && temperature <= 45000,
                    "shtc1_measure_blocking_read temperature");
    CHECK_TRUE_TEXT(humidity >= 0 && humidity <= 100000,
                    "shtc1_measure_blocking_read humidity");

    ret = shtc1_measure();
    CHECK_ZERO_TEXT(ret, "shtc1_measure");

    sensirion_sleep_usec(SHTC1_MEASUREMENT_DURATION_USEC);

    ret = shtc1_read(&temperature, &humidity);
    CHECK_ZERO_TEXT(ret, "shtc1_read");
    CHECK_TRUE_TEXT(temperature >= 5000 && temperature <= 45000,
                    "shtc1_read temperature");
    CHECK_TRUE_TEXT(humidity >= 0 && humidity <= 100000, "shtc1_read humidity");

    ret = shtc1_read_serial(&serial);
    CHECK_ZERO_TEXT(ret, "shtc1_read_serial");
    printf("SHTC1 serial: %u\n", serial);

    const char* version = shtc1_get_driver_version();
    printf("shtc1_get_driver_version: %s\n", version);

    uint8_t addr = shtc1_get_configured_address();
    CHECK_EQUAL_TEXT(0x70, addr, "shtc1_get_configured_address");
}

static void shtc1_test_all_power_modes() {
    int16_t ret;

    printf("Running tests in normal mode...\n");
    ret = shtc1_probe();
    CHECK_ZERO_TEXT(ret, "shtc1_probe before normal mode");
    shtc1_run_test();

    printf("Running tests in low power mode...\n");
    ret = shtc1_probe();
    CHECK_ZERO_TEXT(ret, "shtc1_probe before low power mode");
    shtc1_enable_low_power_mode(1);
    shtc1_run_test();
}

static int16_t shtc1_test_sleep() {
    int16_t ret = shtc1_probe();
    CHECK_ZERO_TEXT(ret, "shtc1_probe before disabling sleep");
    ret = shtc1_sleep();
    return ret;
}

static void shtc1_sleep_success() {
    int16_t ret = shtc1_test_sleep();
    CHECK_ZERO_TEXT(ret, "shtc1_sleep should succeed, but it didn't");
    ret = shtc1_wake_up();
    CHECK_ZERO_TEXT(ret, "shtc1_wakeup should succeed, but it didn't");
}

static void shtc1_sleep_fail() {
    int16_t ret = shtc1_test_sleep();
    CHECK_TRUE_TEXT(ret, "shtc1_sleep should fail, but didn't");
    ret = shtc1_wake_up();
    CHECK_TRUE_TEXT(ret, "shtc1_wake_up should fail, but didn't");
}

static void test_teardown() {
    int16_t ret = sensirion_i2c_general_call_reset();
    CHECK_ZERO_TEXT(ret, "sensirion_i2c_general_call_reset");
    sensirion_i2c_release();
}

TEST_GROUP (SHTC1_Tests) {
    void setup() {
        sensirion_i2c_init();
        int16_t ret = sensirion_i2c_mux_set_single_channel(0x71, 6);
        CHECK_ZERO_TEXT(ret, "sensirion_i2c_mux_select_TEXT(0x71, 6)");
    }

    void teardown() {
        test_teardown();
    }
};

TEST_GROUP (SHTC3_Tests) {
    void setup() {
        sensirion_i2c_init();
        int16_t ret = sensirion_i2c_mux_set_single_channel(0x72, 0);
        CHECK_ZERO_TEXT(ret, "sensirion_i2c_mux_select_TEXT(0x72, 0)");
    }

    void teardown() {
        test_teardown();
    }
};

TEST_GROUP (SHTW2_Tests) {
    void setup() {
        sensirion_i2c_init();
        int16_t ret = sensirion_i2c_mux_set_single_channel(0x71, 0);
        CHECK_ZERO_TEXT(ret, "sensirion_i2c_mux_select_TEXT(0x71, 0)");
    }

    void teardown() {
        test_teardown();
    }
};

TEST (SHTC1_Tests, SHTC1Test) { shtc1_test_all_power_modes(); }

TEST (SHTC1_Tests, SHTC1Test_sleep) {
    shtc1_sleep_fail();
    shtc1_test_all_power_modes();
}

TEST (SHTC3_Tests, SHTC3Test) { shtc1_test_all_power_modes(); }

TEST (SHTC3_Tests, SHTC3Test_sleep) {
    shtc1_sleep_success();
    shtc1_test_all_power_modes();
}

TEST (SHTW2_Tests, SHTW2Test) { shtc1_test_all_power_modes(); }

TEST (SHTW2_Tests, SHTW2Test_sleep) {
    shtc1_sleep_fail();
    shtc1_test_all_power_modes();
}
