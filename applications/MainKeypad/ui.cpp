/**
 * @file      ui.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2024-09-12
 *
 */

#include <Arduino.h>
#include <WiFi.h>
#include <SD.h>
#include "lvgl.h"
#include "utilities.h"
#include "ui_performance.h"  // Performance optimizations - temporarily disabled

#include <vector>
#include "config.h"

// Performance optimization: Use faster vector with reserved capacity
std::vector<lv_obj_t *> sub_section;

// Pre-allocate string buffers to avoid dynamic allocation
static char gps_lat_buffer[16];
static char gps_lng_buffer[16]; 
static char gps_speed_buffer[16];
static char gps_date_buffer[16];
static char gps_time_buffer[16];
static char battery_buffer[16];
static char wifi_rssi_buffer[16];

LV_IMG_DECLARE(system_background);

// Performance optimization: Struct packing and better memory layout
typedef struct __attribute__((packed)) {
    lv_obj_t *label_lat;
    lv_obj_t *label_lng;
    lv_obj_t *label_date;
    lv_obj_t *label_time;
    lv_obj_t *label_speed;
    lv_obj_t *label_processchar;
    lv_obj_t *label_use_seconds;
} Deck_GPS_t;

typedef struct __attribute__((packed)) {
    lv_obj_t *label_radio_state;
    lv_obj_t *label_radio_msg;
} Deck_Radio_t;

// Global instances
static Deck_GPS_t sub_gps_val;
static Deck_Radio_t sub_radio_val;
static lv_obj_t *sound_vad_label;

// Forward declarations for pages
static lv_obj_t *sub_mechanics_page;
static lv_obj_t *sub_sound_page;
static lv_obj_t *sub_display_page;
static lv_obj_t *sub_gps_page;
static lv_obj_t *sub_kb_page;
static lv_obj_t *sub_setting_page;
lv_obj_t *home_screen;

static void back_event_handler(lv_event_t *e);
static void switch_handler(lv_event_t *e);
static void app_icon_cb(lv_event_t *e);
static void back_to_home_cb(lv_event_t *e);

// Touch isolation functions (defined in UnitTest.ino)
extern void isolate_touch_input(uint32_t screen_id);
extern bool is_touch_isolated();

lv_obj_t *root_page;
static lv_obj_t *global_menu;
static lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt);
static lv_obj_t *create_slider(lv_obj_t *parent,
                               const char *icon, const char *txt, int32_t min, int32_t max,
                               int32_t val, lv_event_cb_t cb, lv_event_code_t filter);

static lv_obj_t *create_button(lv_obj_t *parent, const char *icon, const char *txt, lv_event_cb_t cb);

static lv_obj_t *create_switch(lv_obj_t *parent,
                               const char *icon, const char *txt, bool chk, lv_event_cb_t cb);
static lv_obj_t *create_label(lv_obj_t *parent, const char *icon, const char *txt, const char *default_text);
static lv_obj_t *create_dropdown(lv_obj_t *parent, const char *icon, const char *txt, const char *options, uint8_t default_sel, lv_event_cb_t cb);
static lv_obj_t *create_app_icon(lv_obj_t *parent, const char *icon, const char *label, lv_obj_t *target_page, lv_event_cb_t cb);

// iPhone-style section header
static lv_obj_t *create_section_header(lv_obj_t *parent, const char *title)
{
    lv_obj_t *header = lv_obj_create(parent);
    lv_obj_set_size(header, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(header, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_top(header, 12, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(header, 4, LV_PART_MAIN);
    
    // Make header sticky/fixed - disable scrolling for this object
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    
    lv_obj_t *title_label = lv_label_create(header);
    lv_label_set_text(title_label, title);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_align(title_label, LV_ALIGN_LEFT_MID, 0, 0);
    
    return header;
}

// Comprehensive scroll optimization function for smooth scrolling
static void optimize_page_scrolling(lv_obj_t *page)
{
    if (!page) return;
    
    // Enable vertical scrolling with momentum
    lv_obj_set_scroll_dir(page, LV_DIR_VER);
    
    // Optimize scrollbar appearance - auto mode for better UX
    lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_AUTO);
    
    // Scroll snap - smooth settling to content boundaries
    lv_obj_set_scroll_snap_y(page, LV_SCROLL_SNAP_NONE); // Smooth free scroll
    
    // Ensure scrollable flag is set
    lv_obj_add_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    
    // Optimize flex layout for smooth scrolling performance
    lv_obj_set_style_pad_row(page, 2, LV_PART_MAIN); // Reduce row padding for smoother scroll
    lv_obj_set_style_pad_column(page, 0, LV_PART_MAIN);
    
    // Enable scroll momentum and smooth animations through basic optimizations
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLL_ONE); // Allow free scrolling
}

// Performance-optimized section group creation using cached styles
static lv_obj_t *create_section_group(lv_obj_t *parent)
{
    lv_obj_t *group = lv_obj_create(parent);
    lv_obj_set_size(group, LV_PCT(90), LV_SIZE_CONTENT);
    
    // Use cached styles for better performance
    // if (g_style_cache.initialized) {
    //     lv_obj_add_style(group, &g_style_cache.section_style, LV_PART_MAIN);
    // } else {
        // Fallback to manual styling if cache not initialized
        lv_obj_set_style_bg_color(group, lv_color_hex(0x2D2D30), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(group, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_radius(group, 6, LV_PART_MAIN);
        lv_obj_set_style_border_width(group, 1, LV_PART_MAIN);
        lv_obj_set_style_border_color(group, lv_color_hex(0x3E3E42), LV_PART_MAIN);
        lv_obj_set_style_pad_all(group, 8, LV_PART_MAIN);
        lv_obj_set_style_pad_top(group, 10, LV_PART_MAIN);
        lv_obj_set_style_pad_bottom(group, 10, LV_PART_MAIN);
    // }
    
    // Optimize animation performance
    // ui_reduce_animation_time(group);
    
    return group;
}

extern void setBrightness(uint8_t value);
extern void setTx();
extern void setRx();
extern void setFreq(float f);
extern void soundPlay();
extern void setBandWidth(float bw);
extern void setTxPower(int16_t dBm);
extern void setSenderInterval(uint32_t interval_ms);
extern void setKeyboardBrightness(uint8_t level);
extern bool        enterSleep;

extern String gps_model;


void lv_brightness_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    uint8_t val =  lv_slider_get_value(obj);
    setBrightness(val);
}

void lv_kb_brightness_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    uint8_t val =  lv_slider_get_value(obj);
    setKeyboardBrightness(val);
}

void lv_background_opa_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    uint8_t val =  lv_slider_get_value(obj);
    std::vector<lv_obj_t *>::iterator it;
    for (it = sub_section.begin(); it != sub_section.end(); it++) {
        lv_obj_set_style_bg_opa(*it, val, LV_PART_MAIN);
    }
}

