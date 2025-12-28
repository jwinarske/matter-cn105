/**
 * @file matter_config.h
 * @brief Matter/CHIP configuration for heat pump controller
 * 
 * Defines Matter cluster IDs, device types, and configuration
 * for integration with the Matter smart home protocol
 */

#ifndef MATTER_CONFIG_H
#define MATTER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Matter Device Type
 * 
 * The heat pump is exposed as a Thermostat device type
 */
#define MATTER_DEVICE_TYPE_THERMOSTAT 0x0301

/**
 * @brief Standard Matter Cluster IDs
 */
#define MATTER_CLUSTER_IDENTIFY         0x0003  /**< Identify cluster */
#define MATTER_CLUSTER_ON_OFF           0x0006  /**< On/Off cluster */
#define MATTER_CLUSTER_DESCRIPTOR       0x001D  /**< Descriptor cluster */
#define MATTER_CLUSTER_THERMOSTAT       0x0201  /**< Thermostat cluster */
#define MATTER_CLUSTER_FAN_CONTROL      0x0202  /**< Fan Control cluster */
#define MATTER_CLUSTER_TEMP_MEASUREMENT 0x0402  /**< Temperature Measurement cluster */

/**
 * @brief Custom Vendor-Specific Cluster IDs
 * 
 * These clusters provide access to Mitsubishi-specific features
 * not available in standard Matter clusters.
 * 
 * Vendor ID range: 0xFFF1XXXX (example vendor ID)
 */
#define MATTER_CLUSTER_VANE_CONTROL     0xFFF1FC01  /**< Vertical vane control */
#define MATTER_CLUSTER_WIDE_VANE_CONTROL 0xFFF1FC02 /**< Horizontal vane control */
#define MATTER_CLUSTER_HP_STATUS        0xFFF1FC03  /**< Heat pump status information */
#define MATTER_CLUSTER_ISEE_CONTROL     0xFFF1FC04  /**< i-See sensor control */

/**
 * @brief Thermostat Cluster Attributes
 */
#define MATTER_ATTR_LOCAL_TEMPERATURE           0x0000  /**< Current room temperature */
#define MATTER_ATTR_OCCUPIED_COOLING_SETPOINT   0x0011  /**< Cooling setpoint */
#define MATTER_ATTR_OCCUPIED_HEATING_SETPOINT   0x0012  /**< Heating setpoint */
#define MATTER_ATTR_SYSTEM_MODE                 0x001C  /**< System mode (off/heat/cool/auto) */
#define MATTER_ATTR_THERMOSTAT_RUNNING_STATE    0x0029  /**< Running state */

/**
 * @brief Fan Control Cluster Attributes
 */
#define MATTER_ATTR_FAN_MODE                    0x0000  /**< Fan mode */
#define MATTER_ATTR_FAN_MODE_SEQUENCE           0x0001  /**< Fan mode sequence */
#define MATTER_ATTR_PERCENT_SETTING             0x0002  /**< Fan speed percentage */
#define MATTER_ATTR_PERCENT_CURRENT             0x0003  /**< Current fan speed percentage */

/**
 * @brief Thermostat System Mode Values
 */
#define MATTER_THERMOSTAT_MODE_OFF      0x00
#define MATTER_THERMOSTAT_MODE_AUTO     0x01
#define MATTER_THERMOSTAT_MODE_COOL     0x03
#define MATTER_THERMOSTAT_MODE_HEAT     0x04
#define MATTER_THERMOSTAT_MODE_DRY      0x08  /**< Precool/Dehumidify */
#define MATTER_THERMOSTAT_MODE_FAN_ONLY 0x07

/**
 * @brief Fan Mode Values
 */
#define MATTER_FAN_MODE_OFF    0x00
#define MATTER_FAN_MODE_LOW    0x01
#define MATTER_FAN_MODE_MEDIUM 0x02
#define MATTER_FAN_MODE_HIGH   0x03
#define MATTER_FAN_MODE_ON     0x04
#define MATTER_FAN_MODE_AUTO   0x05
#define MATTER_FAN_MODE_SMART  0x06

/**
 * @brief Matter Endpoint Configuration
 */
#define MATTER_ENDPOINT_ROOT        0   /**< Root endpoint */
#define MATTER_ENDPOINT_THERMOSTAT  1   /**< Thermostat endpoint */

/**
 * @brief Temperature Conversion Macros
 * 
 * Matter uses temperature in units of 0.01°C (hundredths of degrees)
 */
#define CELSIUS_TO_MATTER_TEMP(temp) ((int16_t)((temp) * 100))
#define MATTER_TEMP_TO_CELSIUS(temp) ((float)(temp) / 100.0f)

/**
 * @brief Default Configuration Values
 */
#define DEFAULT_MIN_SETPOINT_CELSIUS    16.0f   /**< Minimum temperature setpoint */
#define DEFAULT_MAX_SETPOINT_CELSIUS    31.0f   /**< Maximum temperature setpoint */
#define DEFAULT_SETPOINT_CELSIUS        22.0f   /**< Default temperature setpoint */

/**
 * @brief Matter Stack Configuration
 * 
 * TODO: These values need to be tuned based on actual testing
 */
#define MATTER_THREAD_STACK_SIZE    4096    /**< Matter task stack size */
#define MATTER_THREAD_PRIORITY      5       /**< Matter task priority */

/**
 * @brief Update Intervals (milliseconds)
 */
#define HP_STATUS_UPDATE_INTERVAL_MS    5000    /**< Status polling interval */
#define HP_SETTINGS_UPDATE_INTERVAL_MS  1000    /**< Settings update interval */

/* Configuration placeholders for future implementation */
/* TODO: Implement actual Matter integration */
/* TODO: Add commissioning configuration */
/* TODO: Add network credential storage configuration */
/* TODO: Add OTA update configuration */
/* TODO: Add factory reset configuration */

#ifdef __cplusplus
}
#endif

#endif /* MATTER_CONFIG_H */
