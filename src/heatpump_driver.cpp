/**
 * @file heatpump_driver.cpp
 * @brief Heat pump driver implementation for CN105 communication
 * 
 * This file will contain the adaptation of the SwiCago/HeatPump library
 * for use with Zephyr RTOS.
 * 
 * Original library: https://github.com/SwiCago/HeatPump
 * License: GPL-3.0
 */

#include "heatpump_driver.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "../lib/HeatPump/heat_pump.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(heatpump_driver, CONFIG_LOG_DEFAULT_LEVEL);

/* Static variables for driver state */
static heatpump_settings_t current_settings;
static heatpump_status_t current_status;
static heatpump_timers_t current_timers;
static bool connected = false;

/* Callback functions */
static heatpump_settings_callback_t settings_callback = NULL;
static heatpump_status_callback_t status_callback = NULL;

/* UART device (assigned when CN105 transport is implemented) */
static const struct device *uart_dev;
static HeatPump s_hp;

/**
 * @brief Initialize the heat pump driver
 */
int heatpump_init(void)
{
    LOG_INF("Initializing heat pump driver");
    
#if DT_HAS_CHOSEN(heatpump_uart)
    uart_dev = DEVICE_DT_GET(DT_CHOSEN(heatpump_uart));
#else
    uart_dev = DEVICE_DT_GET(DT_NODELABEL(eusart0));
#endif
    if (!device_is_ready(uart_dev)) {
        LOG_ERR("Heatpump UART device not ready");
        return -ENODEV;
    }
    if (!s_hp.connect(uart_dev, HP_UART_BAUD_RATE)) {
        LOG_ERR("Heat pump handshake failed");
        return -EIO;
    }
    connected = s_hp.isConnected();
    
    /* Initialize settings to default values */
    current_settings.power = "OFF";
    current_settings.mode = "AUTO";
    current_settings.temperature = 22.0f;
    current_settings.fan = "AUTO";
    current_settings.vane = "AUTO";
    current_settings.wideVane = "|";
    current_settings.iSee = false;
    current_settings.connected = false;
    
    /* Initialize status */
    current_status.roomTemperature = 20.0f;
    current_status.operating = false;
    current_status.compressorFrequency = 0;
    
    /* Initialize timers */
    current_timers.mode = "NONE";
    current_timers.onMinutesSet = 0;
    current_timers.onMinutesRemaining = 0;
    current_timers.offMinutesSet = 0;
    current_timers.offMinutesRemaining = 0;
    
    LOG_INF("Heat pump driver initialized");
    return 0;
}

/**
 * @brief Connect to the heat pump
 */
int heatpump_connect(void)
{
    LOG_INF("Connecting to heat pump...");
    if (!device_is_ready(uart_dev)) {
        return -ENODEV;
    }
    if (!s_hp.connect(uart_dev, HP_UART_BAUD_RATE)) {
        return -EIO;
    }
    connected = s_hp.isConnected();
    return connected ? 0 : -EIO;
}

/**
 * @brief Synchronize with the heat pump
 */
void heatpump_sync(void)
{
    s_hp.sync();
}

/**
 * @brief Get current heat pump settings
 */
int heatpump_get_settings(heatpump_settings_t *settings)
{
    if (settings == NULL) {
        return -EINVAL;
    }
    heatpumpSettings hp = s_hp.getSettings();
    settings->power = hp.power;
    settings->mode = hp.mode;
    settings->temperature = hp.temperature;
    settings->fan = hp.fan;
    settings->vane = hp.vane;
    settings->wideVane = hp.wideVane;
    settings->iSee = hp.iSee;
    settings->connected = connected;
    return 0;
}

/**
 * @brief Get current heat pump status
 */
int heatpump_get_status(heatpump_status_t *status)
{
    if (status == NULL) {
        return -EINVAL;
    }
    heatpumpStatus s = s_hp.getStatus();
    status->roomTemperature = s.roomTemperature;
    status->operating = s.operating;
    status->compressorFrequency = s.compressorFrequency;
    return 0;
}

/**
 * @brief Get current heat pump timers
 */
int heatpump_get_timers(heatpump_timers_t *timers)
{
    if (timers == NULL) {
        return -EINVAL;
    }
    /* The library exposes timers via getStatus parsing; no direct method */
    heatpumpStatus s = s_hp.getStatus();
    (void)s;
    *timers = current_timers;
    return 0;
}

/**
 * @brief Set power state
 */
int heatpump_set_power(const char *power)
{
    LOG_INF("Setting power: %s", power);
    s_hp.setPowerSetting((power && power[0] == 'O' && power[1] == 'N') ? true : false);
    return s_hp.update() ? 0 : -EIO;
}

/**
 * @brief Set operating mode
 */
int heatpump_set_mode(const char *mode)
{
    LOG_INF("Setting mode: %s", mode);
    s_hp.setModeSetting(mode);
    return s_hp.update() ? 0 : -EIO;
}

/**
 * @brief Set target temperature
 */
int heatpump_set_temperature(float temperature)
{
    LOG_INF("Setting temperature: %.1f°C", (double)temperature);
    s_hp.setTemperature(temperature);
    return s_hp.update() ? 0 : -EIO;
}

/**
 * @brief Set fan speed
 */
int heatpump_set_fan(const char *fan)
{
    LOG_INF("Setting fan: %s", fan);
    s_hp.setFanSpeed(fan);
    return s_hp.update() ? 0 : -EIO;
}

/**
 * @brief Set vertical vane position
 */
int heatpump_set_vane(const char *vane)
{
    LOG_INF("Setting vane: %s", vane);
    s_hp.setVaneSetting(vane);
    return s_hp.update() ? 0 : -EIO;
}

/**
 * @brief Set horizontal vane position
 */
int heatpump_set_wide_vane(const char *wide_vane)
{
    LOG_INF("Setting wide vane: %s", wide_vane);
    s_hp.setWideVaneSetting(wide_vane);
    return s_hp.update() ? 0 : -EIO;
}

/**
 * @brief Update all settings at once
 */
int heatpump_update_settings(const heatpump_settings_t *settings)
{
    if (settings == NULL) {
        return -EINVAL;
    }
    LOG_INF("Updating all settings");
    heatpumpSettings s;
    s.power = settings->power;
    s.mode = settings->mode;
    s.temperature = settings->temperature;
    s.fan = settings->fan;
    s.vane = settings->vane;
    s.wideVane = settings->wideVane;
    s_hp.setSettings(s);
    return s_hp.update() ? 0 : -EIO;
}

/**
 * @brief Register callback for settings changes
 */
void heatpump_set_settings_callback(heatpump_settings_callback_t callback)
{
    settings_callback = callback;
}

/**
 * @brief Register callback for status changes
 */
void heatpump_set_status_callback(heatpump_status_callback_t callback)
{
    status_callback = callback;
}

/**
 * @brief Check if connected to heat pump
 */
bool heatpump_is_connected(void)
{
    return s_hp.isConnected();
}
