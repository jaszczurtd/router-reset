
#pragma once

#include <Arduino.h>
#include <WiFi.h>    
#include <tools.h>
#include <Credentials.h>

#define ROUTER_RELAY_PIN 0

#define DEVICE_DOMAIN "RouterWatchdog"

#define MAX_DEAD_TIME 7000

#define PING_ONE "1.1.1.1"
#define PING_TWO "8.8.8.8"

#define ON HIGH
#define OFF LOW

enum STATE {
  WIFI_NOT_CONNECTED = 0, WIFI_CONNECTING, WIFI_CONNECTED, WIFI_RESET_START, WIFI_RESET_END
};
const char *printableStates[] = {
  "WIFI_NOT_CONNECTED", "WIFI_CONNECTING", "WIFI_CONNECTED", "WIFI_RESET_START", "WIFI_RESET_END"
};

#define PING_TIMEOUT 1000

#define MAX_PINGS 100
#define MAX_PERCENT_OF_LOST_PINGS 75

#define WIFI_MAX_CONNECTION_MINUTES 6
#define WIFI_MAX_RESET_TIME_SECONDS 10

