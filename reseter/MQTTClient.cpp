#include "MQTTClient.hpp"

#include <Credentials.h>
#include <JaszczurHAL.h>
#include <tools.h>
#include <utils/cJSON.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "utils.hpp"

static char msg[512];
static char topic[128];

static bool clientInitialized = false;
static bool publishPending = false;

SmartTimers reconnectTimer;

static bool reconnect();

static void handleMessage(const char *topicArrived,
              const uint8_t *payload,
              uint16_t length,
              void *user) {
  (void)user;

  if(length >= sizeof(msg)) {
    length = sizeof(msg) - 1;
  }

  if(payload != nullptr && length > 0) {
    memcpy(msg, payload, length);
  } else if(length > 0) {
    length = 0;
  }

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

void MQTTstart(const char *brokerIP, uint16_t port) {
  if(brokerIP == nullptr || brokerIP[0] == '\0') {
    derr("invalid broker IP address!");
    return;
  }
  if(port == 0u) {
    derr("invalid broker port: %u", port);
    return;
  }

  deb("MQTT: connect attempt! %s / %u", brokerIP, port);

  if(!hal_mqtt_set_server(brokerIP, port)) {
    derr("MQTT: cannot set server to %s:%u", brokerIP, port);
    return;
  }

  hal_mqtt_set_socket_timeout(MQTT_SOCKET_MAX_TIMEOUT);
  hal_mqtt_set_keepalive(MQTT_KEEPALIVE_SECONDS);
  hal_mqtt_set_callback(handleMessage, nullptr);

  reconnectTimer.begin(nullptr, MQTT_RECONNECT_TIME);
  reconnect();
  clientInitialized = true;
}

void MQTTstop() {
  publishPending = clientInitialized = false;
  if(hal_mqtt_connected()) {
    deb("MQTT: disconnecting...");
    hal_mqtt_disconnect();
  }
}

void MQTTpublish() {
  if(hal_wifi_is_connected() && hal_mqtt_connected()) {
    // Detailed status publish is currently disabled in this sketch.
  }
}

static bool reconnect() {
  deb("MQTT: WiFi status: %d", hal_wifi_status());

  if(hal_wifi_is_connected() && hal_wifi_has_local_ip()) {
    const char *hostName = getMyHostname();
    hal_watchdog_feed();

    deb("MQTT: attempting connection to broker: %s", hostName);
    if(hal_mqtt_connect_auth(hostName, MQTT_USER, MQTT_PASSWORD)) {
      hal_watchdog_feed();

      snprintf(topic, sizeof(topic), "%s%s", MQTT_TOPIC_STATUS, hostName);
      if(!hal_mqtt_publish_str(topic, "", true)) {
        deb("MQTT: failed to publish retained status topic");
      }

      publishPending = true;

      deb("MQTT: (re)connected successfully!");

      return true;
    }
    deb("MQTT: connect failed! state=%d", hal_mqtt_state());

  } else {
    deb("MQTT: wifi is not connected!");
  }

  return false;
}

void MQTThandleClient() {
  if(clientInitialized) {
    if(!hal_mqtt_connected()) {
      if(reconnectTimer.available()) {
        reconnectTimer.restart();
        reconnect();
      }
    } else {
      hal_mqtt_loop();

      if(publishPending) {
        MQTTpublish();
        publishPending = false;
      }
    }
  } else {
    hal_mqtt_loop();
  }
}
