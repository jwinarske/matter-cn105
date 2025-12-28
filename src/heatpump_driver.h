/**
 * @file heatpump_driver.h
 * @brief Heat pump driver interface for CN105 communication
 * 
 * Provides the API for communicating with Mitsubishi heat pumps
 * via the CN105 connector. Based on the SwiCago/HeatPump library.
 */

#ifndef HEATPUMP_DRIVER_H
#define HEATPUMP_DRIVER_H

#include "heatpump_types.h"
#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback function type for settings updates
 * 
 * Called when the heat pump settings change
 */
typedef void (*heatpump_settings_callback_t)(heatpump_settings_t settings);

/**
 * @brief Callback function type for status updates
 * 
 * Called when the heat pump status changes
 */
typedef void (*heatpump_status_callback_t)(heatpump_status_t status);

/**
 * @brief Initialize the heat pump driver
 * 
 * Sets up UART communication with the heat pump via CN105
 * 
 * @return 0 on success, negative errno on failure
 */
int heatpump_init(void);

/**
 * @brief Connect to the heat pump
 * 
 * Establishes communication and performs initial handshake
 * 
 * @return 0 on success, negative errno on failure
 */
int heatpump_connect(void);

/**
 * @brief Synchronize with the heat pump
 * 
 * Sends sync packets and waits for response
 * Should be called periodically to maintain connection
 */
void heatpump_sync(void);

/**
 * @brief Get current heat pump settings
 * 
 * @param settings Pointer to settings structure to fill
 * @return 0 on success, negative errno on failure
 */
int heatpump_get_settings(heatpump_settings_t *settings);

/**
 * @brief Get current heat pump status
 * 
 * @param status Pointer to status structure to fill
 * @return 0 on success, negative errno on failure
 */
int heatpump_get_status(heatpump_status_t *status);

/**
 * @brief Get current heat pump timers
 * 
 * @param timers Pointer to timers structure to fill
 * @return 0 on success, negative errno on failure
 */
int heatpump_get_timers(heatpump_timers_t *timers);

/**
 * @brief Set power state
 * 
 * @param power Power state: "ON" or "OFF"
 * @return 0 on success, negative errno on failure
 */
int heatpump_set_power(const char *power);

/**
 * @brief Set operating mode
 * 
 * @param mode Mode: "HEAT", "DRY", "COOL", "FAN", "AUTO"
 * @return 0 on success, negative errno on failure
 */
int heatpump_set_mode(const char *mode);

/**
 * @brief Set target temperature
 * 
 * @param temperature Temperature in Celsius (16-31°C)
 * @return 0 on success, negative errno on failure
 */
int heatpump_set_temperature(float temperature);

/**
 * @brief Set fan speed
 * 
 * @param fan Fan speed: "AUTO", "QUIET", "1", "2", "3", "4"
 * @return 0 on success, negative errno on failure
 */
int heatpump_set_fan(const char *fan);

/**
 * @brief Set vertical vane position
 * 
 * @param vane Vane position: "AUTO", "1"-"5", "SWING"
 * @return 0 on success, negative errno on failure
 */
int heatpump_set_vane(const char *vane);

/**
 * @brief Set horizontal vane position
 * 
 * @param wide_vane Wide vane position: "<<", "<", "|", ">", ">>", "<>", "SWING"
 * @return 0 on success, negative errno on failure
 */
int heatpump_set_wide_vane(const char *wide_vane);

/**
 * @brief Update all settings at once
 * 
 * More efficient than calling individual setters
 * 
 * @param settings Settings structure to apply
 * @return 0 on success, negative errno on failure
 */
int heatpump_update_settings(const heatpump_settings_t *settings);

/**
 * @brief Register callback for settings changes
 * 
 * @param callback Function to call when settings change
 */
void heatpump_set_settings_callback(heatpump_settings_callback_t callback);

/**
 * @brief Register callback for status changes
 * 
 * @param callback Function to call when status changes
 */
void heatpump_set_status_callback(heatpump_status_callback_t callback);

/**
 * @brief Check if connected to heat pump
 * 
 * @return true if connected, false otherwise
 */
bool heatpump_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* HEATPUMP_DRIVER_H */
