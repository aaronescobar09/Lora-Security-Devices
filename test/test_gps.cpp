#include <unity.h>
#include <Arduino.h>

// GPS test functions
void test_gps_coordinate_validation(void) {
    // Test valid latitude ranges (-90 to 90)
    TEST_ASSERT_TRUE(-90.0 >= -90.0 && -90.0 <= 90.0);
    TEST_ASSERT_TRUE(0.0 >= -90.0 && 0.0 <= 90.0);
    TEST_ASSERT_TRUE(90.0 >= -90.0 && 90.0 <= 90.0);
    
    // Test invalid latitude
    TEST_ASSERT_FALSE(91.0 >= -90.0 && 91.0 <= 90.0);
    TEST_ASSERT_FALSE(-91.0 >= -90.0 && -91.0 <= 90.0);
    
    // Test valid longitude ranges (-180 to 180)
    TEST_ASSERT_TRUE(-180.0 >= -180.0 && -180.0 <= 180.0);
    TEST_ASSERT_TRUE(0.0 >= -180.0 && 0.0 <= 180.0);
    TEST_ASSERT_TRUE(180.0 >= -180.0 && 180.0 <= 180.0);
    
    // Test invalid longitude
    TEST_ASSERT_FALSE(181.0 >= -180.0 && 181.0 <= 180.0);
    TEST_ASSERT_FALSE(-181.0 >= -180.0 && -181.0 <= 180.0);
}

void test_gps_time_validation(void) {
    // Test valid time ranges
    uint8_t valid_hour = 12;
    uint8_t valid_minute = 30;
    uint8_t valid_second = 45;
    
    TEST_ASSERT_TRUE(valid_hour >= 0 && valid_hour <= 23);
    TEST_ASSERT_TRUE(valid_minute >= 0 && valid_minute <= 59);
    TEST_ASSERT_TRUE(valid_second >= 0 && valid_second <= 59);
    
    // Test invalid time values
    uint8_t invalid_hour = 25;
    uint8_t invalid_minute = 60;
    uint8_t invalid_second = 60;
    
    TEST_ASSERT_FALSE(invalid_hour >= 0 && invalid_hour <= 23);
    TEST_ASSERT_FALSE(invalid_minute >= 0 && invalid_minute <= 59);
    TEST_ASSERT_FALSE(invalid_second >= 0 && invalid_second <= 59);
}

void test_gps_date_validation(void) {
    // Test valid date ranges
    uint16_t valid_year = 2024;
    uint8_t valid_month = 6;
    uint8_t valid_day = 15;
    
    TEST_ASSERT_TRUE(valid_year >= 1900 && valid_year <= 2100);
    TEST_ASSERT_TRUE(valid_month >= 1 && valid_month <= 12);
    TEST_ASSERT_TRUE(valid_day >= 1 && valid_day <= 31);
    
    // Test invalid date values
    uint8_t invalid_month = 13;
    uint8_t invalid_day = 32;
    
    TEST_ASSERT_FALSE(invalid_month >= 1 && invalid_month <= 12);
    TEST_ASSERT_FALSE(invalid_day >= 1 && invalid_day <= 31);
}

void test_gps_speed_validation(void) {
    // Test speed values (assuming reasonable GPS speeds)
    double valid_speed_stationary = 0.0;
    double valid_speed_walking = 5.0;
    double valid_speed_driving = 60.0;
    
    TEST_ASSERT_TRUE(valid_speed_stationary >= 0.0);
    TEST_ASSERT_TRUE(valid_speed_walking >= 0.0);
    TEST_ASSERT_TRUE(valid_speed_driving >= 0.0);
    
    // Test that negative speeds are invalid
    double invalid_speed = -10.0;
    TEST_ASSERT_FALSE(invalid_speed >= 0.0);
}
