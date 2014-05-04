#include "pebble.h"

static Window* window;
static TextLayer* time_text_layer;
static TextLayer* day_text_layer;
static TextLayer* date_text_layer;
static Layer* background_layer;
static GBitmap *image;
static GFont font, font_small;
static char time_text[6];
static char day_text[15];
static char date_text[15];

static void background_layer_update(Layer *layer_to_update, GContext* ctx) {
	GRect bounds = image->bounds;
	graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { 0, 0 }, .size = bounds.size });
}

void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
	if (!tick_time) {
		time_t now = time(NULL);
		tick_time = localtime(&now);
	}
	if (clock_is_24h_style()) {
		strftime(time_text, sizeof(time_text), "%R", tick_time);
	} else {
		strftime(time_text, sizeof(time_text), "%I:%M", tick_time);
	}
	text_layer_set_text(time_text_layer, time_text);
	
	strftime(day_text, sizeof(day_text), "%A", tick_time);//%H:%M
	text_layer_set_text(day_text_layer, day_text);
	
	strftime(date_text, sizeof(date_text), "%B %e", tick_time);//%H:%M
	text_layer_set_text(date_text_layer, date_text);
}

void deinit() {
	tick_timer_service_unsubscribe();
	gbitmap_destroy(image);
	fonts_unload_custom_font(font);
	layer_destroy(background_layer);
	window_destroy(window);
}

void init() {
	//resource_init_current_app(&APP_RESOURCES);
	window = window_create();
	window_stack_push(window, false);

	image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);

	Layer* window_layer = window_get_root_layer(window);

	GRect bounds = layer_get_frame(window_layer);
	background_layer = layer_create(bounds);
	layer_set_update_proc(background_layer, background_layer_update);
	layer_add_child(window_layer, background_layer);

	time_text_layer = text_layer_create(GRect(0, 73, 143, 60));
	text_layer_set_text_alignment(time_text_layer, GTextAlignmentRight);
	text_layer_set_text_color(time_text_layer, GColorWhite);
	text_layer_set_background_color(time_text_layer, GColorClear);

	day_text_layer = text_layer_create(GRect(0, 10, 140, 30));
	text_layer_set_text_alignment(day_text_layer, GTextAlignmentRight);
	text_layer_set_text_color(day_text_layer, GColorWhite);
	text_layer_set_background_color(day_text_layer, GColorClear);
	
	date_text_layer = text_layer_create(GRect(0, 40, 140, 30));
	text_layer_set_text_alignment(date_text_layer, GTextAlignmentRight);
	text_layer_set_text_color(date_text_layer, GColorWhite);
	text_layer_set_background_color(date_text_layer, GColorClear);
	
	font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_55));
	text_layer_set_font(time_text_layer, font);
	
	font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_25));
	text_layer_set_font(day_text_layer, font_small);
	text_layer_set_font(date_text_layer, font_small);
	//text_layer_set_font(time_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(window_layer, text_layer_get_layer(time_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(day_text_layer));
	layer_add_child(window_layer, text_layer_get_layer(date_text_layer));

	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	handle_minute_tick(NULL, MINUTE_UNIT);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
