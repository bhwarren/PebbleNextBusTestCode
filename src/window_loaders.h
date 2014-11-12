#include <pebble.h>
#include "callbacks.h"
	

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

BusInfo* nearby_buses;

int num_nearby_buses;

	
Window* window;
Window* nearby_buses_window;
Window* bus_info_window;
//----Main menu windows-----
Window* fav_stops_window;
Window* selected_stops_window;
Window* info_window;

//window layers to place UI stuff on
Layer* window_layer;
Layer* nearby_buses_window_layer;
Layer* bus_info_window_layer;
//------Main menu layers-----
Layer* info_window_layer;



//--the UI stuff that the user actually interacts with--
MenuLayer* menu_layer;
MenuLayer* nearby_menu_layer;

//all text layers used for displaying 
TextLayer* route_text_layer;
TextLayer* stop_text_layer;
TextLayer* direction_text_layer;
TextLayer* arrival_text_layer;
//static TextLayer* second_arrival_text_layer;


//-----------------------function defs--------------------------------

void window_load(Window *window);
void nearby_buses_window_load(Window *window);
void bus_info_window_load(Window *window);
void info_window_load();


void window_unload(Window* window);
void nearby_buses_window_unload();
void bus_info_window_unload();
void info_window_unload();