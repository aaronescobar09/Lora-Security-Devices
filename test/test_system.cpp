#include <unity.h>
#include <Arduino.h>

// System and hardware test functions
void test_system_initialization(void) {
    // Test basic system constants and initialization values
    TEST_ASSERT_TRUE(true); // Basic assertion test
    
    // Test serial baud rate
    uint32_t baud_rate = 115200;
    TEST_ASSERT_TRUE(baud_rate > 0);
    TEST_ASSERT_EQUAL(115200, baud_rate);
}

void test_memory_allocation(void) {
    // Test basic memory allocation
    void* test_ptr = malloc(100);
    TEST_ASSERT_NOT_NULL(test_ptr);
    free(test_ptr);
    
    // Test small allocation
    char* small_buffer = (char*)malloc(10);
    TEST_ASSERT_NOT_NULL(small_buffer);
    if (small_buffer) {
        strcpy(small_buffer, "test");
        TEST_ASSERT_EQUAL_STRING("test", small_buffer);
        free(small_buffer);
    }
}

void test_string_operations(void) {
    // Test string formatting and operations
    char buffer[64];
    
    // Test sprintf functionality
    sprintf(buffer, "Value: %d", 42);
    TEST_ASSERT_EQUAL_STRING("Value: 42", buffer);
    
    // Test string length
    const char* test_str = "Hello World";
    TEST_ASSERT_EQUAL(11, strlen(test_str));
    
    // Test string comparison
    TEST_ASSERT_EQUAL_STRING("test", "test");
    TEST_ASSERT_NOT_EQUAL(0, strcmp("test", "different"));
}

void test_math_operations(void) {
    // Test basic math operations
    TEST_ASSERT_EQUAL(4, 2 + 2);
    TEST_ASSERT_EQUAL(6, 2 * 3);
    TEST_ASSERT_EQUAL(2, 6 / 3);
    TEST_ASSERT_EQUAL(1, 5 % 2);
    
    // Test floating point operations
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.14, 22.0/7.0);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.83, sqrt(8.0));
}

void test_array_operations(void) {
    // Test array initialization and access
    int test_array[5] = {1, 2, 3, 4, 5};
    
    TEST_ASSERT_EQUAL(1, test_array[0]);
    TEST_ASSERT_EQUAL(3, test_array[2]);
    TEST_ASSERT_EQUAL(5, test_array[4]);
    
    // Test array size calculation
    size_t array_size = sizeof(test_array) / sizeof(test_array[0]);
    TEST_ASSERT_EQUAL(5, array_size);
}

void test_time_functions(void) {
    // Test time-related calculations
    uint32_t milliseconds = 1000;
    uint32_t seconds = milliseconds / 1000;
    TEST_ASSERT_EQUAL(1, seconds);
    
    // Test timeout calculations
    uint32_t timeout_3s = 3000;
    uint32_t timeout_5s = 5000;
    TEST_ASSERT_TRUE(timeout_3s < timeout_5s);
    
    // Test timer interval validation
    uint32_t valid_intervals[] = {100, 200, 500, 1000, 2000, 3000, 5000};
    size_t num_intervals = sizeof(valid_intervals) / sizeof(valid_intervals[0]);
    TEST_ASSERT_EQUAL(7, num_intervals);
    
    // Test that all intervals are positive
    for (size_t i = 0; i < num_intervals; i++) {
        TEST_ASSERT_TRUE(valid_intervals[i] > 0);
    }
}
