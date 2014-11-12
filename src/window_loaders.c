#include "window_loaders.h"
	

//-------------------Window loading and unloading-------------------------

// This initializes the menu upon window load
void window_load(Window *window) {
	//this is here to keep the buses on the screen when coming back from info
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Start LOADING first window");

	menu_layer_destroy(nearby_menu_layer); 
	

	
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);
	//text_layer = text_layer_create(bounds);
	

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
	
			APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "finished LOADING first window");

}


void nearby_buses_window_load(Window *window) {
	
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Start LOADING nearby window");

	
	//APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load1");
	nearby_buses_window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(nearby_buses_window_layer);
	
	nearby_menu_layer = menu_layer_create(bounds);
		//APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load2");

	menu_layer_set_callbacks(nearby_menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = nearby_menu_get_num_sections_callback,
    .get_num_rows = nearby_menu_get_num_rows_callback,
    .get_header_height = nearby_menu_get_header_height_callback,
    .draw_header = nearby_menu_draw_header_callback,
    .draw_row = nearby_menu_draw_row_callback,
    .select_click = nearby_menu_select_callback,
	});
		//APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load3");

	// Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(nearby_menu_layer, window);

		//APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load4");

  // Add it to the window for display
  layer_add_child(nearby_buses_window_layer, menu_layer_get_layer(nearby_menu_layer));
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "window load5");
			APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "finished LOADING nearby window");

}


	
void bus_info_window_load(Window *window) {
	
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Start LOADING info window");

	
	bus_info_window_layer = window_get_root_layer(window);

	//add all layers to screen
	layer_add_child(bus_info_window_layer, text_layer_get_layer(route_text_layer));
	layer_add_child(bus_info_window_layer, text_layer_get_layer(stop_text_layer));
	layer_add_child(bus_info_window_layer, text_layer_get_layer(direction_text_layer));
	layer_add_child(bus_info_window_layer, text_layer_get_layer(arrival_text_layer));
//	layer_add_child(window_layer, text_layer_get_layer(second_arrival_text_layer));	
	
	
		APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "finished LOADING info window");

}

void info_window_load(){
	
}


void window_unload(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Start Unloading first window");

  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
	//text_layer_destroy(text_layer);
	
	
	
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Finished Unloading first window");

}

void nearby_buses_window_unload(){
	layer_mark_dirty(window_layer);

	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Start Unloading nearby window");
	
	layer_mark_dirty(menu_layer_get_layer(menu_layer));

	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Finished Unloading nearby window");

	//pop and push back previous window to make sure there arent' 
	//any blank menus
	window_stack_pop(false);
	window_stack_push(window, true);
}

void bus_info_window_unload(){
	layer_mark_dirty(nearby_buses_window_layer);


	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Start Unloading bus info window");

	text_layer_destroy(route_text_layer);
	text_layer_destroy(stop_text_layer);
	text_layer_destroy(direction_text_layer);
	text_layer_destroy(arrival_text_layer);
	//text_layer_destroy(second_arrival_text_layer);
		
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Middle Unloading bus info window");

	

	
	/*if(!window_is_loaded(nearby_buses_window)){
		window_stack_push(nearby_buses_window, true);
	}	*/
		
	APP_LOG(APP_LOG_LEVEL_DEBUG,"%s\n", "Finished Unloading bus info window");

	//pop and push back previous window to make sure there arent' 
	//any blank menus
	window_stack_pop(false);
	window_stack_push(nearby_buses_window, true);
}

void info_window_unload(){
	
}
