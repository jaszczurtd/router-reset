#include "MQTTClient.hpp"

static char msg[512];
static char topic[128];
static char response[512];

static bool clientInitialized = false;
static bool publishPending = false;

WiFiClient currentClient;   
PubSubClient mqttClient(currentClient);
SmartTimers reconnectTimer;

bool reconnect();

void handleMessage(char* topicArrived, byte* payload, unsigned int length) {
    if(length >= sizeof(msg)) length = sizeof(msg) - 1;
    memcpy(msg, payload, length);
    msg[length] = '\0';

    cJSON *root = cJSON_Parse(msg);
    if(!root) {
    deb("MQTT: problem with json parsing");
    return;
    }

    const char *myHostName = getMyHostname();
    deb("MQTT: topic:%s update: %s hostname:%s", topicArrived, msg, myHostName);



    cJSON_Delete(root);
}

void MQTTstart(const char *brokerIP, const int port) {
    if(brokerIP == NULL) {
        derr("invalid broker IP address!");
        return;
    }
    deb("MQTT: connect attempt! %s / %d", brokerIP, port);
    
    IPAddress server;
    server.fromString(brokerIP);

    mqttClient.setServer(server, port);
    mqttClient.setSocketTimeout(MQTT_SOCKET_MAX_TIMEOUT);
    mqttClient.setKeepAlive(MQTT_KEEPALIVE);

    mqttClient.setCallback(handleMessage);
    reconnectTimer.begin(nullptr, MQTT_RECONNECT_TIME);
    reconnect();
    clientInitialized = true;
}

void MQTTstop() {
    publishPending = clientInitialized = false;
    if(mqttClient.connected()) {
        deb("MQTT: disconnecting...");
        mqttClient.disconnect();
    }
}

void MQTTpublish() {
  if(hal_wifi_is_connected() && mqttClient.connected()) {
//     long s = 0, e = 0;
//     bool *switches = hardware().getSwitchesStates();
//     cJSON *root = nullptr;
//     char *json = nullptr;
//     const char *time = nullptr;
//     char strength[10];

//     memset(response, 0, sizeof(response));

//     root = cJSON_CreateObject();
//     NONULL(root);

//     NONULL(cJSON_AddStringToObject(root, "status", "ok"));
//     NONULL(cJSON_AddNumberToObject(root, "dateHourStart", s));
//     NONULL(cJSON_AddNumberToObject(root, "dateHourEnd", e));
//     NONULL(cJSON_AddBoolToObject(root, "isOn1", switches[0]));
//     NONULL(cJSON_AddBoolToObject(root, "isOn2", switches[1]));
//     NONULL(cJSON_AddBoolToObject(root, "isOn3", switches[2]));
//     NONULL(cJSON_AddBoolToObject(root, "isOn4", switches[3]));

//     time = ntp().getTimeFormatted();
//     NONULL(cJSON_AddStringToObject(root, "localTime", strlen(time) ? time : "not available yet."));
//     NONULL(cJSON_AddNumberToObject(root, "localMillis", hal_millis()));

//     snprintf(strength, sizeof(strength), "5/%d", hal_wifi_get_strength());
//     NONULL(cJSON_AddStringToObject(root, "wifi", strength));

//     json = cJSON_PrintUnformatted(root);
//     NONULL(json);

//     strncpy(response, json, sizeof(response) - 1);
//     cJSON_free(json);
//     cJSON_Delete(root);

//     snprintf(topic, sizeof(topic), "%s%s", MQTT_TOPIC_STATUS, hardware().getMyHostname());
//     deb("MQTT: topic:%s publish: %s", topic, response);

//     mqttClient.publish(topic, response, true);
//     return;

// error:
//     if(json) {
//       cJSON_free(json);
//     }
//     if(root) {
//       cJSON_Delete(root);
//     }
//     strncpy(response, "{\"status\":\"JSON build failed\"}", sizeof(response) - 1);

//     snprintf(topic, sizeof(topic), "%s%s", MQTT_TOPIC_STATUS, hardware().getMyHostname());
//     deb("MQTT: topic:%s publish: %s", topic, response);
//     mqttClient.publish(topic, response, true);
//   } else {
//     publishPending = true;
   }
}

bool reconnect() {
  deb("MQTT: WiFi status: %d", hal_wifi_status());

  if(hal_wifi_is_connected() && hal_wifi_has_local_ip()) {
    const char *hostName = getMyHostname();
    hal_watchdog_feed();

    deb("MQTT: attempting connection to broker: %s", hostName);
    if(mqttClient.connect(hostName, MQTT_USER, MQTT_PASSWORD)) {
      hal_watchdog_feed();

      snprintf(topic, sizeof(topic), "%s%s", MQTT_TOPIC_STATUS, hostName);
      mqttClient.publish(topic, "", true);

      publishPending = true;

      deb("MQTT: (re)connected successfully!");

      return true;
    }
    deb("MQTT: connect failed! state=%d", mqttClient.state());

  } else {
    deb("MQTT: wifi is not connected!");
  }

  return false;
}

void MQTThandleClient() {
    if(clientInitialized) {
        if(!mqttClient.connected()) {
        if (reconnectTimer.available()) {
            reconnectTimer.restart();
            reconnect();
        }
        } else {
        mqttClient.loop();

        if(publishPending) {
            MQTTpublish();
            publishPending = false;
        }
        }
    } else {
        mqttClient.loop();
    }
}
