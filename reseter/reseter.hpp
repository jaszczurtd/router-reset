
#pragma once

#include <Arduino.h>
#include <WiFi.h>    
#include <tools.h>
#include <Credentials.h>

#define ON HIGH
#define OFF LOW

enum STATE {
  WIFI_NOT_CONNECTED = 0, WIFI_CONNECTING, WIFI_CONNECTED, WIFI_RESET_START, WIFI_RESET_END
};

#define WIFI_MAX_CONNECTION_MINUTES 5