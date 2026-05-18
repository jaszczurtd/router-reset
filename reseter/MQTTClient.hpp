#pragma once

#include <cstdint>

#define MQTT_SOCKET_MAX_TIMEOUT 2
#define MQTT_RECONNECT_TIME 5000
#define MQTT_KEEPALIVE_SECONDS 15

#define MQTT_TOPIC_STATUS "status-"

void MQTTstart(const char *brokerIP, uint16_t port);
void MQTTstop(); 
void MQTThandleClient();
void MQTTpublish();

