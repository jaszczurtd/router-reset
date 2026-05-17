#pragma once

#include <JaszczurHAL.h>
#include <tools.h>

#include <WiFi.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include <string.h>
#include <PubSubClient.h>

#include <utils/cJSON.h>

#include "PubSubClient.h"
#include "ca_cert.h"
#include <Credentials.h>
#include <cstring>
#include <cstdio>

#include "utils.hpp"

#define MQTT_SOCKET_MAX_TIMEOUT 2
#define MQTT_RECONNECT_TIME 5000

#define MQTT_TOPIC_STATUS "status-"

void MQTTstart(const char *brokerIP, const int port);
void MQTTstop(); 
void MQTThandleClient();
void MQTTpublish();

