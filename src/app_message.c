#include <pebble.h>

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 4
#define NUM_NEARBY_MENU_SECTIONS 1
#define MAX_SUPPORTED_BUSES 20
	
typedef struct {
  char* bus_route;
	char* stop;
	char* direction;
	char* arrival_time;
} BusInfo;

int num_nearby_buses = 0;
bool got_all_buses = false;

BusInfo* nearby_buses;
	
Window *window;
Window* nearby_buses_window;
Window* bus_info_window;

//window layers to place UI stuff on
Layer *window_layer;
Layer* nearby_buses_window_layer;
Layer* bus_info_window_layer;

TextLayer *text_layer;


//--the UI stuff that the user actually interacts with--
static MenuLayer *menu_layer;
static MenuLayer *nearby_menu_layer;

//all text layers used for displaying 
static TextLayer* route_text_layer;
static TextLayer* stop_text_layer;
static TextLayer* direction_text_layer;
static TextLayer* arrival_text_layer;
static TextLayer* direction_text_layer;
static TextLayer* arrival_text_layer;
//static TextLayer* second_arrival_text_layer;


//------------------------Endu UI stuff--------------------------


//iterator used for populating the nearby_buses array
char* msg;


static char* js_msg_recieved = "hold on..."; // default message on load
	
// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,
	DATA_KEY,
	DEBUG_KEY
};



//-----------------Function Callbacks-----------------


// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}
static uint16_t nearby_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_NEARBY_MENU_SECTIONS;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  if (section_index == 0) {
      return NUM_FIRST_MENU_ITEMS;
  }
	else{
		return 0;
	}
}

static uint16_t nearby_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	if (section_index == 0){
		return num_nearby_buses;
	}
	return 0;
}



// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t nearby_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  if (section_index == 0) {
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "PebbleNextbus");
  }
}

static void nearby_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  if(section_index == 0) {
      menu_cell_basic_header_draw(ctx, cell_layer, "Nearby Buses");
  }
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  if(cell_index->section == 0) {
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          // This is a basic menu item with a title and subtitle - ""
          menu_cell_basic_draw(ctx, cell_layer, "Nearby Buses", "", NULL);
          break;
				case 1:
		  		menu_cell_basic_draw(ctx, cell_layer, "Select Stop", "From a predefined list", NULL);
		  		break;
				case 2:
		  		menu_cell_basic_draw(ctx, cell_layer, "Favorite Stops", "", NULL);
		  		break;
				case 3:
		  		menu_cell_basic_draw(ctx, cell_layer, "Information", "", NULL);
		  		break;
			}
  }
}

static void nearby_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	
	// Determine which section we're going to draw in
  if (cell_index->section == 0) {
    
		// Use the row to specify which item we'll draw
		int arr_pos = cell_index->row;
		
		
		//if bus was not init, then don't do anything
		if(strcmp(nearby_buses[arr_pos].bus_route,"")==0){
			menu_cell_basic_draw(ctx, cell_layer, "where most info goes", "where stop goes", NULL);
			return;
		}
	
		APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "b4 doing title stuff");
		char title[170];
		
		APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n",nearby_buses[arr_pos].bus_route );
		strcpy(title, nearby_buses[arr_pos].bus_route);
		//strcat(title, " ");
		//strcat(title, nearby_buses[arr_pos].stop);
		strcat(title, "   ");
				APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n",nearby_buses[arr_pos].arrival_time);

		strcat(title, nearby_buses[arr_pos].arrival_time);

		APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "after doing title stuff, b4 drawing");
		
		//menu_cell_basic_draw(ctx, cell_layer, "where most info goes", "where stop goes", NULL);
			//return;
    menu_cell_basic_draw(ctx, cell_layer, title, nearby_buses[arr_pos].stop, NULL);
//		free(title);

  }
}


// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	
  // Use the row to specify which item will receive the select action
	
	//need to get the bus options, then load them into the nearby_menu_layer
	//this is possible b/c it gets drawn only after it is added to the window
	//get_buses_from_server();

	if(cell_index->section == 0) {
		switch(cell_index->row){
			case 0:
				APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "b4 hide menulayer");

				//hide the main menu layer
				layer_set_hidden((Layer *)menu_layer, true);
				APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "after hide menulayer, b4 push window");

				//show the nearby buses
				window_stack_push(nearby_buses_window, true);
				break;
			
			default:
				break;
		}
	}
		
}

