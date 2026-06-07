#pragma once

/**
 * @file hal_project_config.h
 * @brief JaszczurHAL module configuration for the RouterWatchdog project.
 *
 * This file is automatically picked up by hal_config.h via __has_include.
 * Define HAL_DISABLE_* flags here to exclude unused HAL modules from the
 * build.
 */

#define PICO_W

/* ── Modules not used by RouterWatchdog ──────────────────────────────── */

#define HAL_ENABLE_MQTT
#define HAL_ENABLE_WIREGUARD
#define HAL_ENABLE_CJSON
#define HAL_ENABLE_WIFI
#define HAL_ENABLE_TIME