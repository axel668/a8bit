#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1

// Settings structure
typedef struct ClaySettings {
  bool showHealthAsDefault;
  char ColorScheme[4];
} __attribute__((__packed__)) ClaySettings;
ClaySettings settings;

void createDefaultSettings();
void loadSettings();
void saveSettings();
void updateInfoHealth();
void updateInfoDate();