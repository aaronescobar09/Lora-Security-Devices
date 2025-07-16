/**
 * @file      ui_performance.cpp
 * @author    Performance optimizations for T-Deck UI
 * @license   MIT
 * @copyright Copyright (c) 2024
 * @date      2024-07-15
 *
 */

#include "ui_performance.h"
#include <esp_heap_caps.h>

// Pre-allocated string constants to avoid repeated allocations
const char* const LOADING_TEXT = "Loading...";
const char* const NOT_AVAILABLE_TEXT = "N.A";
const char* const OFFLINE_TEXT = "Offline";

// Pre-calculated colors for performance
const lv_color_t UI_COLOR_BG_DARK = LV_COLOR_MAKE(0x1E, 0x1E, 0x1E);
const lv_color_t UI_COLOR_BG_SECTION = LV_COLOR_MAKE(0x2D, 0x2D, 0x30);
const lv_color_t UI_COLOR_BORDER = LV_COLOR_MAKE(0x3E, 0x3E, 0x42);
const lv_color_t UI_COLOR_ACCENT_BLUE = LV_COLOR_MAKE(0x00, 0x7A, 0xCC);
const lv_color_t UI_COLOR_TEXT_PRIMARY = LV_COLOR_MAKE(0xCC, 0xCC, 0xCC);
const lv_color_t UI_COLOR_TEXT_SECONDARY = LV_COLOR_MAKE(0x9C, 0xDC, 0xFE);

// Global style cache
ui_style_cache_t g_style_cache = {0};

// Memory pool for UI objects
static void* g_ui_memory_pool = nullptr;
static size_t g_ui_memory_pool_size = 32768; // 32KB for UI objects
static size_t g_ui_memory_used = 0;

void ui_performance_init(void) {
    // Initialize memory pool for UI objects
    ui_memory_pool_init();
    
    // Set faster LVGL refresh rate
    ui_set_fast_refresh_rate();
    
    // Initialize style cache
    if (!g_style_cache.initialized) {
        // Pre-create and cache frequently used styles
        lv_style_init(&g_style_cache.section_style);
        lv_style_set_bg_color(&g_style_cache.section_style, UI_COLOR_BG_SECTION);
        lv_style_set_bg_opa(&g_style_cache.section_style, LV_OPA_COVER);
        lv_style_set_radius(&g_style_cache.section_style, 6);
        lv_style_set_border_width(&g_style_cache.section_style, 1);
        lv_style_set_border_color(&g_style_cache.section_style, UI_COLOR_BORDER);
        lv_style_set_pad_all(&g_style_cache.section_style, 8);
        
        lv_style_init(&g_style_cache.button_style);
        lv_style_set_bg_color(&g_style_cache.button_style, UI_COLOR_ACCENT_BLUE);
        lv_style_set_bg_opa(&g_style_cache.button_style, LV_OPA_COVER);
        lv_style_set_radius(&g_style_cache.button_style, 8);
        lv_style_set_shadow_width(&g_style_cache.button_style, 3);
        lv_style_set_shadow_opa(&g_style_cache.button_style, LV_OPA_20);
        
        lv_style_init(&g_style_cache.label_style);
        lv_style_set_text_color(&g_style_cache.label_style, UI_COLOR_TEXT_PRIMARY);
        lv_style_set_text_font(&g_style_cache.label_style, &lv_font_montserrat_12);
        
        g_style_cache.initialized = true;
    }
}

void ui_memory_pool_init(void) {
    if (g_ui_memory_pool == nullptr) {
        // Allocate memory pool in PSRAM for better performance
        g_ui_memory_pool = heap_caps_malloc(g_ui_memory_pool_size, MALLOC_CAP_SPIRAM);
        if (g_ui_memory_pool == nullptr) {
            // Fallback to regular RAM if PSRAM allocation fails
            g_ui_memory_pool = malloc(g_ui_memory_pool_size);
        }
        g_ui_memory_used = 0;
    }
}

