
#include "reseter.hpp"

static int currentState = WIFI_NOT_CONNECTED;
static int wifiConnectionProcessSeconds = 0;
static String srv;

IPAddress pingTarget(8, 8, 8, 8);
WiFiClient client;

void routerEnabled(int state) {
  digitalWrite(0, state); 
}

void startWifiConnection() {
  deb("Laczenie z WiFi: %s", WIFI_SSID);

  WiFi.disconnect(true);     
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void stopWifiConnection() {
  deb("Rozlaczanie...");

  WiFi.disconnect(true);     
  WiFi.end();
}

void setup() {
  debugInit();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(0, OUTPUT); //RP2040 PicoW pin 0
  digitalWrite(LED_BUILTIN, LOW);  

  WiFi.setHostname("RouterWatchdog");
  srv = pingTarget.toString();
}

static unsigned long t0;

void loop() {

  unsigned long dt;

  switch(currentState) {
    case WIFI_NOT_CONNECTED: {
      startWifiConnection();
      routerEnabled(ON);

      currentState = WIFI_CONNECTING;
      wifiConnectionProcessSeconds = 0;
      t0 = millis();
    }
    break;

    case WIFI_CONNECTING: {
      if(WiFi.status() == WL_CONNECTED) {

        deb("Polaczono z wifi, adres IP: %s", WiFi.localIP().toString().c_str());

        digitalWrite(LED_BUILTIN, HIGH);  
        t0 = millis();
        currentState = WIFI_CONNECTED;

      } else {
        if(millis() - t0 > SECOND) {
          t0 = millis();
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

          deb("connection seconds: %ld", wifiConnectionProcessSeconds);

          if(++wifiConnectionProcessSeconds > (WIFI_MAX_CONNECTION_MINUTES * 60)) {
            stopWifiConnection();
            routerEnabled(OFF);
            currentState = WIFI_RESET_START;
          }
        }
      }
    }
    break;

    case WIFI_RESET_START: {

    }
    break;

    case WIFI_RESET_END: {

    }
    break;

    case WIFI_CONNECTED: {
      if(millis() - t0 > SECOND) {
        t0 = millis();

        unsigned long t_ping = millis();
        int res = WiFi.ping(pingTarget); 
        dt = millis() - t_ping;
        if (res >= 0) {
          deb("Ping do %s : %ld ms", srv.c_str(), dt);
        } else {
          deb("No response from %s (timeout)", srv.c_str());
          //TODO:
        }  
      }
    }
    break;

  }
}
