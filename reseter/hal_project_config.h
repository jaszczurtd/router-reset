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

#define HAL_DISABLE_GPS
#define HAL_DISABLE_THERMOCOUPLE
#define HAL_DISABLE_UART
#define HAL_DISABLE_SWSERIAL
#define HAL_DISABLE_DISPLAY
#define HAL_DISABLE_RGB_LED
#define HAL_DISABLE_EXTERNAL_ADC
#define HAL_DISABLE_CAN
#define HAL_DISABLE_EEPROM
#define HAL_DISABLE_I2C
#define HAL_DISABLE_PWM_FREQ
#define HAL_DISABLE_UNITY