//Show all of the Bus info on the screen
void load_bus_info(int index){
	
	BusInfo* b_info = &(nearby_buses[index]);
	
	//if the bus wasn't set, then do nothing
	if(strcmp(b_info->bus_route,"") == 0){
		return;
	}
	
	//hide nearby menu list
	layer_set_hidden((Layer *)nearby_menu_layer, true);
	
		route_text_layer = text_layer_create(GRect(0,0,144,50));
//	text_layer_set_background_color(route_text_layer, GColorClear);
//  text_layer_set_text_color(route_text_layer, GColorBlack);
	text_layer_set_text_alignment(route_text_layer, GTextAlignmentCenter);
	
	//route layer
	text_layer_set_font(route_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text(route_text_layer, b_info->bus_route);
	
	//stop layer
	stop_text_layer = text_layer_create(GRect(0, 50, 144, 20));
	text_layer_set_text(stop_text_layer, b_info->stop);
					
	//direction layer
	direction_text_layer = text_layer_create(GRect(0, 70, 144, 20));
	text_layer_set_text(direction_text_layer, b_info->direction);
	
	//arrival time
	arrival_text_layer = text_layer_create(GRect(0, 90, 144, 70));
	text_layer_set_font(arrival_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(arrival_text_layer, GTextAlignmentCenter);
	text_layer_set_text(arrival_text_layer, b_info->arrival_time);																							 
		
	
	
	//push bus_info_window on stack
	window_stack_push(bus_info_window, true);
}

void nearby_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
	load_bus_info(cell_index->row);
}




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
	
	if (rec_debug) {
		msg = malloc(strlen(rec_debug->value->cstring)*sizeof(char));
		strcpy(msg, rec_debug->value->cstring);
		
	} else if (rec_data) {
		msg = malloc(strlen(rec_data->value->cstring)*sizeof(char));
		strcpy(msg, rec_data->value->cstring);
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", msg);
	
	if(strcmp(msg, "done")==0){
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



//-------------------Window loading and unloading-------------------------

// This initializes the menu upon window load
void window_load(Window *window) {
	
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);
	text_layer = text_layer_create(bounds);
	

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });


  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}


void nearby_buses_window_load(Window *window) {
	
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load1");
	nearby_buses_window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(nearby_buses_window_layer);
	
	nearby_menu_layer = menu_layer_create(bounds);
		APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load2");

	menu_layer_set_callbacks(nearby_menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = nearby_menu_get_num_sections_callback,
    .get_num_rows = nearby_menu_get_num_rows_callback,
    .get_header_height = nearby_menu_get_header_height_callback,
    .draw_header = nearby_menu_draw_header_callback,
    .draw_row = nearby_menu_draw_row_callback,
    .select_click = nearby_menu_select_callback,
	});
		APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load3");

	// Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(nearby_menu_layer, window);

		APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load4");

  // Add it to the window for display
  layer_add_child(nearby_buses_window_layer, menu_layer_get_layer(nearby_menu_layer));
	
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load5");
}


	
void bus_info_window_load(Window *window) {
	bus_info_window_layer = window_get_root_layer(window);
	
	
	//add all layers to screen
	layer_add_child(bus_info_window_layer, text_layer_get_layer(route_text_layer));
	layer_add_child(bus_info_window_layer, text_layer_get_layer(stop_text_layer));
	layer_add_child(bus_info_window_layer, text_layer_get_layer(direction_text_layer));
	layer_add_child(bus_info_window_layer, text_layer_get_layer(arrival_text_layer));
//	layer_add_child(window_layer, text_layer_get_layer(second_arrival_text_layer));	
}


void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
	text_layer_destroy(text_layer);
}

void nearby_buses_window_unload(){
	menu_layer_destroy(nearby_menu_layer);
	//layer_mark_dirty(menu_layer_get_layer(menu_layer));
		
	window_stack_pop(window);
	window_stack_push(window, true);
}

void bus_info_window_unload(){
	text_layer_destroy(route_text_layer);
	text_layer_destroy(stop_text_layer);
	text_layer_destroy(direction_text_layer);
	text_layer_destroy(arrival_text_layer);
	//text_layer_destroy(second_arrival_text_layer);
	
	//layer_mark_dirty(menu_layer_get_layer(nearby_menu_layer));
	
	window_stack_pop(nearby_buses_window);
	window_stack_push(nearby_buses_window, true);
	
}



//----------------Startup and main/managerial stuff----------------------

void init(void) {
	
	nearby_buses = malloc(sizeof(BusInfo)*MAX_SUPPORTED_BUSES);
	
	window = window_create();
	nearby_buses_window = window_create();
	bus_info_window = window_create();
	
	
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
	
	//layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

void deinit(void) {
	
	//check that we don't need to free individuals inside
	free(nearby_buses);
	
	app_message_deregister_callbacks();
	
	text_layer_destroy(text_layer);
	
	window_destroy(bus_info_window);
	window_destroy(nearby_buses_window);
	window_destroy(window);

	
	
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}