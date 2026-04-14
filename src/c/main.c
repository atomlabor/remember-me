#include <pebble.h>

#define KEY_REMINDER_1       0
#define KEY_REMINDER_2       1
#define KEY_REMINDER_3       2
#define KEY_COLOR_TOP        3
#define KEY_COLOR_BOTTOM     4
#define KEY_OUTLINE_WHITE    5
#define KEY_COLOR_TEXT       6
#define KEY_HIDE_ICON        7
#define KEY_ENABLE_GRADIENT  8
#define KEY_COLOR_BOTTOM_END 9
#define KEY_COLOR_TIME       10
#define KEY_COLOR_BATTERY    11

static Window *s_main_window;
static Layer *s_canvas_layer;
static GFont s_time_font;
static char s_time_buffer[8] = "00:00";
static int s_time_y, s_time_h;

static TextLayer *s_reminder_layer;
static BitmapLayer *s_icon_layer;
static GBitmap *s_reminder_icon;

static char s_reminder_1[64] = "Keep Smiling";
static char s_reminder_2[64] = "Drink Water";
static char s_reminder_3[64] = "Stay Focused";
static int s_current_index = 0;
static int s_battery_level = 100;

static GColor s_color_top, s_color_bottom, s_color_bottom_end, s_color_text, s_color_time, s_color_battery;
static bool s_outline_white, s_hide_icon, s_enable_gradient;

static int get_layout_offset() {
    #if defined(PBL_BW) || defined(PBL_PLATFORM_BASALT)
        return 5;
    #else
        return 0;
    #endif
}

static void update_layout() {
    Layer *window_layer = window_get_root_layer(s_main_window);
    GRect bounds = layer_get_bounds(window_layer);
    
    int top_height = (bounds.size.h / 3) + PBL_IF_ROUND_ELSE(10, 0) + get_layout_offset();
    int accent_height = 8;
    int icon_y = top_height + 4 - 12;
    
    if (s_hide_icon) {
        layer_set_hidden(bitmap_layer_get_layer(s_icon_layer), true);
        int reminder_area_top = top_height + accent_height; 
        int reminder_area_height = bounds.size.h - reminder_area_top;
        layer_set_frame(text_layer_get_layer(s_reminder_layer), GRect(0, reminder_area_top, bounds.size.w, reminder_area_height));
    } else {
        layer_set_hidden(bitmap_layer_get_layer(s_icon_layer), false);
        int reminder_area_top = icon_y + 29;
        int reminder_area_height = bounds.size.h - reminder_area_top;
        layer_set_frame(text_layer_get_layer(s_reminder_layer), GRect(0, reminder_area_top, bounds.size.w, reminder_area_height));
    }

    #if defined(PBL_COLOR)
        text_layer_set_text_color(s_reminder_layer, s_color_text);
    #else
        text_layer_set_text_color(s_reminder_layer, gcolor_equal(s_color_bottom, GColorBlack) ? GColorWhite : GColorBlack);
    #endif
}

