#include <pebble.h>
#include "main.h"

Window *window;
Layer *windowLayer;
GFont numbersFont, infoFont;
TextLayer *infoLayer;
GRect bounds;
GColor palette[2];
GColor bgColor;

const int imgIds[10] = {
  RESOURCE_ID_IMG0, RESOURCE_ID_IMG1, RESOURCE_ID_IMG2, RESOURCE_ID_IMG3, RESOURCE_ID_IMG4, RESOURCE_ID_IMG5,
  RESOURCE_ID_IMG6, RESOURCE_ID_IMG7, RESOURCE_ID_IMG8, RESOURCE_ID_IMG9 };
static GBitmap *images[10];


static void doNothing() {
}

//settings *********************************************************************
void createDefaultSettings() {
  settings.showHealthAsDefault = false;
  PBL_IF_COLOR_ELSE(strcpy(settings.ColorScheme, "CGA"), strcpy(settings.ColorScheme, "MON"));
}

void applySettings() {      
  //MON, DEFAULT
  bgColor = GColorBlack;
  GColor fgColor = GColorWhite;
  GColor infoColor = fgColor;
  
  if (strcmp(settings.ColorScheme, "CGA")==0) {
    fgColor = GColorCyan;
    infoColor = GColorShockingPink;
  } else if (strcmp(settings.ColorScheme, "GRE")==0) {
    fgColor = GColorMalachite;
    infoColor = fgColor;
  } else if (strcmp(settings.ColorScheme, "AMB")==0) {
    fgColor = GColorChromeYellow;
    infoColor = fgColor;
  } else if (strcmp(settings.ColorScheme, "C64")==0) {
    bgColor = GColorBlue;
    fgColor = GColorWhite;
    infoColor = fgColor;
  } else if (strcmp(settings.ColorScheme, "GAB")==0) {
    bgColor = GColorSpringBud;
    fgColor = GColorMidnightGreen;
    infoColor = fgColor;
  } 
  
  palette[0] = bgColor;
  window_set_background_color(window, bgColor);
  
  palette[1] = fgColor;
  text_layer_set_text_color(infoLayer, infoColor);
}

void loadSettings() {
  createDefaultSettings();
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
  applySettings();
}

void saveSettings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  applySettings();
  
  if (settings.showHealthAsDefault) {
    updateInfoHealth();    
  } else {
    updateInfoDate();
  }
}

static void inboxReceivedHandler(DictionaryIterator *iter, void *context) {
  
  Tuple *colors_t = dict_find(iter, MESSAGE_KEY_ColorScheme);
  if (colors_t) {
    strcpy(settings.ColorScheme, colors_t->value->cstring);
  }
    
  Tuple *showHealth_t = dict_find(iter, MESSAGE_KEY_ShowHealthDefault);
  if(showHealth_t) {
    settings.showHealthAsDefault = showHealth_t->value->int32 == 1;
  }
  
  saveSettings();
}

static void drawNumber(GContext* ctx, int number, int quadrant) {
  
#ifdef PBL_ROUND
  int top=5, left=10;
  switch(quadrant) {
  	case 1: top=-1; left+=1; break;
  	case 2: top=-1; left=bounds.size.w/2+1; break; //144/2 = 72, so we can go 2px right
  	case 3: top=71; left+=1; break;
    case 4: top=71; left=bounds.size.w/2+1; break;
  	default: break;
  }
  graphics_draw_bitmap_in_rect(ctx, images[number], GRect(left, top, 70, 70));
#else
  int top=0, left=0;
  switch(quadrant) {
  	case 1: top=-1; left=1; break;
  	case 2: top=-1; left=bounds.size.w/2+1; break; //144/2 = 72, so we can go 2px right
  	case 3: top=71; left=1; break;
    case 4: top=71; left=bounds.size.w/2+1; break;
  	default: break;
  }
  graphics_draw_bitmap_in_rect(ctx, images[number], GRect(left, top, 70, 70));
#endif
      
             
}

static void updateWindowLayer(struct Layer *layer, GContext* ctx) {
  //paint background
  graphics_context_set_fill_color(ctx, bgColor);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  
  //paint numbers
  time_t now = time(NULL);
  struct tm *tmNow = localtime(&now);
  int h = tmNow->tm_hour;
  if(!clock_is_24h_style()) {
    if (h==0) {
      h = 12;
    } else if (h>12) {
      h -= 12;
    }
  }
  int m = tmNow->tm_min;  
  
  drawNumber(ctx, h/10, 1);
  drawNumber(ctx, h%10, 2);
  drawNumber(ctx, m/10, 3);
  drawNumber(ctx, m%10, 4);  
}

