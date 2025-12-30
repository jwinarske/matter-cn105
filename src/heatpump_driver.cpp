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

/* Thread configuration */
#define HEATPUMP_THREAD_STACK_SIZE 2048
#define HEATPUMP_THREAD_PRIORITY   5
#define HEATPUMP_UPDATE_INTERVAL_MS 100  /* Poll heat pump every 100ms */

/* Memory slab configuration for packet buffers */
#define PACKET_BUFFER_SIZE  64  /* Max packet size is 22 bytes, rounded to 64 for alignment */
#define NUM_PACKET_BUFFERS  4   /* Pre-allocate 4 packet buffers for concurrent operations */

/**
 * @brief Packet buffer structure for memory slab
 *
 * Provides a predictable memory layout for efficient allocation.
 */
struct packet_buffer {
    uint8_t data[PACKET_BUFFER_SIZE];
    size_t length;
};

/* Define and initialize the packet buffer memory slab */
K_MEM_SLAB_DEFINE(packet_slab, sizeof(struct packet_buffer), NUM_PACKET_BUFFERS, 4);

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

/* Thread management */
static struct k_thread heatpump_thread_data;
static k_tid_t heatpump_thread_id = NULL;
static bool heatpump_thread_running = false;
static K_THREAD_STACK_DEFINE(heatpump_stack, HEATPUMP_THREAD_STACK_SIZE);

/* Forward declarations for HeatPump library callbacks */
static void hp_on_connect_callback(void);
static void hp_settings_changed_callback(void);
static void hp_status_changed_callback(heatpumpStatus newStatus);
static void hp_packet_callback(uint8_t* packet, unsigned int length, char* packetDirection);
static void hp_room_temp_changed_callback(float currentRoomTemperature);
/**
 * @brief HeatPump library callback: Connection established
 * 
 * Called when the heat pump handshake is successful
 */
static void hp_on_connect_callback(void)
{
    LOG_INF("Heat pump connected");
    connected = true;
}

/**
 * @brief HeatPump library callback: Settings changed
 * 
 * Called when the heat pump settings change
 */
static void hp_settings_changed_callback(void)
{
    LOG_INF("Heat pump settings changed");
    
    /* Update local cache */
    heatpumpSettings hp = s_hp.getSettings();
    current_settings.power = hp.power;
    current_settings.mode = hp.mode;
    current_settings.temperature = hp.temperature;
    current_settings.fan = hp.fan;
    current_settings.vane = hp.vane;
    current_settings.wideVane = hp.wideVane;
    current_settings.iSee = hp.iSee;
    current_settings.connected = connected;
    
    /* Call registered application callback if present */
    if (settings_callback) {
        settings_callback(current_settings);
    }
}

/**
 * @brief HeatPump library callback: Status changed
 * 
 * Called when the heat pump status changes
 * 
 * @param newStatus The new status from the heat pump
 */
static void hp_status_changed_callback(heatpumpStatus newStatus)
{
    LOG_INF("Heat pump status changed");
    
    /* Update local cache */
    current_status.roomTemperature = newStatus.roomTemperature;
    current_status.operating = newStatus.operating;
    current_status.compressorFrequency = newStatus.compressorFrequency;
    
    /* Update timers as well */
    current_timers.mode = newStatus.timers.mode;
    current_timers.onMinutesSet = newStatus.timers.onMinutesSet;
    current_timers.onMinutesRemaining = newStatus.timers.onMinutesRemaining;
    current_timers.offMinutesSet = newStatus.timers.offMinutesSet;
    current_timers.offMinutesRemaining = newStatus.timers.offMinutesRemaining;
    
    /* Call registered application callback if present */
    if (status_callback) {
        status_callback(current_status);
    }
}

/**
 * @brief HeatPump library callback: Packet transmitted or received
 * 
 * Called for debug/logging purposes when packets are sent or received
 * 
 * @param packet The packet data
 * @param length The packet length
 * @param packetDirection Direction indicator ("packetSent" or "packetRecv")
 */
