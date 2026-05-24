#pragma once

#include <JaszczurHAL.h>
#include <tools.h>
#include <Credentials.h>
#include <time.h>

#include "utils.hpp"
#include "MQTTClient.hpp"

#define ROUTER_RELAY_PIN 0

#define DEVICE_DOMAIN "RouterWatchdog"

#define MAX_DEAD_TIME 7000

#define PING_ONE "1.1.1.1"
#define PING_TWO "8.8.8.8"

#define ON true
#define OFF false

#define NTP_BUFFER 64

enum STATE {
    WIFI_NOT_CONNECTED = 0,
    WIFI_CONNECTING,
    NTP_SYNC,
    WG_START,
    WG_HANDSHAKE,
    MQTT_START,
    CONNECTED_AND_WORKING,
    WIFI_RESET_START,
    WIFI_RESET_END
};
inline const char *printableStates[] = {
    "WIFI_NOT_CONNECTED",
    "WIFI_CONNECTING",
    "NTP_SYNC",
    "WG_START",
    "WG_HANDSHAKE",
    "MQTT_START",
    "CONNECTED_AND_WORKING",
    "WIFI_RESET_START",
    "WIFI_RESET_END"
};

#define PING_TIMEOUT SECOND

#define MAX_PINGS 200
#define MAX_PERCENT_OF_LOST_PINGS 75

#define WIFI_MAX_CONNECTION_MINUTES 10
#define WIFI_MAX_RESET_TIME_SECONDS 10

#define WG_HANDSHAKE_TIMEOUT_SECONDS 30
#define WG_HANDSHAKE_KICK_INTERVAL_MS 250

#define NTP_SYNC_TIMEOUT_SECONDS 20

#define MAX_TIMEOUT (SECOND * 5)

void initialization();
void mainLoop();