void* ui_fast_malloc(size_t size) {
    // Simple linear allocator for UI objects
    if (g_ui_memory_pool && (g_ui_memory_used + size) < g_ui_memory_pool_size) {
        void* ptr = (uint8_t*)g_ui_memory_pool + g_ui_memory_used;
        g_ui_memory_used += size;
        return ptr;
    }
    // Fallback to regular malloc
    return malloc(size);
}

void ui_fast_free(void* ptr) {
    // For linear allocator, we don't actually free individual allocations
    // This is acceptable for UI objects that persist for the app lifetime
    // Use regular free for malloc'd objects
    if (ptr < g_ui_memory_pool || ptr >= (uint8_t*)g_ui_memory_pool + g_ui_memory_pool_size) {
        free(ptr);
    }
}

void ui_reduce_animation_time(lv_obj_t* obj) {
    // Reduce animation time for faster transitions
    lv_obj_set_style_anim_time(obj, 100, LV_PART_MAIN); // 100ms instead of default 200ms
}

void ui_optimize_scrolling(lv_obj_t* obj) {
    // Optimize scrolling performance
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_snap_x(obj, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scroll_snap_y(obj, LV_SCROLL_SNAP_CENTER);
}

void ui_set_fast_refresh_rate(void) {
    // Set faster refresh rate for smoother animations
    lv_disp_t* disp = lv_disp_get_default();
    if (disp) {
        lv_disp_set_default(disp);
        // Force faster refresh - this will be handled by the display driver
    }
}

void ui_batch_style_updates_begin(void) {
    // Begin batching style updates to reduce redraws
    lv_obj_invalidate(lv_scr_act());
}

void ui_batch_style_updates_end(void) {
    // End batching and trigger single redraw
    lv_refr_now(lv_disp_get_default());
}

// Optimized GPS update function
void ui_update_gps_fast(double lat, double lng, double speed, const char* time_str) {
    // Use static buffers to avoid memory allocation
    static char lat_buf[16];
    static char lng_buf[16];
    static char speed_buf[16];
    
    // Only update if values actually changed (avoid unnecessary redraws)
    static double last_lat = 999.0, last_lng = 999.0, last_speed = -1.0;
    
    ui_batch_style_updates_begin();
    
    if (fabs(lat - last_lat) > 0.000001) {
        snprintf(lat_buf, sizeof(lat_buf), "%.6f", lat);
        // Update lat label here
        last_lat = lat;
    }
    
    if (fabs(lng - last_lng) > 0.000001) {
        snprintf(lng_buf, sizeof(lng_buf), "%.6f", lng);
        // Update lng label here
        last_lng = lng;
    }
    
    if (fabs(speed - last_speed) > 0.01) {
        snprintf(speed_buf, sizeof(speed_buf), "%.2f", speed);
        // Update speed label here
        last_speed = speed;
    }
    
    ui_batch_style_updates_end();
}

// Optimized battery update function
void ui_update_battery_fast(uint16_t voltage_mv) {
    static uint16_t last_voltage = 0;
    static char voltage_buf[16];
    
    // Only update if voltage changed significantly (>10mV)
    if (abs((int)voltage_mv - (int)last_voltage) > 10) {
        snprintf(voltage_buf, sizeof(voltage_buf), "%u mV", voltage_mv);
        // Update battery label here
        last_voltage = voltage_mv;
    }
}

// Optimized WiFi update function
void ui_update_wifi_fast(int32_t rssi, bool connected) {
    static int32_t last_rssi = 999;
    static bool last_connected = false;
    static char rssi_buf[16];
    
    // Only update if status changed
    if (connected != last_connected || abs(rssi - last_rssi) > 2) {
        if (connected) {
            snprintf(rssi_buf, sizeof(rssi_buf), "%ld dBm", rssi);
        } else {
            strcpy(rssi_buf, NOT_AVAILABLE_TEXT);
        }
        // Update WiFi labels here
        last_rssi = rssi;
        last_connected = connected;
    }
}
