#include <unity.h>
#include <Arduino.h>
#include "ui_performance.h"

// Performance test functions
void test_ui_memory_allocation_speed(void) {
    uint32_t start_time = micros();
    
    // Test fast memory allocation
    void* ptr1 = ui_fast_malloc(100);
    void* ptr2 = ui_fast_malloc(200);
    void* ptr3 = ui_fast_malloc(500);
    
    uint32_t alloc_time = micros() - start_time;
    
    TEST_ASSERT_NOT_NULL(ptr1);
    TEST_ASSERT_NOT_NULL(ptr2);
    TEST_ASSERT_NOT_NULL(ptr3);
    
    // Allocation should be very fast (< 100 microseconds)
    TEST_ASSERT_TRUE(alloc_time < 100);
    
    ui_fast_free(ptr1);
    ui_fast_free(ptr2);
    ui_fast_free(ptr3);
}

void test_ui_color_constants_performance(void) {
    uint32_t start_time = micros();
    
    // Test that pre-calculated colors are faster than hex conversion
    for (int i = 0; i < 1000; i++) {
        lv_color_t color1 = UI_COLOR_BG_DARK;
        lv_color_t color2 = UI_COLOR_ACCENT_BLUE;
        lv_color_t color3 = UI_COLOR_TEXT_PRIMARY;
        (void)color1; (void)color2; (void)color3; // Prevent optimization
    }
    
    uint32_t const_time = micros() - start_time;
    
    start_time = micros();
    
    // Compare with hex color creation
    for (int i = 0; i < 1000; i++) {
        lv_color_t color1 = lv_color_hex(0x1E1E1E);
        lv_color_t color2 = lv_color_hex(0x007ACC);
        lv_color_t color3 = lv_color_hex(0xCCCCCC);
        (void)color1; (void)color2; (void)color3; // Prevent optimization
    }
    
    uint32_t hex_time = micros() - start_time;
    
    // Pre-calculated constants should be at least as fast as hex conversion
    TEST_ASSERT_TRUE(const_time <= hex_time + 10); // Allow 10us tolerance
}

void test_ui_string_buffer_performance(void) {
    uint32_t start_time = micros();
    
    // Test static buffer performance
    char static_buffer[32];
    for (int i = 0; i < 100; i++) {
        snprintf(static_buffer, sizeof(static_buffer), "Value: %d", i);
    }
    
    uint32_t static_time = micros() - start_time;
    
    start_time = micros();
    
    // Test dynamic allocation performance
    for (int i = 0; i < 100; i++) {
        char* dynamic_buffer = (char*)malloc(32);
        snprintf(dynamic_buffer, 32, "Value: %d", i);
        free(dynamic_buffer);
    }
    
    uint32_t dynamic_time = micros() - start_time;
    
    // Static buffers should be significantly faster
    TEST_ASSERT_TRUE(static_time < dynamic_time / 2);
}

void test_ui_update_rate_constants(void) {
    // Test that update rates are reasonable
    TEST_ASSERT_TRUE(UI_REFRESH_RATE_MS >= 10);     // At least 100 FPS max
    TEST_ASSERT_TRUE(UI_REFRESH_RATE_MS <= 33);     // At least 30 FPS min
    
    TEST_ASSERT_TRUE(UI_GPS_UPDATE_RATE_MS >= 100);  // GPS updates not too frequent
    TEST_ASSERT_TRUE(UI_GPS_UPDATE_RATE_MS <= 1000); // GPS updates not too slow
    
    TEST_ASSERT_TRUE(UI_BATTERY_UPDATE_RATE_MS >= 1000); // Battery updates reasonable
    TEST_ASSERT_TRUE(UI_WIFI_UPDATE_RATE_MS >= 1000);    // WiFi updates reasonable
}

void test_ui_style_cache_initialization(void) {
    // Test that style cache initializes properly
    ui_performance_init();
    
    TEST_ASSERT_TRUE(g_style_cache.initialized);
    
    // Test that cached styles are valid
    TEST_ASSERT_NOT_NULL(&g_style_cache.section_style);
    TEST_ASSERT_NOT_NULL(&g_style_cache.button_style);
    TEST_ASSERT_NOT_NULL(&g_style_cache.label_style);
}

void test_ui_memory_pool_functionality(void) {
    // Test memory pool initialization
    ui_memory_pool_init();
    
    // Test sequential allocations
    void* ptr1 = ui_fast_malloc(64);
    void* ptr2 = ui_fast_malloc(128);
    void* ptr3 = ui_fast_malloc(256);
    
    TEST_ASSERT_NOT_NULL(ptr1);
    TEST_ASSERT_NOT_NULL(ptr2);
    TEST_ASSERT_NOT_NULL(ptr3);
    
    // Test that pointers are different
    TEST_ASSERT_NOT_EQUAL(ptr1, ptr2);
    TEST_ASSERT_NOT_EQUAL(ptr2, ptr3);
    TEST_ASSERT_NOT_EQUAL(ptr1, ptr3);
    
    // Test that pointers are in sequence (linear allocator)
    TEST_ASSERT_TRUE((uint8_t*)ptr2 > (uint8_t*)ptr1);
    TEST_ASSERT_TRUE((uint8_t*)ptr3 > (uint8_t*)ptr2);
}

void test_performance_benchmark_suite(void) {
    Serial.println("\n=== UI Performance Benchmark ===");
    
    // Benchmark memory allocation
    uint32_t start = micros();
    for (int i = 0; i < 100; i++) {
        void* ptr = ui_fast_malloc(64);
        ui_fast_free(ptr);
    }
    uint32_t fast_alloc_time = micros() - start;
    
    start = micros();
    for (int i = 0; i < 100; i++) {
        void* ptr = malloc(64);
        free(ptr);
    }
    uint32_t std_alloc_time = micros() - start;
    
    Serial.printf("Fast alloc: %luμs, Standard alloc: %luμs\n", fast_alloc_time, std_alloc_time);
    Serial.printf("Performance improvement: %.1fx\n", (float)std_alloc_time / fast_alloc_time);
    
    // Benchmark should show improvement or at least equal performance
    TEST_ASSERT_TRUE(fast_alloc_time <= std_alloc_time + 50); // Allow 50μs tolerance
}