static void set_responsive_font(const char* text) {
    int len = strlen(text);
    if (len < 15) text_layer_set_font(s_reminder_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    else if (len < 25) text_layer_set_font(s_reminder_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    else text_layer_set_font(s_reminder_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
}

static void update_reminder_display() {
    const char* active_text = (s_current_index == 0) ? s_reminder_1 : (s_current_index == 1 ? s_reminder_2 : s_reminder_3);
    set_responsive_font(active_text);
    text_layer_set_text(s_reminder_layer, active_text);
    update_layout();
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
    s_current_index = (s_current_index + 1) % 3;
    update_reminder_display();
}

static void battery_callback(BatteryChargeState state) {
    s_battery_level = state.charge_percent;
    if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);
}

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
    if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *t;
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_REMINDER_1))) { strncpy(s_reminder_1, t->value->cstring, 63); persist_write_string(KEY_REMINDER_1, s_reminder_1); }
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_REMINDER_2))) { strncpy(s_reminder_2, t->value->cstring, 63); persist_write_string(KEY_REMINDER_2, s_reminder_2); }
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_REMINDER_3))) { strncpy(s_reminder_3, t->value->cstring, 63); persist_write_string(KEY_REMINDER_3, s_reminder_3); }
    
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_COLOR_TOP))) { s_color_top = GColorFromHEX(t->value->uint32); persist_write_int(KEY_COLOR_TOP, t->value->uint32); }
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_COLOR_BOTTOM))) { s_color_bottom = GColorFromHEX(t->value->uint32); persist_write_int(KEY_COLOR_BOTTOM, t->value->uint32); }
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_COLOR_BOTTOM_END))) { s_color_bottom_end = GColorFromHEX(t->value->uint32); persist_write_int(KEY_COLOR_BOTTOM_END, t->value->uint32); }
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_COLOR_TEXT))) { s_color_text = GColorFromHEX(t->value->uint32); persist_write_int(KEY_COLOR_TEXT, t->value->uint32); }
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_COLOR_TIME))) { s_color_time = GColorFromHEX(t->value->uint32); persist_write_int(KEY_COLOR_TIME, t->value->uint32); }
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_COLOR_BATTERY))) { s_color_battery = GColorFromHEX(t->value->uint32); persist_write_int(KEY_COLOR_BATTERY, t->value->uint32); }
    
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_OUTLINE_WHITE))) { s_outline_white = t->value->int32 == 1; persist_write_bool(KEY_OUTLINE_WHITE, s_outline_white); }
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_HIDE_ICON))) { s_hide_icon = t->value->int32 == 1; persist_write_bool(KEY_HIDE_ICON, s_hide_icon); }
    if ((t = dict_find(iter, MESSAGE_KEY_CONFIG_ENABLE_GRADIENT))) { s_enable_gradient = t->value->int32 == 1; persist_write_bool(KEY_ENABLE_GRADIENT, s_enable_gradient); }
    
    update_reminder_display();
    layer_mark_dirty(s_canvas_layer);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    int top_height = (bounds.size.h / 3) + PBL_IF_ROUND_ELSE(10, 0) + get_layout_offset();
    int accent_height = 8; 
    
    graphics_context_set_fill_color(ctx, s_color_top);
    graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, top_height), 0, GCornerNone);
    graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorSunsetOrange, GColorBlack));
    graphics_fill_rect(ctx, GRect(0, top_height, bounds.size.w, accent_height), 0, GCornerNone);
    
    // SMOOTH GRADIENT DITHERING ENGINE
    #if defined(PBL_COLOR)
        if (s_enable_gradient) {
            int start_y = top_height + accent_height;
            int end_y = bounds.size.h;
            int h = end_y - start_y;
            if (h > 0) {
                // Bayer Matrix 4x4 pre-scaled für Pebble Farben
                static const int bayer[4][4] = {
                    { -40,   0, -30,  10 },
                    {  20, -20,  30, -10 },
                    { -25,  15, -35,   5 },
                    {  35,  -5,  25, -15 }
                };
                for(int y = start_y; y < end_y; y++) {
                    int step = y - start_y;
                    // Skalierung auf 0-255 für GColor
                    int r_interp = (s_color_bottom.r * 85) + (((s_color_bottom_end.r - s_color_bottom.r) * 85 * step) / h);
                    int g_interp = (s_color_bottom.g * 85) + (((s_color_bottom_end.g - s_color_bottom.g) * 85 * step) / h);
                    int b_interp = (s_color_bottom.b * 85) + (((s_color_bottom_end.b - s_color_bottom.b) * 85 * step) / h);

                    for(int x = 0; x < bounds.size.w; x++) {
                        int offset = bayer[y % 4][x % 4];
                        int r = r_interp + offset;
                        int g = g_interp + offset;
                        int b = b_interp + offset;

                        r = (r < 0) ? 0 : ((r > 255) ? 255 : r);
                        g = (g < 0) ? 0 : ((g > 255) ? 255 : g);
                        b = (b < 0) ? 0 : ((b > 255) ? 255 : b);

                        graphics_context_set_stroke_color(ctx, GColorFromRGB(r, g, b));
                        graphics_draw_pixel(ctx, GPoint(x, y));
                    }
                }
            }
        } else {
            graphics_context_set_fill_color(ctx, s_color_bottom);
            graphics_fill_rect(ctx, GRect(0, top_height + accent_height, bounds.size.w, bounds.size.h - (top_height + accent_height)), 0, GCornerNone);
        }
    #else
        graphics_context_set_fill_color(ctx, s_color_bottom);
        graphics_fill_rect(ctx, GRect(0, top_height + accent_height, bounds.size.w, bounds.size.h - (top_height + accent_height)), 0, GCornerNone);
    #endif

    // Battery Rendering
    #if defined(PBL_BW)
    graphics_context_set_compositing_mode(ctx, GCompOpAssignInverted);
    #endif
    graphics_context_set_stroke_color(ctx, s_color_battery);
    graphics_context_set_fill_color(ctx, s_color_battery);
    
    int total_bat_w = (10 * 6) + (9 * 3);
    int start_x = (bounds.size.w - total_bat_w) / 2;
    int active_blocks = (s_battery_level + 5) / 10;
    for (int i = 0; i < 10; i++) {
        GRect r = GRect(start_x + i * 9, PBL_IF_ROUND_ELSE(14, 2), 6, 4);
        if (i < active_blocks) graphics_fill_rect(ctx, r, 1, GCornersAll); 
        else graphics_draw_round_rect(ctx, r, 1);
    }
    #if defined(PBL_BW)
    graphics_context_set_compositing_mode(ctx, GCompOpAssign);
    #endif

    // Time Rendering
    GRect tb = GRect(0, s_time_y, bounds.size.w, s_time_h);
    graphics_context_set_text_color(ctx, s_outline_white ? GColorWhite : GColorBlack);
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            if (dx == 0 && dy == 0) continue; 
            graphics_draw_text(ctx, s_time_buffer, s_time_font, GRect(tb.origin.x + dx, tb.origin.y + dy, tb.size.w, tb.size.h), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
        }
    }
    graphics_context_set_text_color(ctx, s_color_time);
    graphics_draw_text(ctx, s_time_buffer, s_time_font, tb, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    #if defined(PBL_PLATFORM_GABBRO)
        s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BITCOUNT_LIGHT_64));
        s_time_y = 16; s_time_h = 74;
    #elif defined(PBL_PLATFORM_EMERY)
        s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BITCOUNT_LIGHT_56));
        s_time_y = -4; s_time_h = 66;
    #else
        s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BITCOUNT_LIGHT_42));
        s_time_y = PBL_IF_ROUND_ELSE(12, 8); s_time_h = 48;
    #endif

    s_canvas_layer = layer_create(bounds);
    layer_set_update_proc(s_canvas_layer, canvas_update_proc);
    layer_add_child(window_layer, s_canvas_layer);

    int top_height = (bounds.size.h / 3) + PBL_IF_ROUND_ELSE(10, 0) + get_layout_offset();
    s_reminder_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WATCH_ICON);
    int icon_y = top_height + 4 - 12; 
    s_icon_layer = bitmap_layer_create(GRect((bounds.size.w - 25) / 2, icon_y, 25, 25));
    bitmap_layer_set_bitmap(s_icon_layer, s_reminder_icon);
    bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet); 
    layer_add_child(window_layer, bitmap_layer_get_layer(s_icon_layer));

    s_reminder_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
    text_layer_set_background_color(s_reminder_layer, GColorClear);
    text_layer_set_text_alignment(s_reminder_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_reminder_layer));
    
    if (persist_exists(KEY_REMINDER_1)) persist_read_string(KEY_REMINDER_1, s_reminder_1, 64);
    if (persist_exists(KEY_REMINDER_2)) persist_read_string(KEY_REMINDER_2, s_reminder_2, 64);
    if (persist_exists(KEY_REMINDER_3)) persist_read_string(KEY_REMINDER_3, s_reminder_3, 64);
    
    update_layout();
    update_reminder_display();
}