void lv_radio_tx_event_cb(lv_event_t *e)
{
    Serial.println("set TX");
    setTx();
    lv_label_set_text(sub_radio_val.label_radio_msg, "RF Tx Starting");
    lv_obj_t *obj =  (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t *swRx = (lv_obj_t *)lv_obj_get_user_data(obj);
    lv_obj_clear_state(swRx, LV_STATE_CHECKED);
}

void lv_radio_rx_event_cb(lv_event_t *e)
{
    Serial.println("set RX");
    setRx();
    lv_label_set_text(sub_radio_val.label_radio_msg, "RF monitoring");
    lv_obj_t *obj =  (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t *swTx = (lv_obj_t *)lv_obj_get_user_data(obj);
    lv_obj_clear_state(swTx, LV_STATE_CHECKED);
}

void speaker_play_event(lv_event_t *e)
{
    soundPlay();
}

void sleep_event_cb(lv_event_t *e)
{
    enterSleep = true;
}

void setLoRaMessage(const char *text)
{
    lv_label_set_text(sub_radio_val.label_radio_msg, text);
}

// Performance-optimized GPS update function
void updateGPS(double lat, double lng,
               uint16_t year, uint8_t month, uint8_t day,
               uint8_t hour, uint8_t minute, uint8_t second,
               double speed, uint32_t rx_char, uint32_t use_sec)
{
    // Static variables to avoid unnecessary updates
    static double last_lat = 999.0, last_lng = 999.0, last_speed = -1.0;
    static uint16_t last_year = 0;
    static uint8_t last_month = 0, last_day = 0, last_hour = 25, last_minute = 61, last_second = 61;
    static uint32_t last_rx_char = 0, last_use_sec = 0;
    
    bool needs_update = false;
    
    // Batch updates for better performance
    ui_batch_style_updates_begin();
    
    // Only update use_seconds if changed
    if (use_sec != last_use_sec) {
        if (use_sec == 0) {
            lv_label_set_text(sub_gps_val.label_use_seconds, "N.A");
        } else {
            lv_label_set_text_fmt(sub_gps_val.label_use_seconds, "%u", use_sec);
        }
        last_use_sec = use_sec;
        needs_update = true;
    }
    
    // Only update coordinates if changed significantly (> 0.000001 degrees)
    if (fabs(lng - last_lng) > 0.000001) {
        lv_label_set_text_fmt(sub_gps_val.label_lng, "%.6f", lng);
        last_lng = lng;
        needs_update = true;
    }
    
    if (fabs(lat - last_lat) > 0.000001) {
        lv_label_set_text_fmt(sub_gps_val.label_lat, "%.6f", lat);
        last_lat = lat;
        needs_update = true;
    }
    
    // Only update date if changed
    if (year != last_year || month != last_month || day != last_day) {
        lv_label_set_text_fmt(sub_gps_val.label_date, "%u/%u/%u", year, month, day);
        last_year = year;
        last_month = month;
        last_day = day;
        needs_update = true;
    }
    
    // Only update time if changed
    if (hour != last_hour || minute != last_minute || second != last_second) {
        lv_label_set_text_fmt(sub_gps_val.label_time, "%u:%02u:%02u", hour, minute, second);
        last_hour = hour;
        last_minute = minute;
        last_second = second;
        needs_update = true;
    }
    
    // Only update speed if changed significantly (> 0.01)
    if (fabs(speed - last_speed) > 0.01) {
        lv_label_set_text_fmt(sub_gps_val.label_speed, "%.2f", speed);
        last_speed = speed;
        needs_update = true;
    }
    
    // Only update rx_char if changed
    if (rx_char != last_rx_char) {
        lv_label_set_text_fmt(sub_gps_val.label_processchar, "%u", rx_char);
        last_rx_char = rx_char;
        needs_update = true;
    }
    
    // Only trigger redraw if something actually changed
    if (needs_update) {
        ui_batch_style_updates_end();
    }
}

void updateNoiseLabel(uint32_t cnt)
{
    if (sound_vad_label) {
        lv_label_set_text_fmt(sound_vad_label, "%u", cnt);
    }
}

const char *radio_freq_list =
#ifdef  JAPAN_MIC
    "920MHZ";
#else
    "433MHZ\n"
    "470MHZ\n"
    "850MHZ\n"
    "868MHZ\n"
    "915MHZ\n"
    "923MHZ";
#endif
const float freq_list[] = {
#ifdef  JAPAN_MIC
    920.0
#else
    433.0, 470.0, 850.0, 868.0, 915.0, 923.0
#endif
};

const char *radio_bandwidth_list =
    "125KHz\n"
    "250KHz\n"
    "500KHz";
const float bandwidth_list[] = {125.0, 250.0, 500.0};


const char *radio_power_level_list =
    "2dBm\n"
    "5dBm\n"
    "10dBm\n"
    "12dBm\n"
    "17dBm\n"
    "20dBm\n"
    "22dBm";
const float radio_power_args_list[] = {2, 5, 10, 12, 17, 20, 22};

const char   *radio_tx_interval_list =
    "100ms\n"
    "200ms\n"
    "500ms\n"
    "1000ms\n"
    "2000ms\n"
    "3000ms\n"
    "5000ms";
const float radio_tx_interval_args_list[] = {100, 200, 500, 1000, 2000, 3000, 5000};

static void radio_freq_cb(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    uint32_t index = lv_dropdown_get_selected(obj);
    Serial.printf("Option: %s id:%u\n", buf, index);
    setFreq(freq_list[index]);
}


static void radio_power_cb(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    uint32_t index = lv_dropdown_get_selected(obj);
    Serial.printf("Option: %s id:%u\n", buf, index);
    setTxPower(radio_power_args_list[index]);

}

static void radio_bandwidth_cb(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    uint32_t index = lv_dropdown_get_selected(obj);
    Serial.printf("Option: %s id:%u\n", buf, index);
    setBandWidth(bandwidth_list[index]);
}

static void radio_interval_cb(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    char buf[32];
    lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
    uint32_t index = lv_dropdown_get_selected(obj);
    Serial.printf("Option: %s id:%u\n", buf, index);
    setSenderInterval(radio_tx_interval_args_list[index]);
}


LV_IMG_DECLARE(system_background);
extern bool  hasRadio;

// Global variable for icon transparency control
static uint8_t icon_transparency = 180; // Default semi-transparent (70% opacity)
static std::vector<lv_obj_t *> app_icons; // Store all app icons for transparency updates

// Callback to update icon transparency
void lv_icon_transparency_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    uint8_t val = lv_slider_get_value(obj);
    icon_transparency = val;
    
    // Update transparency for all app icons
    for (auto icon : app_icons) {
        if (icon) {
            lv_obj_set_style_bg_opa(icon, val, LV_PART_MAIN);
            lv_obj_set_style_border_opa(icon, val, LV_PART_MAIN);
        }
    }
}

// Function to create icon button with navigation
static lv_obj_t *create_app_icon(lv_obj_t *parent, const char *icon, const char *label, lv_obj_t *target_page, lv_event_cb_t cb)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 80, 80);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x2D2D30), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn, icon_transparency, LV_PART_MAIN); // Use adjustable transparency
    lv_obj_set_style_radius(btn, 16, LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x3E3E42), LV_PART_MAIN);
    lv_obj_set_style_border_opa(btn, icon_transparency, LV_PART_MAIN); // Make border transparent too
    lv_obj_set_style_shadow_width(btn, 4, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(btn, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_30, LV_PART_MAIN); // Slightly more visible shadow for depth
    lv_obj_set_style_shadow_ofs_y(btn, 2, LV_PART_MAIN);
    
    // Pressed state - more opaque when pressed
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x007ACC), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_PRESSED); // Full opacity when pressed
    lv_obj_set_style_transform_zoom(btn, 240, LV_STATE_PRESSED);
    
    // Icon
    lv_obj_t *icon_label = lv_label_create(btn);
    lv_label_set_text(icon_label, icon);
    lv_obj_set_style_text_color(icon_label, lv_color_hex(0x007ACC), LV_PART_MAIN);
    lv_obj_set_style_text_font(icon_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_align(icon_label, LV_ALIGN_CENTER, 0, -10);
    
    // Label
    lv_obj_t *text_label = lv_label_create(btn);
    lv_label_set_text(text_label, label);
    lv_obj_set_style_text_color(text_label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_font(text_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_align(text_label, LV_ALIGN_CENTER, 0, 15);
    
    // Store target page in user data
    lv_obj_set_user_data(btn, target_page);
    
    if (cb) {
        lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
    }
    
    // Store icon in vector for transparency updates
    app_icons.push_back(btn);
    
    return btn;
}

// Navigation callback for app icons with debouncing
static void app_icon_cb(lv_event_t *e)
{
    static uint32_t last_click_time = 0;
    uint32_t current_time = lv_tick_get();
    
    // Debounce: ignore clicks within 300ms of the last click
    if (current_time - last_click_time < 300) {
        return;
    }
    last_click_time = current_time;
    
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *target_page = (lv_obj_t *)lv_obj_get_user_data(btn);
    
    if (target_page) {
        // Isolate touch input during screen transition to prevent cross-menu button activation
        uint32_t screen_id = (uint32_t)target_page; // Use page pointer as unique screen ID
        isolate_touch_input(screen_id);
        
        // Start display update batching for better performance
        lv_disp_t *disp = lv_disp_get_default();
        if (disp) {
            // Pause screen refreshing during batch updates
            lv_disp_enable_invalidation(disp, false);
        }
        
        // Hide home screen
        lv_obj_add_flag(home_screen, LV_OBJ_FLAG_HIDDEN);
        
        // Hide all pages first
        lv_obj_add_flag(sub_mechanics_page, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(sub_sound_page, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(sub_display_page, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(sub_gps_page, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(sub_kb_page, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(sub_setting_page, LV_OBJ_FLAG_HIDDEN);
        
        // Show the target page
        lv_obj_clear_flag(target_page, LV_OBJ_FLAG_HIDDEN);
        
        // Re-enable screen refreshing and force immediate update
        if (disp) {
            lv_disp_enable_invalidation(disp, true);
            lv_obj_invalidate(lv_scr_act()); // Force full screen redraw
        }
        
        // Force immediate LVGL processing to ensure responsive UI
        lv_task_handler();
    }
}

// Back to home callback
static void back_to_home_cb(lv_event_t *e)
{
    // Isolate touch input during screen transition back to home
    isolate_touch_input(0); // Use 0 as home screen ID
    
    // Hide all pages
    lv_obj_add_flag(sub_mechanics_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sub_sound_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sub_display_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sub_gps_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sub_kb_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sub_setting_page, LV_OBJ_FLAG_HIDDEN);
    
    // Show home screen
    lv_obj_clear_flag(home_screen, LV_OBJ_FLAG_HIDDEN);
}

void setupUI(void)
{
    // Initialize performance optimizations first
    ui_performance_init();
    
    // Reserve capacity for sub_section vector to avoid reallocations
    sub_section.reserve(32);  // Estimate of total sections needed
    
    // Pre-allocate string buffers
    memset(gps_lat_buffer, 0, sizeof(gps_lat_buffer));
    memset(gps_lng_buffer, 0, sizeof(gps_lng_buffer));
    memset(gps_speed_buffer, 0, sizeof(gps_speed_buffer));
    memset(gps_date_buffer, 0, sizeof(gps_date_buffer));
    memset(gps_time_buffer, 0, sizeof(gps_time_buffer));
    memset(battery_buffer, 0, sizeof(battery_buffer));
    memset(wifi_rssi_buffer, 0, sizeof(wifi_rssi_buffer));
    
    // Batch all UI creation for better performance
    ui_batch_style_updates_begin();
    // Create a simple full-screen container instead of menu
    home_screen = lv_obj_create(lv_scr_act());
    lv_obj_set_size(home_screen, LV_PCT(100), LV_PCT(100));
    
    // Use system background image instead of solid color
    lv_obj_set_style_bg_img_src(home_screen, &system_background, LV_PART_MAIN);
    lv_obj_set_style_bg_img_opa(home_screen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_img_tiled(home_screen, false, LV_PART_MAIN);
    
    lv_obj_set_style_border_width(home_screen, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(home_screen, 0, LV_PART_MAIN);
    lv_obj_center(home_screen);
    
    // Store main screen reference instead of menu
    global_menu = home_screen;

    lv_obj_t *cont;
    lv_obj_t *section;

  
    /*Create sub pages as simple containers*/
    // !RADIO
    sub_mechanics_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sub_mechanics_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(sub_mechanics_page, 10, 0);
    lv_obj_set_style_bg_color(sub_mechanics_page, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
    lv_obj_set_style_border_width(sub_mechanics_page, 0, LV_PART_MAIN);
    lv_obj_add_flag(sub_mechanics_page, LV_OBJ_FLAG_HIDDEN); // Hide initially
    lv_obj_set_flex_flow(sub_mechanics_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sub_mechanics_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Apply comprehensive scroll optimizations for smooth scrolling
    optimize_page_scrolling(sub_mechanics_page);
    
    // Add back button at the top
    lv_obj_t *radio_back_btn = lv_btn_create(sub_mechanics_page);
    lv_obj_set_size(radio_back_btn, LV_SIZE_CONTENT, 40);
    lv_obj_set_style_bg_color(radio_back_btn, lv_color_hex(0x007ACC), LV_PART_MAIN);
    lv_obj_set_style_radius(radio_back_btn, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(radio_back_btn, back_to_home_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *radio_back_label = lv_label_create(radio_back_btn);
    lv_label_set_text(radio_back_label, LV_SYMBOL_LEFT " Back");
    lv_obj_set_style_text_color(radio_back_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(radio_back_label);
    
    // Radio - Each control gets its own individual section/box
    if (hasRadio) {
        lv_obj_t *tx_section = create_section_group(sub_mechanics_page);
        sub_section.push_back(tx_section);
        lv_obj_t *swTx = create_switch(tx_section, LV_SYMBOL_UP, "Tx", true, lv_radio_tx_event_cb);

        lv_obj_t *rx_section = create_section_group(sub_mechanics_page);
        sub_section.push_back(rx_section);
        lv_obj_t *swRx = create_switch(rx_section, LV_SYMBOL_DOWN, "Rx", false, lv_radio_rx_event_cb);

        lv_obj_set_user_data(swTx, swRx);
        lv_obj_set_user_data(swRx, swTx);

        lv_obj_t *message_section = create_section_group(sub_mechanics_page);
        sub_section.push_back(message_section);
        create_label(message_section, LV_SYMBOL_LOOP, "Message", NULL);
        sub_radio_val.label_radio_msg = create_label(message_section, NULL, NULL, "N.A");

#ifdef  JAPAN_MIC
        uint8_t freq_index = 0;
#else
        uint8_t freq_index = 2;
#endif
        lv_obj_t *freq_section = create_section_group(sub_mechanics_page);
        sub_section.push_back(freq_section);
        create_dropdown(freq_section, NULL, "Freq", radio_freq_list, freq_index, radio_freq_cb);

        lv_obj_t *bandwidth_section = create_section_group(sub_mechanics_page);
        sub_section.push_back(bandwidth_section);
        create_dropdown(bandwidth_section, NULL, "BandWidth", radio_bandwidth_list, 0, radio_bandwidth_cb);

        lv_obj_t *power_section = create_section_group(sub_mechanics_page);
        sub_section.push_back(power_section);
        create_dropdown(power_section, NULL, "TxPower", radio_power_level_list, 6, radio_power_cb);

        lv_obj_t *interval_section = create_section_group(sub_mechanics_page);
        sub_section.push_back(interval_section);
        create_dropdown(interval_section, NULL, "Interval", radio_tx_interval_list, 3, radio_interval_cb);

    } else {
        lv_obj_t *offline_section = create_section_group(sub_mechanics_page);
        sub_section.push_back(offline_section);
        lv_obj_t *label = lv_label_create(offline_section);
        lv_label_set_text(label, "Radio is offline");
        lv_obj_set_style_text_color(label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
        lv_obj_center(label);
    }


    // !SOUND
    sub_sound_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sub_sound_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(sub_sound_page, 10, 0);
    lv_obj_set_style_bg_color(sub_sound_page, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
    lv_obj_set_style_border_width(sub_sound_page, 0, LV_PART_MAIN);
    lv_obj_add_flag(sub_sound_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_flex_flow(sub_sound_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sub_sound_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Apply comprehensive scroll optimizations for smooth scrolling
    optimize_page_scrolling(sub_sound_page);

    // Add back button
    lv_obj_t *sound_back_btn = lv_btn_create(sub_sound_page);
    lv_obj_set_size(sound_back_btn, LV_SIZE_CONTENT, 40);
    lv_obj_set_style_bg_color(sound_back_btn, lv_color_hex(0x007ACC), LV_PART_MAIN);
    lv_obj_set_style_radius(sound_back_btn, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(sound_back_btn, back_to_home_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *sound_back_label = lv_label_create(sound_back_btn);
    lv_label_set_text(sound_back_label, LV_SYMBOL_LEFT " Back");
    lv_obj_set_style_text_color(sound_back_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(sound_back_label);

    // Sound - Each control gets its own individual section/box
    create_section_header(sub_sound_page, "AUDIO OUTPUT");
    lv_obj_t *speaker_section = create_section_group(sub_sound_page);
    sub_section.push_back(speaker_section);
    create_button(speaker_section, LV_SYMBOL_AUDIO, "Test Speaker", speaker_play_event);

    create_section_header(sub_sound_page, "MICROPHONE");
    lv_obj_t *mic_section = create_section_group(sub_sound_page);
    sub_section.push_back(mic_section);
    sound_vad_label = create_label(mic_section, LV_SYMBOL_VOLUME_MAX, "Voice Activity", "N.A");


    // !DISPLAY
    sub_display_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sub_display_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(sub_display_page, 10, 0);
    lv_obj_set_style_bg_color(sub_display_page, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
    lv_obj_set_style_border_width(sub_display_page, 0, LV_PART_MAIN);
    lv_obj_add_flag(sub_display_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_flex_flow(sub_display_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sub_display_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Apply comprehensive scroll optimizations for smooth scrolling
    optimize_page_scrolling(sub_display_page);

    // Add back button
    lv_obj_t *display_back_btn = lv_btn_create(sub_display_page);
    lv_obj_set_size(display_back_btn, LV_SIZE_CONTENT, 40);
    lv_obj_set_style_bg_color(display_back_btn, lv_color_hex(0x007ACC), LV_PART_MAIN);
    lv_obj_set_style_radius(display_back_btn, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(display_back_btn, back_to_home_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *display_back_label = lv_label_create(display_back_btn);
    lv_label_set_text(display_back_label, LV_SYMBOL_LEFT " Back");
    lv_obj_set_style_text_color(display_back_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(display_back_label);

    // Display - Each slider gets its own individual section/box
    create_section_header(sub_display_page, "BRIGHTNESS");
    lv_obj_t *screen_brightness_section = create_section_group(sub_display_page);
    sub_section.push_back(screen_brightness_section);
    create_slider(screen_brightness_section, LV_SYMBOL_SETTINGS, "Screen Brightness", 1, 16, 16, lv_brightness_cb, LV_EVENT_VALUE_CHANGED);

    lv_obj_t *background_opacity_section = create_section_group(sub_display_page);
    sub_section.push_back(background_opacity_section);
    create_slider(background_opacity_section, LV_SYMBOL_SETTINGS, "Background Opacity", 0, 255, DEFAULT_OPA, lv_background_opa_cb, LV_EVENT_VALUE_CHANGED);

    lv_obj_t *keyboard_backlight_section = create_section_group(sub_display_page);
    sub_section.push_back(keyboard_backlight_section);
    create_slider(keyboard_backlight_section, LV_SYMBOL_SETTINGS, "Keyboard Backlight", 0, 255, DEFAULT_OPA, lv_kb_brightness_cb, LV_EVENT_VALUE_CHANGED);

    lv_obj_t *icon_transparency_section = create_section_group(sub_display_page);
    sub_section.push_back(icon_transparency_section);
    create_slider(icon_transparency_section, LV_SYMBOL_SETTINGS, "Icon Transparency", 50, 255, icon_transparency, lv_icon_transparency_cb, LV_EVENT_VALUE_CHANGED);


    // !GPS
    sub_gps_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sub_gps_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(sub_gps_page, 10, 0);
    lv_obj_set_style_bg_color(sub_gps_page, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
    lv_obj_set_style_border_width(sub_gps_page, 0, LV_PART_MAIN);
    lv_obj_add_flag(sub_gps_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_flex_flow(sub_gps_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sub_gps_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // Enable scrolling for GPS page
    lv_obj_set_scroll_dir(sub_gps_page, LV_DIR_VER);
    
    // Apply comprehensive scroll optimizations for smooth scrolling
    optimize_page_scrolling(sub_gps_page);

    // Add back button
    lv_obj_t *gps_back_btn = lv_btn_create(sub_gps_page);
    lv_obj_set_size(gps_back_btn, LV_SIZE_CONTENT, 40);
    lv_obj_set_style_bg_color(gps_back_btn, lv_color_hex(0x007ACC), LV_PART_MAIN);
    lv_obj_set_style_radius(gps_back_btn, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(gps_back_btn, back_to_home_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *gps_back_label = lv_label_create(gps_back_btn);
    lv_label_set_text(gps_back_label, LV_SYMBOL_LEFT " Back");
    lv_obj_set_style_text_color(gps_back_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(gps_back_label);

    // GPS - Each label gets its own individual section/box
    lv_obj_t *model_section = create_section_group(sub_gps_page);
    sub_section.push_back(model_section);
    lv_obj_t *model_label = create_label(model_section, LV_SYMBOL_GPS, "Model", gps_model.c_str());

    lv_obj_t *use_seconds_section = create_section_group(sub_gps_page);
    sub_section.push_back(use_seconds_section);
    sub_gps_val.label_use_seconds = create_label(use_seconds_section, LV_SYMBOL_GPS, "Use Seconds", "N.A");

    lv_obj_t *lat_section = create_section_group(sub_gps_page);
    sub_section.push_back(lat_section);
    sub_gps_val.label_lat = create_label(lat_section, LV_SYMBOL_GPS, "lat", "N.A");

    lv_obj_t *lng_section = create_section_group(sub_gps_page);
    sub_section.push_back(lng_section);
    sub_gps_val.label_lng = create_label(lng_section, LV_SYMBOL_GPS, "lng", "N.A");

    lv_obj_t *speed_section = create_section_group(sub_gps_page);
    sub_section.push_back(speed_section);
    sub_gps_val.label_speed = create_label(speed_section, LV_SYMBOL_SETTINGS, "Speed", "N.A");

    lv_obj_t *date_section = create_section_group(sub_gps_page);
    sub_section.push_back(date_section);
    sub_gps_val.label_date = create_label(date_section, LV_SYMBOL_SETTINGS, "Date", "N.A");

    lv_obj_t *time_section = create_section_group(sub_gps_page);
    sub_section.push_back(time_section);
    sub_gps_val.label_time = create_label(time_section, LV_SYMBOL_SETTINGS, "Time", "N.A");

    lv_obj_t *rx_section = create_section_group(sub_gps_page);
    sub_section.push_back(rx_section);
    sub_gps_val.label_processchar = create_label(rx_section, LV_SYMBOL_SETTINGS, "Rx", "N.A");

    //! KEYBOARD
    sub_kb_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sub_kb_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(sub_kb_page, 10, 0);
    lv_obj_set_style_bg_color(sub_kb_page, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
    lv_obj_set_style_border_width(sub_kb_page, 0, LV_PART_MAIN);
    lv_obj_add_flag(sub_kb_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_flex_flow(sub_kb_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sub_kb_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Apply comprehensive scroll optimizations for smooth scrolling
    optimize_page_scrolling(sub_kb_page);

    // Add back button
    lv_obj_t *kb_back_btn = lv_btn_create(sub_kb_page);
    lv_obj_set_size(kb_back_btn, LV_SIZE_CONTENT, 40);
    lv_obj_set_style_bg_color(kb_back_btn, lv_color_hex(0x007ACC), LV_PART_MAIN);
    lv_obj_set_style_radius(kb_back_btn, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(kb_back_btn, back_to_home_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *kb_back_label = lv_label_create(kb_back_btn);
    lv_label_set_text(kb_back_label, LV_SYMBOL_LEFT " Back");
    lv_obj_set_style_text_color(kb_back_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(kb_back_label);

    // Keyboard - Text input gets its own individual section/box
    create_section_header(sub_kb_page, "TEXT INPUT");
    lv_obj_t *textarea_section = create_section_group(sub_kb_page);
    sub_section.push_back(textarea_section);

    lv_obj_t *radio_ta = lv_textarea_create(textarea_section);
    lv_textarea_set_cursor_click_pos(radio_ta, false);
    lv_textarea_set_text_selection(radio_ta, false);
    lv_obj_set_size(radio_ta, LV_PCT(90), 150);
    lv_textarea_set_text(radio_ta, "");
    lv_textarea_set_max_length(radio_ta, 1024);
    
    // iPhone-style textarea
    lv_obj_set_style_bg_color(radio_ta, lv_color_hex(0x2D2D30), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(radio_ta, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(radio_ta, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(radio_ta, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(radio_ta, lv_color_hex(0x3E3E42), LV_PART_MAIN);
    lv_obj_set_style_border_opa(radio_ta, LV_OPA_60, LV_PART_MAIN);
    lv_obj_set_style_pad_all(radio_ta, 8, LV_PART_MAIN);
    lv_obj_set_style_text_font(radio_ta, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_color(radio_ta, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    
    sub_section.push_back(radio_ta);


    lv_timer_create([](lv_timer_t *t) {
        extern lv_indev_t  *kb_indev ;
        if (NULL == kb_indev) {
            lv_obj_t *radio_ta = (lv_obj_t *)t->user_data;
            lv_textarea_set_text(radio_ta, "Keyboard is offline");
            lv_obj_invalidate(radio_ta);
        }
    }, 3000, radio_ta);

    //! SD
    // lv_obj_t *sub_sd_page = lv_menu_page_create(menu, NULL);
    // lv_obj_set_style_pad_hor(sub_sd_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    // lv_menu_separator_create(sub_sd_page);
    // section = lv_menu_section_create(sub_sd_page);

    //! SETTING
    sub_setting_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sub_setting_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(sub_setting_page, 10, 0);
    lv_obj_set_style_bg_color(sub_setting_page, lv_color_hex(0x1E1E1E), LV_PART_MAIN);
    lv_obj_set_style_border_width(sub_setting_page, 0, LV_PART_MAIN);
    lv_obj_add_flag(sub_setting_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_flex_flow(sub_setting_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sub_setting_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Apply comprehensive scroll optimizations for smooth scrolling
    optimize_page_scrolling(sub_setting_page);

    // Add back button
    lv_obj_t *settings_back_btn = lv_btn_create(sub_setting_page);
    lv_obj_set_size(settings_back_btn, LV_SIZE_CONTENT, 40);
    lv_obj_set_style_bg_color(settings_back_btn, lv_color_hex(0x007ACC), LV_PART_MAIN);
    lv_obj_set_style_radius(settings_back_btn, 8, LV_PART_MAIN);
    lv_obj_add_event_cb(settings_back_btn, back_to_home_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *settings_back_label = lv_label_create(settings_back_btn);
    lv_label_set_text(settings_back_label, LV_SYMBOL_LEFT " Back");
    lv_obj_set_style_text_color(settings_back_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(settings_back_label);

    // Settings - Each item gets its own individual section/box
    create_section_header(sub_setting_page, "DEVICE INFO");
    
    lv_obj_t *mac_section = create_section_group(sub_setting_page);
    sub_section.push_back(mac_section);
    uint8_t mac[6] = {0};
    esp_efuse_mac_get_default(mac);
    static char buffer [128] = {0};
    snprintf(buffer, 128, "%X:%X:%X:%X:%X:%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    create_label(mac_section, NULL, "MAC Address", buffer);

    lv_obj_t *sd_section = create_section_group(sub_setting_page);
    sub_section.push_back(sd_section);
    float sd_size = SD.cardSize() / 1024 / 1024 / 1024.0;
    create_label(sd_section, NULL, "SD Card", (SD.cardSize() != 0) ? (String(sd_size) + "GB").c_str() : "N.A");

    lv_obj_t *battery_section = create_section_group(sub_setting_page);
    sub_section.push_back(battery_section);
    lv_obj_t *voltage_label = create_label(battery_section, NULL, "Battery", "N.A");
    lv_timer_create([](lv_timer_t *t) {
        lv_obj_t *voltage_label = (lv_obj_t *)t->user_data;
        lv_label_set_text_fmt(voltage_label, "%u mV", analogReadMilliVolts(BOARD_BAT_ADC) * 2);
    }, 10000, voltage_label);

    create_section_header(sub_setting_page, "NETWORK");
    
    lv_obj_t *wifi_ssid_section = create_section_group(sub_setting_page);
    sub_section.push_back(wifi_ssid_section);
    const char *wifi_name = WIFI_SSID;
    create_label(wifi_ssid_section, NULL, "WiFi SSID", wifi_name);

    lv_obj_t *ip_section = create_section_group(sub_setting_page);
    sub_section.push_back(ip_section);
    const char *wifi_ip = "N.A";
    lv_obj_t *label = create_label(ip_section, NULL, "IP Address", wifi_ip);
    lv_msg_subsribe_obj(_BV(1), label, NULL);
    lv_obj_add_event_cb( label, [](lv_event_t *e) {
        lv_obj_t *label = (lv_obj_t *)lv_event_get_target(e);
        if (WiFi.isConnected()) {
            lv_label_set_text_fmt(label, "%s", (WiFi.localIP().toString().c_str()));
        } else {
            lv_label_set_text(label, "N.A");
        }
    }, LV_EVENT_MSG_RECEIVED, NULL);

    lv_obj_t *signal_section = create_section_group(sub_setting_page);
    sub_section.push_back(signal_section);
    const char *wifi_rssi = "N.A";
    lv_obj_t *wifi_rssi_label = create_label(signal_section, NULL, "Signal Strength", wifi_rssi);

    lv_timer_create([](lv_timer_t *t) {
        lv_obj_t *wifi_rssi_label = (lv_obj_t *)t->user_data;
        if (WiFi.isConnected()) {
            lv_label_set_text_fmt(wifi_rssi_label, "%d dBm", (WiFi.RSSI()));
        }
    }, 3000, wifi_rssi_label);

    lv_obj_t *ntp_section = create_section_group(sub_setting_page);
    sub_section.push_back(ntp_section);
    lv_obj_t *ntp_datetime = create_label(ntp_section, NULL, "Network Time", "00:00:00");

    lv_timer_create([](lv_timer_t *t) {
        lv_obj_t *ntp_datetime = (lv_obj_t *)t->user_data;
        if (WiFi.isConnected()) {
            time_t now;
            struct tm  timeinfo;
            time(&now);
            localtime_r(&now, &timeinfo);
            char datetime[128] = {0};
            snprintf(datetime, 128, "%d/%d/%d %d:%d:%d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                     timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
            lv_label_set_text_fmt(ntp_datetime, "%s", datetime);
        }
    }, 1000, ntp_datetime);

    create_section_header(sub_setting_page, "SOFTWARE");
    
    lv_obj_t *lvgl_section = create_section_group(sub_setting_page);
    sub_section.push_back(lvgl_section);
    String lvgl_version = String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    create_label(lvgl_section, NULL, "LVGL", lvgl_version.c_str());

    lv_obj_t *arduino_section = create_section_group(sub_setting_page);
    sub_section.push_back(arduino_section);
    String arduino_version = String('V') + String(ESP_ARDUINO_VERSION_MAJOR) + "." + String(ESP_ARDUINO_VERSION_MINOR) + "." + String(ESP_ARDUINO_VERSION_PATCH);
    create_label(arduino_section, NULL, "Arduino ESP32", arduino_version.c_str());

    lv_obj_t *tft_section = create_section_group(sub_setting_page);
    sub_section.push_back(tft_section);
    const char *tft_espi_version = "V2.5.22";
    create_label(tft_section, NULL, "TFT_eSPI", tft_espi_version);

    create_section_header(sub_setting_page, "POWER");
    lv_obj_t *power_section = create_section_group(sub_setting_page);
    sub_section.push_back(power_section);
    create_button(power_section, LV_SYMBOL_POWER, "Sleep Mode", sleep_event_cb);



    /*Create a home page with app icons*/
    // Create the home screen app icons
    // This replaces the old sidebar navigation with app icons
    lv_obj_set_style_pad_hor(home_screen, 8, 0);
    lv_obj_set_style_pad_ver(home_screen, 8, 0);
    // Background image is already set above - no need for solid color
    
    // Create grid container for app icons
    lv_obj_t *grid_cont = lv_obj_create(home_screen);
    lv_obj_set_size(grid_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(grid_cont, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(grid_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(grid_cont, 10, LV_PART_MAIN);
    
    // Set grid layout (3 columns, auto rows)
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(grid_cont, col_dsc, row_dsc);
    
    // Create app icons with navigation
    lv_obj_t *radio_icon = create_app_icon(grid_cont, LV_SYMBOL_SETTINGS, "Radio", sub_mechanics_page, app_icon_cb);
    lv_obj_set_grid_cell(radio_icon, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    
    lv_obj_t *sound_icon = create_app_icon(grid_cont, LV_SYMBOL_AUDIO, "Sound", sub_sound_page, app_icon_cb);
    lv_obj_set_grid_cell(sound_icon, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    
    lv_obj_t *display_icon = create_app_icon(grid_cont, LV_SYMBOL_EYE_OPEN, "Display", sub_display_page, app_icon_cb);
    lv_obj_set_grid_cell(display_icon, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    
    lv_obj_t *gps_icon = create_app_icon(grid_cont, LV_SYMBOL_GPS, "GPS", sub_gps_page, app_icon_cb);
    lv_obj_set_grid_cell(gps_icon, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    
    lv_obj_t *kb_icon = create_app_icon(grid_cont, LV_SYMBOL_KEYBOARD, "Keyboard", sub_kb_page, app_icon_cb);
    lv_obj_set_grid_cell(kb_icon, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    
    lv_obj_t *config_icon = create_app_icon(grid_cont, LV_SYMBOL_SETTINGS, "Settings", sub_setting_page, app_icon_cb);
    lv_obj_set_grid_cell(config_icon, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    // Show home screen by default (container-based navigation)
    // No need for sidebar or back button handlers anymore
    
    // Apply basic styling to sections
    std::vector<lv_obj_t *>::iterator it;
    for (it = sub_section.begin(); it != sub_section.end(); it++) {
        lv_obj_set_style_bg_opa(*it, DEFAULT_OPA, LV_PART_MAIN);
        // Optimize scrolling for each section
        ui_optimize_scrolling(*it);
    }
    
    // End batched updates for better performance
    ui_batch_style_updates_end();
    
    // Explicitly show the home screen and ensure it's visible
    lv_obj_clear_flag(home_screen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_to_index(home_screen, 0); // Bring to front
    
    // Force multiple LVGL updates to ensure home screen is displayed
    for (int i = 0; i < 5; i++) {
        lv_timer_handler();
        lv_refr_now(NULL);  // Force immediate refresh
    }
    
    // Set up optimized timers for periodic updates - commented out for now
    /*
    static lv_timer_t *gps_timer = nullptr;
    static lv_timer_t *battery_timer = nullptr;
    static lv_timer_t *wifi_timer = nullptr;
    
    // GPS updates every 500ms instead of every LVGL tick
    gps_timer = lv_timer_create([](lv_timer_t *t) {
        // GPS update will be handled by external GPS callback
        // This timer can be used for GPS timeout detection
    }, UI_GPS_UPDATE_RATE_MS, NULL);
    
    // Battery updates every 5 seconds
    battery_timer = lv_timer_create([](lv_timer_t *t) {
        uint16_t voltage = analogReadMilliVolts(BOARD_BAT_ADC) * 2;
        ui_update_battery_fast(voltage);
    }, UI_BATTERY_UPDATE_RATE_MS, NULL);
    
    // WiFi updates every 2 seconds
    wifi_timer = lv_timer_create([](lv_timer_t *t) {
        if (WiFi.isConnected()) {
            ui_update_wifi_fast(WiFi.RSSI(), true);
        } else {
            ui_update_wifi_fast(0, false);
        }
    }, UI_WIFI_UPDATE_RATE_MS, NULL);
    */
}

static void back_event_handler(lv_event_t *e)
{
    // Simple back navigation - return to home screen
    back_to_home_cb(e);
}

static void switch_handler(lv_event_t *e)
{
    // Not needed for container-based navigation
    // This was used for menu sidebar toggling
}

static lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt)
{
    // Create a simple container for the text element
    lv_obj_t *obj = lv_obj_create(parent);
    
    // Simple styling that should work with any LVGL version
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x2D2D30), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(obj, lv_color_hex(0x3E3E42), LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 12, LV_PART_MAIN);
    lv_obj_set_style_min_height(obj, 60, LV_PART_MAIN);
    lv_obj_set_width(obj, LV_PCT(100));
    lv_obj_set_height(obj, LV_SIZE_CONTENT);
    
    // Use column layout for sliders to stack text above controls
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Create header row for icon and text
    lv_obj_t *header_row = lv_obj_create(obj);
    lv_obj_set_width(header_row, LV_PCT(100));
    lv_obj_set_height(header_row, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(header_row, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(header_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(header_row, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(header_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    lv_obj_t *img = NULL;
    lv_obj_t *label = NULL;

    if (icon) {
        img = lv_img_create(header_row);
        lv_img_set_src(img, icon);
        lv_obj_set_style_pad_right(img, 8, LV_PART_MAIN);
    }

    if (txt) {
        label = lv_label_create(header_row);
        lv_label_set_text(label, txt);
        lv_obj_set_style_text_color(label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN);
        lv_obj_set_flex_grow(label, 1); // Take up remaining space
    }

    return obj;
}

static lv_obj_t *create_slider(lv_obj_t *parent, const char *icon, const char *txt, int32_t min, int32_t max,
                               int32_t val, lv_event_cb_t cb, lv_event_code_t filter)
{
    lv_obj_t *obj = create_text(parent, icon, txt);

    // Create value label in the header row to show current slider value
    lv_obj_t *header_row = lv_obj_get_child(obj, 0);  // Get the header row container
    lv_obj_t *value_label = lv_label_create(header_row);
    lv_label_set_text_fmt(value_label, "%d", (int)val);
    lv_obj_set_style_text_color(value_label, lv_color_hex(0x007ACC), LV_PART_MAIN);
    lv_obj_set_style_text_font(value_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_align(value_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);

    // Add some spacing between text and slider
    lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN);

    lv_obj_t *slider = lv_slider_create(obj);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);
    
    // Store value label reference in slider user data for updates
    lv_obj_set_user_data(slider, value_label);
    
    // Center the slider and give it appropriate width
    lv_obj_set_width(slider, LV_PCT(80)); // Use percentage for responsive design
    lv_obj_set_height(slider, 8);
    lv_obj_set_style_pad_top(slider, 8, LV_PART_MAIN);
    
    // iPhone-style slider track
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x3E3E42), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(slider, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(slider, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(slider, 0, LV_PART_MAIN);
    
    // Slider indicator (filled part) - color based on value
    if (val > min) {
        // Slider has value - use blue color
        lv_obj_set_style_bg_color(slider, lv_color_hex(0x007ACC), LV_PART_INDICATOR);
    } else {
        // Slider at minimum - use greyed out color
        lv_obj_set_style_bg_color(slider, lv_color_hex(0x5A5A5A), LV_PART_INDICATOR);
    }
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    
    // Slider knob
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xCCCCCC), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_border_width(slider, 1, LV_PART_KNOB);
    lv_obj_set_style_border_color(slider, val > min ? lv_color_hex(0x007ACC) : lv_color_hex(0x5A5A5A), LV_PART_KNOB);
    lv_obj_set_style_shadow_width(slider, 4, LV_PART_KNOB);
    lv_obj_set_style_shadow_color(slider, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(slider, LV_OPA_20, LV_PART_KNOB);
    lv_obj_set_style_shadow_spread(slider, 1, LV_PART_KNOB);
    lv_obj_set_style_width(slider, 22, LV_PART_KNOB);
    lv_obj_set_style_height(slider, 22, LV_PART_KNOB);

    if (cb != NULL) {
        lv_obj_add_event_cb(slider, cb, filter, NULL);
        // Add event to update styling and value label when slider changes
        lv_obj_add_event_cb(slider, [](lv_event_t *e) {
            lv_obj_t *slider = lv_event_get_target(e);
            int32_t value = lv_slider_get_value(slider);
            int32_t min_val = lv_slider_get_min_value(slider);
            
            // Update value label
            lv_obj_t *value_label = (lv_obj_t *)lv_obj_get_user_data(slider);
            if (value_label) {
                lv_label_set_text_fmt(value_label, "%d", (int)value);
            }
            
            if (value > min_val) {
                // Slider has value - use blue color
                lv_obj_set_style_bg_color(slider, lv_color_hex(0x007ACC), LV_PART_INDICATOR);
                lv_obj_set_style_border_color(slider, lv_color_hex(0x007ACC), LV_PART_KNOB);
            } else {
                // Slider at minimum - use greyed out color
                lv_obj_set_style_bg_color(slider, lv_color_hex(0x5A5A5A), LV_PART_INDICATOR);
                lv_obj_set_style_border_color(slider, lv_color_hex(0x5A5A5A), LV_PART_KNOB);
            }
        }, LV_EVENT_VALUE_CHANGED, NULL);
    }

    return slider;
}

static lv_obj_t *create_switch(lv_obj_t *parent, const char *icon, const char *txt, bool chk, lv_event_cb_t cb)
{
    lv_obj_t *obj = create_text(parent, icon, txt);

    lv_obj_t *sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);
    
    // iPhone-style switch styling
    lv_obj_set_style_width(sw, 40, LV_PART_MAIN);
    lv_obj_set_style_height(sw, 24, LV_PART_MAIN);
    lv_obj_set_style_radius(sw, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(sw, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(sw, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(sw, 0, LV_PART_MAIN);
    
    // Style based on switch state
    if (chk) {
        // Switch is ON - use blue color
        lv_obj_set_style_bg_color(sw, lv_color_hex(0x007ACC), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, LV_PART_MAIN);
    } else {
        // Switch is OFF - use greyed out color
        lv_obj_set_style_bg_color(sw, lv_color_hex(0x3E3E42), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, LV_PART_MAIN);
    }
    
    // Knob styling
    lv_obj_set_style_bg_color(sw, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(sw, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_radius(sw, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_shadow_width(sw, 2, LV_PART_KNOB);
    lv_obj_set_style_shadow_color(sw, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(sw, LV_OPA_30, LV_PART_KNOB);
    
    if (cb) {
        lv_obj_add_event_cb(sw, cb, LV_EVENT_VALUE_CHANGED, NULL);
        // Also add event to update styling when switch changes
        lv_obj_add_event_cb(sw, [](lv_event_t *e) {
            lv_obj_t *sw = lv_event_get_target(e);
            bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
            
            if (is_checked) {
                // Switch turned ON - use blue color
                lv_obj_set_style_bg_color(sw, lv_color_hex(0x007ACC), LV_PART_MAIN);
            } else {
                // Switch turned OFF - use greyed out color
                lv_obj_set_style_bg_color(sw, lv_color_hex(0x3E3E42), LV_PART_MAIN);
            }
        }, LV_EVENT_VALUE_CHANGED, NULL);
    }
    
    return sw;
}

static lv_obj_t *create_button(lv_obj_t *parent, const char *icon, const char *txt, lv_event_cb_t cb)
{
    lv_obj_t *obj = create_text(parent, icon, txt);
    lv_obj_t *btn = lv_btn_create(obj);
    
    // iPhone-style button
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x007ACC), LV_PART_MAIN); // VS Code blue
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 3, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(btn, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_shadow_ofs_y(btn, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_all(btn, 8, LV_PART_MAIN);
    
    // Button pressed state
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x005A9E), LV_STATE_PRESSED);
    lv_obj_set_style_transform_zoom(btn, 245, LV_STATE_PRESSED);
    
    // Button text
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Action");
    lv_obj_set_style_text_color(btn_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_center(btn_label);
    
    lv_obj_set_size(btn, lv_pct(25), 28);
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, -4, 0);
    
    if (cb) {
        lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
    }
    return obj;
}

static lv_obj_t *create_label(lv_obj_t *parent, const char *icon, const char *txt, const char *default_text)
{
    lv_obj_t *obj = create_text(parent, icon, txt);
    if (default_text) {
        lv_obj_t *label = lv_label_create(obj);
        lv_label_set_text(label, default_text);
        
        // iPhone-style secondary text
        lv_obj_set_style_text_color(label, lv_color_hex(0x9CDCFE), LV_PART_MAIN); // VS Code light blue
        lv_obj_set_style_text_font(label, &lv_font_montserrat_12, LV_PART_MAIN);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
        
        return label;
    }
    return obj;
}

static lv_obj_t *create_dropdown(lv_obj_t *parent, const char *icon, const char *txt, const char *options, uint8_t default_sel, lv_event_cb_t cb)
{
    lv_obj_t *obj = create_text(parent, icon, txt);
    lv_obj_t *dd = lv_dropdown_create(obj);
    lv_dropdown_set_options(dd, options);
    lv_dropdown_set_selected(dd, default_sel);
    
    // iPhone-style dropdown
    lv_obj_set_style_bg_color(dd, lv_color_hex(0x2D2D30), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(dd, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(dd, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(dd, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(dd, lv_color_hex(0x3E3E42), LV_PART_MAIN);
    lv_obj_set_style_border_opa(dd, LV_OPA_60, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(dd, 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(dd, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(dd, LV_OPA_10, LV_PART_MAIN);
    lv_obj_set_style_shadow_ofs_y(dd, 1, LV_PART_MAIN);
    lv_obj_set_style_pad_all(dd, 8, LV_PART_MAIN);
    lv_obj_set_style_text_color(dd, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_font(dd, &lv_font_montserrat_12, LV_PART_MAIN);
    
    // Dropdown list styling
    lv_obj_set_style_bg_color(dd, lv_color_hex(0x007ACC), LV_PART_SELECTED);
    lv_obj_set_style_bg_opa(dd, LV_OPA_COVER, LV_PART_SELECTED);
    lv_obj_set_style_text_color(dd, lv_color_hex(0xFFFFFF), LV_PART_SELECTED);
    
    // Dropdown button (arrow)
    lv_obj_set_style_bg_color(dd, lv_color_hex(0x007ACC), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(dd, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(dd, 4, LV_PART_INDICATOR);
    
    lv_obj_set_width(dd, 80);
    
    if (cb) {
        lv_obj_add_event_cb(dd, cb, LV_EVENT_VALUE_CHANGED, NULL);
    }
    return dd;
}













