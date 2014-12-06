#include <pebble.h>
#include "jmuWatch.h"
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_WEATHID 2
static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static GFont s_day_font;
static GFont s_date_font;
static GFont s_month_font;
static GFont s_weather_font;
static GFont s_temp_font;
static TextLayer *s_day_layer;
static TextLayer *s_date_layer;
static TextLayer *s_month_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_temp_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static GBitmap *s_weather_bitmap;
static BitmapLayer *s_weathbmp_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char monthBuf[]="Long date to be safe !";
  static char dateBuf[]="32nd";
  static char dayBuf[]="Wednesday or More";
  static char tempBuf[]="-273.15°C";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  
  // ecrire la date dans le buffer
  strftime(dayBuf, sizeof("Wednesday or more"), "%A", tick_time);
  text_layer_set_text(s_day_layer, dayBuf);
  strftime(monthBuf, sizeof("Long date to be safe !"), "%B", tick_time);
  text_layer_set_text(s_month_layer, monthBuf);
  strftime(dateBuf, sizeof("-273.15°C"), "%d", tick_time);
  text_layer_set_text(s_date_layer, dateBuf);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // MaJ meteo toutes les 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Ajout dictionnaire
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    // Ajout d'une paire cle-valeur
    dict_write_uint8(iter, 0, 0);
    
    // Envoi du message
    app_message_outbox_send();
  }
}

static void main_window_load(Window *window) {
  // creation bitmap layer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TOF_IMAGE);
  s_background_layer = bitmap_layer_create(GRect(0, 56, 144, 112));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // creation text layer
  s_time_layer = text_layer_create(GRect(0, 0, 144, 60));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  //text_layer_set_text(s_time_layer, "12:34");
  
  //police 
  //text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  // Create GFont
  //s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
  s_time_font = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  //ajout du texte à la fenetre
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_time_layer));
  
  // ajout jour
  s_day_layer = text_layer_create(GRect(5,0,139,22));
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  s_day_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(s_day_layer, s_day_font);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentLeft);
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_day_layer));

  // ajout mois
  s_month_layer = text_layer_create(GRect(5,48,95,30));
  text_layer_set_background_color(s_month_layer, GColorClear);
  text_layer_set_text_color(s_month_layer, GColorWhite);
  s_month_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  text_layer_set_font(s_month_layer, s_month_font);
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_month_layer));
  
  // ajout date
  s_date_layer = text_layer_create(GRect(5,48,139,38));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  s_date_font = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_date_layer));
  
  // ajout meteo
  s_weather_layer = text_layer_create(GRect(5, 138, 139, 24));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentLeft);
  text_layer_set_text(s_weather_layer, "La meteo charge ...");
  s_weather_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(s_weather_layer, s_weather_font);
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_weather_layer));
  
  // creation bitmap meteo
  s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TORNADO);
  s_weathbmp_layer = bitmap_layer_create(GRect(0, 88, 50, 50));
  bitmap_layer_set_bitmap(s_weathbmp_layer, s_weather_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_weathbmp_layer));

  // ajout temperature
  s_temp_layer = text_layer_create(GRect(5, 88, 139, 34));
  text_layer_set_background_color(s_temp_layer, GColorClear);
  text_layer_set_text_color(s_temp_layer, GColorWhite);
  text_layer_set_text_alignment(s_temp_layer, GTextAlignmentRight);
  text_layer_set_text(s_temp_layer, ".");
  s_temp_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  text_layer_set_font(s_temp_layer, s_temp_font);
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_temp_layer));


}

static void main_window_unload(Window *window) {
  // destruction couche meteo
  text_layer_destroy(s_weather_layer);
  // destruction couche temperature
  text_layer_destroy(s_temp_layer);
  //destruction couche image meteo
  gbitmap_destroy(s_weather_bitmap);
  bitmap_layer_destroy(s_weathbmp_layer);


  // destruction couche jour 
  text_layer_destroy(s_day_layer);
  // destruction couche mois
  text_layer_destroy(s_month_layer);
  // destruction couche date
  text_layer_destroy(s_date_layer);
  // destruction couche texte
  text_layer_destroy(s_time_layer);
  //destruction couche image
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  // Unload GFonts - Not needed for system fonts
  //fonts_unload_custom_font(s_time_font); 
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context){
  // Stockage pour meteo recue
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[64];
  
  // Lecture premier item
  Tuple *t = dict_read_first(iterator);
  
  // boucle sur les items
  while(t != NULL) {
    // Quelle cle a ete recue ?
    switch(t->key) {
      case KEY_TEMPERATURE:
        snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", (int)t->value->int32);
        break;
      case KEY_WEATHID:  
        s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TORNADO);
        // cf. http://openweathermap.org/weather-conditions
        if( ((int)t->value->int32 >=200) && ((int)t->value->int32 <300)){ // thunderstorm
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_STORM);
        }
        if( (int)t->value->int32 ==211 ) { // thuderstorm
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLOUD_LIGHTING);
        }
        if( ((int)t->value->int32 >=300) && ((int)t->value->int32 <400)){ // drizzle
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LITTLE_RAIN);
        }
        if( (int)t->value->int32 ==500 ) { // light rain
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LITTLE_RAIN);
        }
        if( ((int)t->value->int32 >=500) && ((int)t->value->int32 <600)){ // rain
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_RAIN);
        }
        if( (int)t->value->int32 ==600 ) { // light snow
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LIGHT_SNOW);
        }
        if( ((int)t->value->int32 >600) && ((int)t->value->int32 <700)){ // snow
            APP_LOG(APP_LOG_LEVEL_INFO, "WeathID: SNOW ");
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOW);
        }
        if( ((int)t->value->int32 ==611) || ((int)t->value->int32 == 612) ) { // sleet
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SLEET);
        }
        if( ((int)t->value->int32 >=700) && ((int)t->value->int32 <800)){ // atmosphere
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DUST);
        }
        if( (int)t->value->int32 ==701 ) { // fog
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FOG_DAY);
        }
        if( (int)t->value->int32 ==741 ) { // fog
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FOG_DAY);
        }
        if( (int)t->value->int32 ==781 ) { // fog
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TORNADO);
        }
        if( (int)t->value->int32 ==800 ) { // clear sky
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SUN);
        }
        if( ((int)t->value->int32 >800) && ((int)t->value->int32 <900)){ // clouds
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLOUDS);
        }
        if( (int)t->value->int32 ==900 ) { // tornade
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TORNADO);
        }
        if( (int)t->value->int32 ==906 ){ // tornade
            s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_HAIL);
        }
        bitmap_layer_set_bitmap(s_weathbmp_layer, s_weather_bitmap);
        break;
      case KEY_CONDITIONS:
        snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Cle %d non reconnue !", (int)t->key);
        break;
    }
    
    // recherche du prochain item
    t = dict_read_next(iterator);
  }
  // Assemblage chaine finale et affichage
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", conditions_buffer, temperature_buffer);
  text_layer_set_text(s_weather_layer, conditions_buffer);
  text_layer_set_text(s_temp_layer, temperature_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context){
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context){
  APP_LOG(APP_LOG_LEVEL_ERROR, "outbox send failed!");
}
static void outbox_send_callback(DictionaryIterator *iterator, void * context){
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  
  // enregistrement des callbacks meteo
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_send_callback);
  
  // ouverture de appmessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // creation fenetre
  s_main_window = window_create();
  
  // definition des handlers
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // affichage
  window_stack_push(s_main_window, true);
  update_time();
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}