static void hp_packet_callback(uint8_t* packet, unsigned int length, char* packetDirection)
{
    if (packet && length > 0 && packetDirection) {
        LOG_DBG("Heat pump packet %s: %d bytes", packetDirection, length);
        /* Could log hex dump here for deeper debugging if needed */
    }
}

/**
 * @brief HeatPump library callback: Room temperature changed
 * 
 * Called when the room temperature reading changes
 * 
 * @param currentRoomTemperature The new room temperature
 */
static void hp_room_temp_changed_callback(float currentRoomTemperature)
{
    LOG_INF("Room temperature: %.1f°C", (double)currentRoomTemperature);
    current_status.roomTemperature = currentRoomTemperature;
    
    /* Call registered application callback if present */
    if (status_callback) {
        status_callback(current_status);
    }
}

/**
 * @brief Heatpump update thread
 * 
 * This thread runs periodically to:
 * - Transmit pending commands to the heat pump
 * - Read responses from the heat pump
 * - Invoke callbacks when state changes occur
 * 
 * This replaces the Arduino loop() paradigm with Zephyr threading
 * 
 * @param arg1 Unused argument (required by k_thread_create)
 * @param arg2 Unused argument (required by k_thread_create)
 * @param arg3 Unused argument (required by k_thread_create)
 */
static void heatpump_update_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);
    
    LOG_INF("Heat pump update thread started");
    heatpump_thread_running = true;
    
    /* Main update loop */
    while (heatpump_thread_running) {
        /* Call the HeatPump library's update method */
        /* This method handles:
         * - Sending queued commands
         * - Reading responses
         * - Triggering registered callbacks
         */
        if (s_hp.update()) {
            /* Update was successful */
            LOG_DBG("Heat pump update completed");
        }
        
        /* Sleep for the configured update interval */
        k_msleep(HEATPUMP_UPDATE_INTERVAL_MS);
    }
    
    LOG_INF("Heat pump update thread stopped");
}

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
    
    /* Register HeatPump library callbacks for Zephyr integration */
    s_hp.setOnConnectCallback(hp_on_connect_callback);
    s_hp.setSettingsChangedCallback(hp_settings_changed_callback);
    s_hp.setStatusChangedCallback(hp_status_changed_callback);
    s_hp.setPacketCallback(hp_packet_callback);
    s_hp.setRoomTempChangedCallback(hp_room_temp_changed_callback);
    
    /* Start the heat pump update thread */
    heatpump_thread_id = k_thread_create(&heatpump_thread_data,
                                         heatpump_stack,
                                         HEATPUMP_THREAD_STACK_SIZE,
                                         heatpump_update_thread,
                                         NULL, NULL, NULL,
                                         HEATPUMP_THREAD_PRIORITY,
                                         0,
                                         K_NO_WAIT);
    
    if (heatpump_thread_id == NULL) {
        LOG_ERR("Failed to create heat pump update thread");
        return -EAGAIN;
    }
    
    k_thread_name_set(heatpump_thread_id, "heatpump");
    
    LOG_INF("Heat pump driver initialized");
    return 0;
}

/**
 * @brief Shutdown the heat pump driver
 *
 * Stops the update thread and cleans up resources
 *
 * @return 0 on success, negative errno on failure
 */
int heatpump_shutdown(void)
{
    LOG_INF("Shutting down heat pump driver");
    
    if (heatpump_thread_id != NULL) {
        /* Signal the thread to stop */
        heatpump_thread_running = false;
        
        /* Wait for thread to complete (with timeout) */
        int ret = k_thread_join(heatpump_thread_id, K_MSEC(5000));
        if (ret != 0) {
            LOG_WRN("Heat pump thread did not exit cleanly, forcing abort");
            k_thread_abort(heatpump_thread_id);
        }
        
        heatpump_thread_id = NULL;
    }
    
    LOG_INF("Heat pump driver shutdown complete");
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
