#include <pebble.h>

#define KEY_REMINDER_1     0
#define KEY_REMINDER_2     1
#define KEY_REMINDER_3     2
#define KEY_COLOR_TOP      3
#define KEY_OUTLINE_WHITE  4

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

static GColor s_color_top;
static bool s_outline_white;

static void update_reminder_display() {
    if (s_current_index == 0) text_layer_set_text(s_reminder_layer, s_reminder_1);
    else if (s_current_index == 1) text_layer_set_text(s_reminder_layer, s_reminder_2);
    else text_layer_set_text(s_reminder_layer, s_reminder_3);
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
    Tuple *rem1_t = dict_find(iter, MESSAGE_KEY_CONFIG_REMINDER_1);
    if (rem1_t) {
        strncpy(s_reminder_1, rem1_t->value->cstring, sizeof(s_reminder_1)-1);
        persist_write_string(KEY_REMINDER_1, s_reminder_1);
    }
    Tuple *rem2_t = dict_find(iter, MESSAGE_KEY_CONFIG_REMINDER_2);
    if (rem2_t) {
        strncpy(s_reminder_2, rem2_t->value->cstring, sizeof(s_reminder_2)-1);
        persist_write_string(KEY_REMINDER_2, s_reminder_2);
    }
    Tuple *rem3_t = dict_find(iter, MESSAGE_KEY_CONFIG_REMINDER_3);
    if (rem3_t) {
        strncpy(s_reminder_3, rem3_t->value->cstring, sizeof(s_reminder_3)-1);
        persist_write_string(KEY_REMINDER_3, s_reminder_3);
    }
    Tuple *col_t = dict_find(iter, MESSAGE_KEY_CONFIG_COLOR_TOP);
    if (col_t) {
        s_color_top = GColorFromHEX(col_t->value->uint32);
        persist_write_int(KEY_COLOR_TOP, col_t->value->uint32);
    }
    Tuple *out_t = dict_find(iter, MESSAGE_KEY_CONFIG_OUTLINE_WHITE);
    if (out_t) {
        s_outline_white = out_t->value->int32 == 1;
        persist_write_bool(KEY_OUTLINE_WHITE, s_outline_white);
    }
    update_reminder_display();
    if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    GColor color_accent = PBL_IF_COLOR_ELSE(GColorSunsetOrange, GColorBlack);
    GColor color_bottom = PBL_IF_COLOR_ELSE(GColorPastelYellow, GColorWhite);

    int top_height = (bounds.size.h / 3) + PBL_IF_ROUND_ELSE(10, 0);
    int accent_height = 8; 
    
    graphics_context_set_fill_color(ctx, s_color_top);
    graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, top_height), 0, GCornerNone);
    graphics_context_set_fill_color(ctx, color_accent);
    graphics_fill_rect(ctx, GRect(0, top_height, bounds.size.w, accent_height), 0, GCornerNone);
    graphics_context_set_fill_color(ctx, color_bottom);
    graphics_fill_rect(ctx, GRect(0, top_height + accent_height, bounds.size.w, bounds.size.h - (top_height + accent_height)), 0, GCornerNone);

    int total_bat_w = (10 * 6) + (9 * 3);
    int start_x = (bounds.size.w - total_bat_w) / 2;
    int active_blocks = (s_battery_level + 5) / 10;
    graphics_context_set_stroke_color(ctx, PBL_IF_COLOR_ELSE(GColorRed, GColorBlack));
    graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorRed, GColorBlack));
    for (int i = 0; i < 10; i++) {
        GRect r = GRect(start_x + i * 9, PBL_IF_ROUND_ELSE(14, 2), 6, 4);
        if (i < active_blocks) graphics_fill_rect(ctx, r, 1, GCornersAll); 
        else graphics_draw_round_rect(ctx, r, 1);
    }

    GRect tb = GRect(0, s_time_y, bounds.size.w, s_time_h);
    graphics_context_set_text_color(ctx, s_outline_white ? GColorWhite : GColorBlack);
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            if (dx == 0 && dy == 0) continue; 
            graphics_draw_text(ctx, s_time_buffer, s_time_font, GRect(tb.origin.x + dx, tb.origin.y + dy, tb.size.w, tb.size.h), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
        }
    }
    graphics_context_set_text_color(ctx, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
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

    int top_height = (bounds.size.h / 3) + PBL_IF_ROUND_ELSE(10, 0);
    s_reminder_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WATCH_ICON);
    int icon_y = top_height + 4 - 12; 
    s_icon_layer = bitmap_layer_create(GRect((bounds.size.w - 25) / 2, icon_y, 25, 25));
    bitmap_layer_set_bitmap(s_icon_layer, s_reminder_icon);
    bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet); 
    layer_add_child(window_layer, bitmap_layer_get_layer(s_icon_layer));

    int margin_x = PBL_IF_ROUND_ELSE(bounds.size.w / 6, 10);
    s_reminder_layer = text_layer_create(GRect(margin_x, icon_y + 31, bounds.size.w - (2 * margin_x), bounds.size.h - (icon_y + 31)));
    text_layer_set_background_color(s_reminder_layer, GColorClear);
    text_layer_set_text_color(s_reminder_layer, PBL_IF_COLOR_ELSE(GColorOxfordBlue, GColorBlack));
    text_layer_set_font(s_reminder_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(s_reminder_layer, GTextAlignmentCenter);
    
    if (persist_exists(KEY_REMINDER_1)) persist_read_string(KEY_REMINDER_1, s_reminder_1, sizeof(s_reminder_1));
    if (persist_exists(KEY_REMINDER_2)) persist_read_string(KEY_REMINDER_2, s_reminder_2, sizeof(s_reminder_2));
    if (persist_exists(KEY_REMINDER_3)) persist_read_string(KEY_REMINDER_3, s_reminder_3, sizeof(s_reminder_3));
    
    update_reminder_display();
    layer_add_child(window_layer, text_layer_get_layer(s_reminder_layer));
}

static void main_window_unload(Window *window) {
    layer_destroy(s_canvas_layer); text_layer_destroy(s_reminder_layer);
    bitmap_layer_destroy(s_icon_layer); gbitmap_destroy(s_reminder_icon);
    fonts_unload_custom_font(s_time_font);
}

static void init() {
    s_color_top = persist_exists(KEY_COLOR_TOP) ? GColorFromHEX(persist_read_int(KEY_COLOR_TOP)) : GColorPictonBlue;
    s_outline_white = persist_exists(KEY_OUTLINE_WHITE) ? persist_read_bool(KEY_OUTLINE_WHITE) : false;
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