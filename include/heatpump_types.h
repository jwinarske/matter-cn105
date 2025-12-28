/**
 * @file heatpump_types.h
 * @brief Data structures for Mitsubishi heat pump control
 * 
 * Based on the SwiCago/HeatPump library data structures
 * Adapted for use with Zephyr RTOS and Matter integration
 * 
 * Original library: https://github.com/SwiCago/HeatPump
 * License: GPL-3.0
 */

#ifndef HEATPUMP_TYPES_H
#define HEATPUMP_TYPES_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Heat pump settings structure
 * 
 * Contains all user-configurable settings for the heat pump
 */
typedef struct {
    const char* power;        /**< Power state: "ON", "OFF" */
    const char* mode;         /**< Operating mode: "HEAT", "DRY", "COOL", "FAN", "AUTO" */
    float temperature;        /**< Target temperature in Celsius (16-31°C) */
    const char* fan;          /**< Fan speed: "AUTO", "QUIET", "1", "2", "3", "4" */
    const char* vane;         /**< Vertical vane position: "AUTO", "1", "2", "3", "4", "5", "SWING" */
    const char* wideVane;     /**< Horizontal vane position: "<<", "<", "|", ">", ">>", "<>", "SWING" */
    bool iSee;               /**< i-See sensor enabled/disabled */
    bool connected;          /**< Connection status with heat pump */
} heatpump_settings_t;

/**
 * @brief Heat pump status structure
 * 
 * Contains read-only status information from the heat pump
 */
typedef struct {
    float roomTemperature;       /**< Current room temperature in Celsius */
    bool operating;              /**< True if heat pump is actively heating/cooling */
    int compressorFrequency;     /**< Compressor frequency in Hz (0 when off) */
} heatpump_status_t;

/**
 * @brief Heat pump timers structure
 * 
 * Contains timer settings and status
 */
typedef struct {
    const char* mode;            /**< Timer mode: "NONE", "OFF", "ON", "BOTH" */
    int onMinutesSet;           /**< Minutes until ON timer activates (0 if not set) */
    int onMinutesRemaining;     /**< Minutes remaining for ON timer */
    int offMinutesSet;          /**< Minutes until OFF timer activates (0 if not set) */
    int offMinutesRemaining;    /**< Minutes remaining for OFF timer */
} heatpump_timers_t;

/**
 * @brief Heat pump operating modes enumeration
 */
typedef enum {
    HP_MODE_HEAT = 0,
    HP_MODE_DRY,
    HP_MODE_COOL,
    HP_MODE_FAN,
    HP_MODE_AUTO
} heatpump_mode_e;

/**
 * @brief Fan speed enumeration
 */
typedef enum {
    HP_FAN_AUTO = 0,
    HP_FAN_QUIET,
    HP_FAN_1,
    HP_FAN_2,
    HP_FAN_3,
    HP_FAN_4
} heatpump_fan_e;

/**
 * @brief Vane position enumeration
 */
typedef enum {
    HP_VANE_AUTO = 0,
    HP_VANE_1,
    HP_VANE_2,
    HP_VANE_3,
    HP_VANE_4,
    HP_VANE_5,
    HP_VANE_SWING
} heatpump_vane_e;

/**
 * @brief Wide vane (horizontal) position enumeration
 */
typedef enum {
    HP_WIDE_VANE_FAR_LEFT = 0,    /**< "<<" */
    HP_WIDE_VANE_LEFT,             /**< "<" */
    HP_WIDE_VANE_CENTER,           /**< "|" */
    HP_WIDE_VANE_RIGHT,            /**< ">" */
    HP_WIDE_VANE_FAR_RIGHT,        /**< ">>" */
    HP_WIDE_VANE_WIDE,             /**< "<>" */
    HP_WIDE_VANE_SWING             /**< "SWING" */
} heatpump_wide_vane_e;

/**
 * @brief Temperature limits
 */
#define HP_TEMP_MIN 16.0f
#define HP_TEMP_MAX 31.0f

/**
 * @brief Serial communication parameters for CN105
 */
#define HP_UART_BAUD_RATE 2400
#define HP_UART_DATA_BITS 8
#define HP_UART_PARITY_EVEN 1
#define HP_UART_STOP_BITS 1

#ifdef __cplusplus
}
#endif

#endif /* HEATPUMP_TYPES_H */
