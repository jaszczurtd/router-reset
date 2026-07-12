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

void clearAndFreeSecret(char *secret) {
  if (secret == nullptr) {
    return;
  }

  volatile char *bytes = secret;
  const size_t length = strlen(secret);
  for (size_t i = 0; i < length; ++i) {
    bytes[i] = '\0';
  }
  free(secret);
}
