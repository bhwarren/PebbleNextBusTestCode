#include "window_loaders.h"

//-----------------Function Callbacks-----------------


// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}
uint16_t nearby_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_NEARBY_MENU_SECTIONS;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  if (section_index == 0) {
      return NUM_FIRST_MENU_ITEMS;
  }
	else{
		return 0;
	}
}

uint16_t nearby_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	if (section_index == 0){
		return num_nearby_buses;
	}
	return 0;
}



// A callback is used to specify the height of the section header
int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

int16_t nearby_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  if (section_index == 0) {
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "PebbleNextbus");
  }
}

void nearby_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  if(section_index == 0) {
      menu_cell_basic_header_draw(ctx, cell_layer, "Nearby Buses");
  }
}

// This is the menu item draw callback where you specify what each item should look like
void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
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

void nearby_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	
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
		strcat(title, "   ");
				APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n",nearby_buses[arr_pos].arrival_time);
		strcat(title, nearby_buses[arr_pos].arrival_time);

		APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "after doing title stuff, b4 drawing");
		
		//draw the row
    menu_cell_basic_draw(ctx, cell_layer, title, nearby_buses[arr_pos].stop, NULL);

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
				//layer_set_hidden((Layer *)menu_layer, true);
				APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "after hide menulayer, b4 push window");

				window_stack_push(nearby_buses_window, true);
				if(num_nearby_buses<0){
					layer_set_hidden(text_layer_get_layer(loading_text_layer), false);
				}

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