
#include "reseter.hpp"

static int currentState = WIFI_NOT_CONNECTED;
static int wifiConnectionProcessSeconds = 0;

static int ping1Times[MAX_PINGS];
static int ping1cnt = 0;
static int ping2Times[MAX_PINGS];
static int ping2cnt = 0;

void routerEnabled(bool state) {
  hal_gpio_write(ROUTER_RELAY_PIN, state); 
}

void startWifiConnection() {
  deb("trying to connect to WiFi: %s", WIFI_SSID);

  hal_wifi_disconnect(true);     
  hal_wifi_set_mode(HAL_WIFI_MODE_STA);
  hal_wifi_begin_station(WIFI_SSID, WIFI_PASSWORD, true);
}

void stopWifiConnection() {
  deb("performing disconnection...");

  hal_wifi_disconnect(true);     
  hal_wifi_set_mode(HAL_WIFI_MODE_OFF);
}

void setup() {
  debugInit();

  hal_gpio_set_mode(LED_BUILTIN, HAL_GPIO_OUTPUT);
  hal_gpio_set_mode(ROUTER_RELAY_PIN, HAL_GPIO_OUTPUT);
  hal_gpio_write(LED_BUILTIN, false);  

  hal_wifi_set_hostname(DEVICE_DOMAIN);

  hal_watchdog_enable(MAX_DEAD_TIME, false);  
}

static unsigned long t0;

void setState(int state) {
  deb("set state from:%s to %s", printableStates[currentState], printableStates[state]);
  currentState = state;
}

void loop() {
  hal_watchdog_feed();

  switch(currentState) {
    case WIFI_NOT_CONNECTED: {
      routerEnabled(ON);

      setState(WIFI_CONNECTING);
      wifiConnectionProcessSeconds = 0;

      ping1cnt = ping2cnt = 0;
      memset(ping1Times, 0, sizeof(ping1Times));
      memset(ping2Times, 0, sizeof(ping2Times));

      startWifiConnection();
      t0 = hal_millis();
    }
    break;

    case WIFI_CONNECTING: {
      
      if(hal_wifi_is_connected()) {

        char ipBuf[20];
        hal_wifi_get_local_ip(ipBuf, sizeof(ipBuf));
        deb("connected to WiFi, IP: %s", ipBuf);

        hal_gpio_write(LED_BUILTIN, true);  

        hal_wifi_set_timeout_ms(PING_TIMEOUT);

        t0 = hal_millis();
        setState(WIFI_CONNECTED);

      } else {
        if(hal_millis() - t0 > SECOND) {
          t0 = hal_millis();
          hal_gpio_write(LED_BUILTIN, !hal_gpio_read(LED_BUILTIN));

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
      t0 = hal_millis();
      setState(WIFI_RESET_END);
    }
    break;

    case WIFI_RESET_END: {
      if(hal_millis() - t0 > SECOND * WIFI_MAX_RESET_TIME_SECONDS) {
        setState(WIFI_NOT_CONNECTED);
      }
    }
    break;

    case WIFI_CONNECTED: {
      if(hal_millis() - t0 > SECOND) {
        unsigned long t_ping;
        int res1 = -1, res2 = -1;    
        unsigned long dt1 = 0, dt2 = 0;
        
        t_ping = hal_millis();
        res1 = hal_wifi_ping(PING_ONE); 
        dt1 = hal_millis() - t_ping;
        hal_watchdog_feed();

        t_ping = hal_millis();
        res2 = hal_wifi_ping(PING_TWO); 
        dt2 = hal_millis() - t_ping;
        hal_watchdog_feed();

        if (res1 >= 0 || res2 >= 0) {
          deb("Ping do %s / %s : %ld / %ld ms", PING_ONE, PING_TWO, dt1, dt2);
        } else {

          if(res1 < 0) {
            deb("No response from %s (timeout)", PING_ONE);
          }
          if(res2 < 0) {
            deb("No response from %s (timeout)", PING_TWO);
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
          PING_ONE, percentLost1, PING_TWO, percentLost2);

        if(percentLost1 > MAX_PERCENT_OF_LOST_PINGS && 
            percentLost2 > MAX_PERCENT_OF_LOST_PINGS) {
            setState(WIFI_RESET_START);
        }

        t0 = hal_millis();
      }
    }
    break;

  }
}
