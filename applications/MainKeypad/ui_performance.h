/**
 * @file      ui_performance.h
 * @author    Performance optimizations for T-Deck UI
 * @license   MIT
 * @copyright Copyright (c) 2024
 * @date      2024-07-15
 *
 */

#pragma once

#include <Arduino.h>
#include "lvgl.h"

// Performance optimization constants
#define UI_REFRESH_RATE_MS          16    // 60 FPS (1000/60 = 16.67ms)
#define UI_SLOW_REFRESH_RATE_MS     100   // For less critical updates (10 FPS)
#define UI_GPS_UPDATE_RATE_MS       500   // GPS updates every 500ms
#define UI_BATTERY_UPDATE_RATE_MS   5000  // Battery updates every 5s
#define UI_WIFI_UPDATE_RATE_MS      2000  // WiFi updates every 2s
#define UI_MEMORY_POOL_SIZE         8192  // 8KB memory pool for UI objects

// Memory optimization - pre-allocate common strings
extern const char* const LOADING_TEXT;
extern const char* const NOT_AVAILABLE_TEXT;
extern const char* const OFFLINE_TEXT;

// Color constants (pre-calculated for performance)
extern const lv_color_t UI_COLOR_BG_DARK;
extern const lv_color_t UI_COLOR_BG_SECTION;
extern const lv_color_t UI_COLOR_BORDER;
extern const lv_color_t UI_COLOR_ACCENT_BLUE;
extern const lv_color_t UI_COLOR_TEXT_PRIMARY;
extern const lv_color_t UI_COLOR_TEXT_SECONDARY;

// Performance optimized UI functions
void ui_performance_init(void);
void ui_reduce_animation_time(lv_obj_t* obj);
void ui_optimize_scrolling(lv_obj_t* obj);
void ui_set_fast_refresh_rate(void);
void ui_batch_style_updates_begin(void);
void ui_batch_style_updates_end(void);

// Memory pool for frequently allocated objects
void ui_memory_pool_init(void);
void* ui_fast_malloc(size_t size);
void ui_fast_free(void* ptr);

// Optimized update functions
void ui_update_gps_fast(double lat, double lng, double speed, const char* time_str);
void ui_update_battery_fast(uint16_t voltage_mv);
void ui_update_wifi_fast(int32_t rssi, bool connected);

// Style cache for performance
typedef struct {
    lv_style_t section_style;
    lv_style_t button_style;
    lv_style_t label_style;
    lv_style_t slider_style;
    lv_style_t switch_style;
    bool initialized;
} ui_style_cache_t;

extern ui_style_cache_t g_style_cache;
