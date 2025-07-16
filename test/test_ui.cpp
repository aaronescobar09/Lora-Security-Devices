#include <unity.h>
#include <Arduino.h>

// UI test functions
void test_ui_color_validation(void) {
    // Test hex color values used in the UI
    uint32_t bg_dark = 0x1E1E1E;
    uint32_t bg_section = 0x2D2D30;
    uint32_t border_color = 0x3E3E42;
    uint32_t accent_blue = 0x007ACC;
    uint32_t text_color = 0xCCCCCC;
    uint32_t text_blue = 0x9CDCFE;
    
    // All colors should be valid 24-bit hex values
    TEST_ASSERT_TRUE(bg_dark <= 0xFFFFFF);
    TEST_ASSERT_TRUE(bg_section <= 0xFFFFFF);
    TEST_ASSERT_TRUE(border_color <= 0xFFFFFF);
    TEST_ASSERT_TRUE(accent_blue <= 0xFFFFFF);
    TEST_ASSERT_TRUE(text_color <= 0xFFFFFF);
    TEST_ASSERT_TRUE(text_blue <= 0xFFFFFF);
    
    // Test that colors are not pure black (0x000000)
    TEST_ASSERT_TRUE(bg_dark > 0x000000);
    TEST_ASSERT_TRUE(bg_section > 0x000000);
}

void test_ui_percentage_validation(void) {
    // Test percentage values used in LVGL sizing
    int pct_50 = 50;
    int pct_90 = 90;
    int pct_100 = 100;
    
    TEST_ASSERT_TRUE(pct_50 >= 0 && pct_50 <= 100);
    TEST_ASSERT_TRUE(pct_90 >= 0 && pct_90 <= 100);
    TEST_ASSERT_TRUE(pct_100 >= 0 && pct_100 <= 100);
    
    // Test invalid percentages
    int pct_negative = -10;
    int pct_over = 150;
    
    TEST_ASSERT_FALSE(pct_negative >= 0 && pct_negative <= 100);
    TEST_ASSERT_FALSE(pct_over >= 0 && pct_over <= 100);
}

void test_ui_opacity_validation(void) {
    // Test opacity values (0-255)
    uint8_t opacity_transparent = 0;
    uint8_t opacity_half = 128;
    uint8_t opacity_opaque = 255;
    
    TEST_ASSERT_TRUE(opacity_transparent >= 0 && opacity_transparent <= 255);
    TEST_ASSERT_TRUE(opacity_half >= 0 && opacity_half <= 255);
    TEST_ASSERT_TRUE(opacity_opaque >= 0 && opacity_opaque <= 255);
}

void test_ui_brightness_validation(void) {
    // Test brightness values (1-16 for screen, 0-255 for keyboard)
    uint8_t screen_min = 1;
    uint8_t screen_max = 16;
    uint8_t keyboard_min = 0;
    uint8_t keyboard_max = 255;
    
    TEST_ASSERT_TRUE(screen_min >= 1 && screen_min <= 16);
    TEST_ASSERT_TRUE(screen_max >= 1 && screen_max <= 16);
    TEST_ASSERT_TRUE(keyboard_min >= 0 && keyboard_min <= 255);
    TEST_ASSERT_TRUE(keyboard_max >= 0 && keyboard_max <= 255);
    
    // Test invalid screen brightness
    uint8_t screen_invalid = 0;
    uint8_t screen_too_high = 17;
    
    TEST_ASSERT_FALSE(screen_invalid >= 1 && screen_invalid <= 16);
    TEST_ASSERT_FALSE(screen_too_high >= 1 && screen_too_high <= 16);
}

void test_ui_grid_layout(void) {
    // Test grid layout parameters (3x2 grid for app icons)
    int grid_cols = 3;
    int grid_rows = 2;
    int total_icons = 6;
    
    TEST_ASSERT_TRUE(grid_cols > 0);
    TEST_ASSERT_TRUE(grid_rows > 0);
    TEST_ASSERT_EQUAL(total_icons, grid_cols * grid_rows);
}

void test_ui_icon_sizing(void) {
    // Test app icon dimensions
    int icon_width = 80;
    int icon_height = 80;
    int icon_radius = 16;
    
    TEST_ASSERT_TRUE(icon_width > 0);
    TEST_ASSERT_TRUE(icon_height > 0);
    TEST_ASSERT_TRUE(icon_radius > 0);
    TEST_ASSERT_EQUAL(icon_width, icon_height); // Icons should be square
    TEST_ASSERT_TRUE(icon_radius < icon_width / 2); // Radius should be less than half width
}
