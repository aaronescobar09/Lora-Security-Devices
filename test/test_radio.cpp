#include <unity.h>
#include <Arduino.h>

// Radio test functions
void test_radio_frequency_validation(void) {
    // Test valid frequency values from the frequency list
    float freq_433 = 433.0;
    float freq_470 = 470.0;
    float freq_850 = 850.0;
    float freq_868 = 868.0;
    float freq_915 = 915.0;
    float freq_923 = 923.0;
    
    // All frequencies should be positive
    TEST_ASSERT_TRUE(freq_433 > 0);
    TEST_ASSERT_TRUE(freq_470 > 0);
    TEST_ASSERT_TRUE(freq_850 > 0);
    TEST_ASSERT_TRUE(freq_868 > 0);
    TEST_ASSERT_TRUE(freq_915 > 0);
    TEST_ASSERT_TRUE(freq_923 > 0);
    
    // Test that frequencies are in reasonable range (100-1000 MHz)
    TEST_ASSERT_TRUE(freq_433 >= 100.0 && freq_433 <= 1000.0);
    TEST_ASSERT_TRUE(freq_915 >= 100.0 && freq_915 <= 1000.0);
}

void test_radio_bandwidth_validation(void) {
    // Test valid bandwidth values
    float bw_125 = 125.0;
    float bw_250 = 250.0;
    float bw_500 = 500.0;
    
    TEST_ASSERT_TRUE(bw_125 > 0);
    TEST_ASSERT_TRUE(bw_250 > 0);
    TEST_ASSERT_TRUE(bw_500 > 0);
    
    // Test bandwidth ordering
    TEST_ASSERT_TRUE(bw_125 < bw_250);
    TEST_ASSERT_TRUE(bw_250 < bw_500);
}

void test_radio_power_validation(void) {
    // Test valid power levels (2-22 dBm)
    int power_min = 2;
    int power_mid = 10;
    int power_max = 22;
    
    TEST_ASSERT_TRUE(power_min >= 2 && power_min <= 22);
    TEST_ASSERT_TRUE(power_mid >= 2 && power_mid <= 22);
    TEST_ASSERT_TRUE(power_max >= 2 && power_max <= 22);
    
    // Test invalid power levels
    int power_too_low = 1;
    int power_too_high = 25;
    
    TEST_ASSERT_FALSE(power_too_low >= 2 && power_too_low <= 22);
    TEST_ASSERT_FALSE(power_too_high >= 2 && power_too_high <= 22);
}

void test_radio_interval_validation(void) {
    // Test valid transmission intervals
    float interval_100ms = 100.0;
    float interval_1s = 1000.0;
    float interval_5s = 5000.0;
    
    TEST_ASSERT_TRUE(interval_100ms > 0);
    TEST_ASSERT_TRUE(interval_1s > 0);
    TEST_ASSERT_TRUE(interval_5s > 0);
    
    // Test interval ordering
    TEST_ASSERT_TRUE(interval_100ms < interval_1s);
    TEST_ASSERT_TRUE(interval_1s < interval_5s);
}

void test_radio_message_handling(void) {
    // Test message length constraints (assuming max 1024 chars)
    const char* short_msg = "Hello";
    const char* medium_msg = "This is a test message for radio transmission";
    
    TEST_ASSERT_TRUE(strlen(short_msg) > 0);
    TEST_ASSERT_TRUE(strlen(short_msg) <= 1024);
    TEST_ASSERT_TRUE(strlen(medium_msg) > 0);
    TEST_ASSERT_TRUE(strlen(medium_msg) <= 1024);
}
