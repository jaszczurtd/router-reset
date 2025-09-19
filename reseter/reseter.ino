
#include "reseter.hpp"

static int currentState = WIFI_NOT_CONNECTED;
static int wifiConnectionProcessSeconds = 0;
static String srv1, srv2;

static WiFiClient client;
static IPAddress ping1Target, ping2Target;

static int ping1Times[MAX_PINGS];
static int ping1cnt = 0;
static int ping2Times[MAX_PINGS];
static int ping2cnt = 0;

void routerEnabled(int state) {
  digitalWrite(ROUTER_RELAY_PIN, state); 
}

void startWifiConnection() {
  deb("trying to connect to WiFi: %s", WIFI_SSID);

  WiFi.disconnect(true);     
  WiFi.mode(WIFI_STA);
  WiFi.beginNoBlock(WIFI_SSID, WIFI_PASSWORD);
}

void stopWifiConnection() {
  deb("performing disconnection...");

  WiFi.disconnect(true);     
  WiFi.mode(WIFI_OFF);
}

void setup() {
  debugInit();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(0, OUTPUT); //RP2040 PicoW pin 0
  digitalWrite(LED_BUILTIN, LOW);  

  WiFi.setHostname(DEVICE_DOMAIN);

  //adresses defined in reseter.hpp
  ping1Target.fromString(PING_ONE);
  srv1 = ping1Target.toString();

  ping2Target.fromString(PING_TWO);
  srv2 = ping2Target.toString();

  watchdog_enable(MAX_DEAD_TIME, false);  
}

static unsigned long t0;

void setState(int state) {
  deb("set state from:%s to %s", printableStates[currentState], printableStates[state]);
  currentState = state;
}

void loop() {
  watchdog_update();

  switch(currentState) {
    case WIFI_NOT_CONNECTED: {
      routerEnabled(ON);

      setState(WIFI_CONNECTING);
      wifiConnectionProcessSeconds = 0;

      ping1cnt = ping2cnt = 0;
      memset(ping1Times, 0, sizeof(ping1Times));
      memset(ping2Times, 0, sizeof(ping2Times));

      startWifiConnection();
      t0 = millis();
    }
    break;

    case WIFI_CONNECTING: {
      
      if(WiFi.status() == WL_CONNECTED) {

        deb("connected to WiFi, IP: %s", WiFi.localIP().toString().c_str());

        digitalWrite(LED_BUILTIN, HIGH);  

        WiFi.setTimeout(PING_TIMEOUT);

        t0 = millis();
        setState(WIFI_CONNECTED);

      } else {
        if(millis() - t0 > SECOND) {
          t0 = millis();
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

          deb("connecting for %ld seconds...", wifiConnectionProcessSeconds);

          if(wifiConnectionProcessSeconds > 0 && 
              wifiConnectionProcessSeconds % SECONDS_IN_MINUTE == 0) {
            deb("restarting WiFi");
            stopWifiConnection();
            startWifiConnection();
          }

          if(++wifiConnectionProcessSeconds > (WIFI_MAX_CONNECTION_MINUTES * SECONDS_IN_MINUTE)) {
            setState(WIFI_RESET_START);
          }
        }
      }
    }
    break;

    case WIFI_RESET_START: {
      stopWifiConnection();
      routerEnabled(OFF);
      t0 = millis();
      setState(WIFI_RESET_END);
    }
    break;

    case WIFI_RESET_END: {
      if(millis() - t0 > SECOND * WIFI_MAX_RESET_TIME_SECONDS) {
        setState(WIFI_NOT_CONNECTED);
      }
    }
    break;

    case WIFI_CONNECTED: {
      if(millis() - t0 > SECOND) {
        unsigned long t_ping;
        int res1 = -1, res2 = -1;    
        unsigned long dt1 = 0, dt2 = 0;
        
        t_ping = millis();
        res1 = WiFi.ping(ping1Target); 
        dt1 = millis() - t_ping;
        watchdog_update();

        t_ping = millis();
        res2 = WiFi.ping(ping2Target); 
        dt2 = millis() - t_ping;
        watchdog_update();

        if (res1 >= 0 || res2 >= 0) {
          deb("Ping do %s / %s : %ld / %ld ms", srv1.c_str(), srv2.c_str(), dt1, dt2);
        } else {

          if(res1 < 0) {
            deb("No response from %s (timeout)", srv1.c_str());
          }
          if(res2 < 0) {
            deb("No response from %s (timeout)", srv2.c_str());
          }

        }  

        ping1Times[ping1cnt] = res1 < 0 ? -1 : dt1;
        ping1cnt++; if(ping1cnt >= MAX_PINGS) {
          ping1cnt = 0;
        }

        ping2Times[ping2cnt] = res2 < 0 ? -1 : dt2;
        ping2cnt++; if(ping2cnt >= MAX_PINGS) {
          ping2cnt = 0;
        }

        int percentLost1 = 0;
        int percentLost2 = 0;
        int howManyLost1 = 0;
        int howManyLost2 = 0;

        for(int a = 0; a < MAX_PINGS; a++) {
          if(ping1Times[a] < 0) {
            howManyLost1++;
          }
          if(ping2Times[a] < 0) {
            howManyLost2++;
          }
        }
        percentLost1 = percentFrom(howManyLost1, MAX_PINGS);
        percentLost2 = percentFrom(howManyLost2, MAX_PINGS);

        deb("percent of lost packet for %s is:%d%%, for %s is:%d%%", 
          srv1.c_str(), percentLost1, srv2.c_str(), percentLost2);

        if(percentLost1 > MAX_PERCENT_OF_LOST_PINGS && 
            percentLost2 > MAX_PERCENT_OF_LOST_PINGS) {
            setState(WIFI_RESET_START);
        }

        t0 = millis();
      }
    }
    break;

  }
}