void updateInfoHealth() {
  int steps=0;
  int sleep=0;
  
  //health steps
  static char stepStr[] = "12375 st.";
  time_t start = time_start_of_today();
  time_t end = time(NULL);
  HealthMetric metricStep = HealthMetricStepCount;    
  HealthServiceAccessibilityMask maskStep = health_service_metric_accessible(metricStep, start, end);    
  if(maskStep & HealthServiceAccessibilityMaskAvailable) {
    steps = (int)health_service_sum_today(metricStep);
  }
  //sleep
  static char sleepStr[] = "10:34 hoursUnit";
  HealthMetric metricSleep = HealthMetricSleepSeconds;
  HealthServiceAccessibilityMask maskSleep = health_service_metric_accessible(metricSleep, start, end);    
  if(maskSleep & HealthServiceAccessibilityMaskAvailable) {
    sleep = (int)health_service_sum_today(metricSleep);
  }
  int sleepDigit = (sleep%3600)/60;
  sleep = sleep/3600;  

  //render steps (>2000) or sleep
  if (sleep == 0 || steps > 2000) {
    if (steps<10000) {
      snprintf(stepStr, sizeof(stepStr), "%d %s", steps, "S.");
    } else {
      snprintf(stepStr, sizeof(stepStr), "%d", steps);
    }    
    text_layer_set_text(infoLayer, stepStr);
  } else {
    if (sleepDigit<10) {
      snprintf(sleepStr, sizeof(sleepStr), "%d:0%d %s", sleep, sleepDigit, "H.");
    } else {
      snprintf(sleepStr, sizeof(sleepStr), "%d:%d %s", sleep, sleepDigit, "H.");
    }
    text_layer_set_text(infoLayer, sleepStr);
  }
}

void updateInfoDate() {
  time_t now = time(NULL);
  struct tm *tmNow = localtime(&now);
  static char dateStr[] = "MON 12";
  strftime(dateStr, sizeof(dateStr), "%a %d", tmNow);
  text_layer_set_text(infoLayer,dateStr);
}

static void tapHandler(AccelAxisType axis, int32_t direction) {
  if (settings.showHealthAsDefault) {
    updateInfoDate();
    app_timer_register(3000, updateInfoHealth, NULL);
  } else {
    updateInfoHealth();
    app_timer_register(3000, updateInfoDate, NULL);
  }
}

static void tickHandler(struct tm *tickTime, TimeUnits unitsChanged) {      
  if (settings.showHealthAsDefault) {
    updateInfoHealth();    
  } else {
    updateInfoDate();
  }
  
  //update time
  layer_mark_dirty(windowLayer); 
}

void init() {
  //load resources
  infoFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SILK_36));
  for (int number=0;number<10;number++) {
    images[number] = gbitmap_create_with_resource(imgIds[number]);
    gbitmap_set_palette(images[number], palette, true);
  }
    
  //create window + layers
  window = window_create();
  window_stack_push(window, false); //not animated  
  window_set_background_color(window, GColorBlack);
  
  windowLayer = window_get_root_layer(window);
  bounds = layer_get_bounds(windowLayer);
  layer_set_update_proc(windowLayer, updateWindowLayer); 
 
  infoLayer = text_layer_create(GRect(0, 130, bounds.size.w, 40));
  text_layer_set_background_color(infoLayer, GColorClear);
  text_layer_set_text_alignment(infoLayer, GTextAlignmentCenter);
  text_layer_set_font(infoLayer, infoFont);  
  layer_add_child(windowLayer, text_layer_get_layer(infoLayer));
    
  //register handlers
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tickHandler);
  PBL_IF_HEALTH_ELSE(accel_tap_service_subscribe(tapHandler),doNothing());
  
  //get settings
  loadSettings();
  app_message_register_inbox_received(inboxReceivedHandler);
  app_message_open(128, 128);
    
  //initial refresh 
  //(avoid delay before first timer event)
  layer_mark_dirty(windowLayer);
  if (settings.showHealthAsDefault) {
    updateInfoHealth();    
  } else {
    updateInfoDate();
  }
}

void shutdown () {
  
  tick_timer_service_unsubscribe();
  PBL_IF_HEALTH_ELSE(accel_tap_service_unsubscribe(),doNothing());
  text_layer_destroy(infoLayer);
  app_message_deregister_callbacks();
  
  //don't destroy images, for some reason this crashes the app
  /*for (int number=0;number<10;number++) {
    gbitmap_destroy(images[number]);
  }*/
}

int main() {
  init();
  app_event_loop();
  shutdown();  
  return 0;
}