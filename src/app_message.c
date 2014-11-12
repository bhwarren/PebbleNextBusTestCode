#include "window_loaders.h"

//flag to tell when done getting all buses
bool got_all_buses = false;

//temp var to store messages from the phone
char* msg;

// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,
	DATA_KEY,
	DEBUG_KEY
};


//-----------------------Message Callbacks ----------------------------------


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
	
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "got msg");
	
	if(got_all_buses){
		send_ack_message();
		return;
	}
	
	if (rec_debug) {
		msg = malloc(strlen(rec_debug->value->cstring)*sizeof(char));
		strcpy(msg, rec_debug->value->cstring);
		
	} else if (rec_data) {
		msg = malloc(strlen(rec_data->value->cstring)*sizeof(char));
		strcpy(msg, rec_data->value->cstring);
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", msg);
	
	if(strcmp(msg, "done")==0){
		free(msg);
		got_all_buses = true;
		send_ack_message();
		return;
	}
	
	int last_nl = 0;
	int section = 0;
	int i;
	for(i=0;i<(int)strlen(msg); i++){
		
		//when find a newline, grab stuff between nls and put it in the array
		if(msg[i]!='\n'){
			continue;
		}
		
		switch(section){
			//define route section
			case 0:
				nearby_buses[num_nearby_buses].bus_route = malloc((i-last_nl+1)*sizeof(char));
				strncpy(nearby_buses[num_nearby_buses].bus_route, msg+last_nl, i-last_nl);
				nearby_buses[num_nearby_buses].bus_route[(i-last_nl)] = '\0';
				break;
			//define direction section
			case 1:
				nearby_buses[num_nearby_buses].direction = malloc((i-last_nl+1)*sizeof(char));
				strncpy(nearby_buses[num_nearby_buses].direction, msg+last_nl+1, i-last_nl);
				nearby_buses[num_nearby_buses].direction[(i-last_nl)] = '\0';
				break;
			//define stop section
			case 2:
				nearby_buses[num_nearby_buses].stop = malloc((i-last_nl+1)*sizeof(char));
				strncpy(nearby_buses[num_nearby_buses].stop, msg+last_nl+1, i-last_nl);
				nearby_buses[num_nearby_buses].stop[(i-last_nl)] = '\0';
				break;
			//define arrival time section
			case 3:
				nearby_buses[num_nearby_buses].arrival_time = malloc((i-last_nl+1)*sizeof(char));
				strncpy(nearby_buses[num_nearby_buses].arrival_time, msg+last_nl+1, i-last_nl);
				nearby_buses[num_nearby_buses].arrival_time[(i-last_nl)] = '\0';
				break;
			
		}
		section++;
		last_nl=i;
		
	}
	
	free(msg);
	num_nearby_buses++;
		//APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "after getting msg");
	//layer_add_child(window_layer, text_layer_get_layer(text_layer));
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





//----------------Startup and main/managerial stuff----------------------

void init(void) {
	num_nearby_buses = 0;
	
	nearby_buses = malloc(sizeof(BusInfo)*MAX_SUPPORTED_BUSES);

	window = window_create();
	nearby_buses_window = window_create();
	bus_info_window = window_create();
	info_window = window_create();
	
	
	// Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
	window_set_window_handlers(nearby_buses_window, (WindowHandlers){
		.load = nearby_buses_window_load,
		.unload = nearby_buses_window_unload,
	});
	window_set_window_handlers(bus_info_window, (WindowHandlers){
		.load = bus_info_window_load,
		.unload = bus_info_window_unload,
	});

	
	
	window_stack_push(window, true);
	
	// initialize the only TextLayer
	window_layer = window_get_root_layer(window);
	//GRect bounds = layer_get_frame(window_layer);
	//text_layer = text_layer_create((GRect){ .origin = { 0, 30 }, .size = bounds.size });
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	send_ack_message();

}

void deinit(void) {
	
	
	//check that we don't need to free individuals inside
			APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "In de-init, after free");

	
	app_message_deregister_callbacks();
	
	//text_layer_destroy(text_layer);
	
	window_destroy(bus_info_window);
	window_destroy(nearby_buses_window);
	window_destroy(window);
	window_destroy(info_window);

	int i;
	for(i=0; i<MAX_SUPPORTED_BUSES; i++){
		free(nearby_buses[i].bus_route);
		free(nearby_buses[i].direction);
		free(nearby_buses[i].stop);
		free(nearby_buses[i].arrival_time);
	}
	free(nearby_buses);
	
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}