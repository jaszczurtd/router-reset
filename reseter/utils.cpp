#include "utils.hpp"

char mac_str[sizeof("FF:FF:FF:FF:FF:FF") + 1];
const char *getMyMAC(void) {
  if (!hal_wifi_get_mac(mac_str, sizeof(mac_str))) {
    snprintf(mac_str, sizeof(mac_str), "%s", "00:00:00:00:00:00");
  }
  return (const char *)mac_str;
}

char hostname_str[32];
const char *getMyHostname(void) {
  snprintf(hostname_str, sizeof(hostname_str), "%s", getFriendlyHostname(getMyMAC()));
  return (const char *)hostname_str;
}

