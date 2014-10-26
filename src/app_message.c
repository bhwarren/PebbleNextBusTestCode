#include <pebble.h>

#define NUM_NEARBY_BUSES 5	
	
Window *window;
Layer *window_layer;
TextLayer *text_layer;

static char* js_msg_recieved = "hold on..."; // default message on load
	
// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,
	DATA_KEY,
	DEBUG_KEY
};


// Write message to buffer & send
void send_ack_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, 0x1);
	
	dict_write_end(iter);
  	app_message_outbox_send();
}

// Write message to buffer & send
void send_nack_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, 0x2);
	
	dict_write_end(iter);
  	app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *rec_debug = dict_find(received, DEBUG_KEY);
	Tuple *rec_data = dict_find(received, DATA_KEY);
	
	if (rec_debug) {
		text_layer_set_text(text_layer, rec_debug->value->cstring);
		layer_mark_dirty(window_layer);
	} else if (rec_data) {
		text_layer_set_text(text_layer, rec_data->value->cstring);
		layer_mark_dirty(window_layer);
	}
	
	send_ack_message();
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {
	//send_nack_message();
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	//send_nack_message();
}

void init(void) {
	window = window_create();
	window_stack_push(window, true);
	
	// initialize the only TextLayer
	window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	text_layer = text_layer_create((GRect){ .origin = { 0, 30 }, .size = bounds.size });
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	send_ack_message();
	
	// set TextLayer properties
	text_layer_set_text(text_layer, js_msg_recieved);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

void deinit(void) {
	app_message_deregister_callbacks();
	
	text_layer_destroy(text_layer);
	
	window_destroy(window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}