static void main_window_unload(Window *window) {
    layer_destroy(s_canvas_layer); text_layer_destroy(s_reminder_layer);
    bitmap_layer_destroy(s_icon_layer); gbitmap_destroy(s_reminder_icon);
    fonts_unload_custom_font(s_time_font);
}

static void init() {
    s_color_top = persist_exists(KEY_COLOR_TOP) ? GColorFromHEX(persist_read_int(KEY_COLOR_TOP)) : GColorPictonBlue;
    s_color_bottom = persist_exists(KEY_COLOR_BOTTOM) ? GColorFromHEX(persist_read_int(KEY_COLOR_BOTTOM)) : PBL_IF_COLOR_ELSE(GColorPastelYellow, GColorWhite);
    s_color_bottom_end = persist_exists(KEY_COLOR_BOTTOM_END) ? GColorFromHEX(persist_read_int(KEY_COLOR_BOTTOM_END)) : GColorPictonBlue;
    s_color_text = persist_exists(KEY_COLOR_TEXT) ? GColorFromHEX(persist_read_int(KEY_COLOR_TEXT)) : GColorOxfordBlue;
    s_color_time = persist_exists(KEY_COLOR_TIME) ? GColorFromHEX(persist_read_int(KEY_COLOR_TIME)) : PBL_IF_COLOR_ELSE(GColorRed, GColorWhite);
    s_color_battery = persist_exists(KEY_COLOR_BATTERY) ? GColorFromHEX(persist_read_int(KEY_COLOR_BATTERY)) : PBL_IF_COLOR_ELSE(GColorRed, GColorBlack);
    
    s_outline_white = persist_exists(KEY_OUTLINE_WHITE) ? persist_read_bool(KEY_OUTLINE_WHITE) : false;
    s_hide_icon = persist_exists(KEY_HIDE_ICON) ? persist_read_bool(KEY_HIDE_ICON) : false;
    s_enable_gradient = persist_exists(KEY_ENABLE_GRADIENT) ? persist_read_bool(KEY_ENABLE_GRADIENT) : false;
    
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {.load = main_window_load, .unload = main_window_unload});
    window_stack_push(s_main_window, true);
    update_time();
    tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler)update_time);
    accel_tap_service_subscribe((AccelTapHandler)tap_handler);
    battery_state_service_subscribe(battery_callback);
    battery_callback(battery_state_service_peek());
    app_message_register_inbox_received(inbox_received_handler);
    app_message_open(256, 256); 
}

static void deinit() {
    battery_state_service_unsubscribe(); accel_tap_service_unsubscribe();
    tick_timer_service_unsubscribe(); window_destroy(s_main_window);
}

int main(void) { init(); app_event_loop(); deinit(); }