#pragma once

/**
 * @file hal_project_config.h
 * @brief JaszczurHAL configuration of the router reseter.
 *
 * The hardware (relay pin, status LED) comes from the assemblies of
 * device_tree.json; this file selects the HAL modules the program needs.
 */

#ifndef HAL_PROVIDE_APP_ENTRY
#define HAL_PROVIDE_APP_ENTRY
#endif

/* Network: WiFi station, MQTT status reports, WireGuard tunnel, NTP time. */
#define HAL_ENABLE_MQTT
#define HAL_ENABLE_WIREGUARD
#define HAL_ENABLE_CJSON
#define HAL_ENABLE_WIFI
#define HAL_ENABLE_TIME

/* The ESP32-S3-Zero status LED is an addressable WS2812, driven through the
 * RGB LED driver; the Pico boards have a plain one. */
#if defined(HAL_TARGET_ESP32_S3)
#define HAL_ENABLE_RGB_LED
#endif
