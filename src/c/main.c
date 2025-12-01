#include <pebble.h>

#define SETTINGS_KEY 1
typedef struct ClaySettings {
  int upper_bound;
} ClaySettings;
static ClaySettings settings;

/* State */

static int current_number = 1;

/* UI */

static Window *s_window;
static TextLayer *s_text_layer;

static void prv_update_ui() {
  static char string_buffer[7];
  snprintf(string_buffer, sizeof(string_buffer), "%d", current_number);
  text_layer_set_text(s_text_layer, string_buffer);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  const int text_h = 32 + 10;
  GRect text_bounds = GRect(0, (bounds.size.h - text_h) / 2, bounds.size.w, text_h);
  GFont text_font = fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS);

  s_text_layer = text_layer_create(text_bounds);

  text_layer_set_background_color(s_text_layer, GColorBlack);
  text_layer_set_font(s_text_layer, text_font);
  text_layer_set_text_color(s_text_layer, GColorWhite);
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
  prv_update_ui();
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

/* Input */

static void prv_click_handler(ClickRecognizerRef recognizer, void *context) {
  current_number = 1 + rand() % settings.upper_bound;
  prv_update_ui();
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, prv_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_click_handler);
}

/* Settings */

static void prv_default_settings() {
  settings.upper_bound = 6;
}

static void prv_load_settings() {
  if (persist_exists(SETTINGS_KEY)) {
    persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
  } else {
    prv_default_settings();
  }
  current_number = settings.upper_bound;
}

static void prv_save_settings() {
  if (settings.upper_bound < 1) {
    settings.upper_bound = 1;
  }

  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  current_number = settings.upper_bound;
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *upper_bound_t = dict_find(iter, MESSAGE_KEY_UPPER_BOUND);
  if (upper_bound_t) {
    settings.upper_bound = upper_bound_t->value->int32;
  }

  prv_save_settings();
  prv_update_ui();
}

/* Lifecycle */

static void prv_init(void) {
  prv_load_settings();

  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);

  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void) {
  prv_save_settings();
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
