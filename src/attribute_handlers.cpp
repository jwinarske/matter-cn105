/**
 * @file attribute_handlers.cpp
 * @brief Matter attribute read/write handlers
 * 
 * Handles Matter cluster attribute operations and converts
 * between Matter data formats and heat pump formats.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "matter_config.h"
#include "heatpump_driver.h"

LOG_MODULE_REGISTER(attribute_handlers, CONFIG_LOG_DEFAULT_LEVEL);

/**
 * @brief Handle thermostat system mode attribute write
 * 
 * Converts Matter thermostat mode to heat pump mode and applies
 */
int handle_thermostat_mode_write(uint8_t mode)
{
    const char *hp_mode = NULL;
    
    /* Convert Matter mode to heat pump mode */
    switch (mode) {
        case MATTER_THERMOSTAT_MODE_OFF:
            return heatpump_set_power("OFF");
        
        case MATTER_THERMOSTAT_MODE_HEAT:
            hp_mode = "HEAT";
            break;
        
        case MATTER_THERMOSTAT_MODE_COOL:
            hp_mode = "COOL";
            break;
        
        case MATTER_THERMOSTAT_MODE_AUTO:
            hp_mode = "AUTO";
            break;
        
        case MATTER_THERMOSTAT_MODE_DRY:
            hp_mode = "DRY";
            break;
        
        case MATTER_THERMOSTAT_MODE_FAN_ONLY:
            hp_mode = "FAN";
            break;
        
        default:
            LOG_ERR("Unsupported thermostat mode: %d", mode);
            return -EINVAL;
    }
    
    /* Ensure power is on */
    heatpump_set_power("ON");
    
    /* Set the mode */
    return heatpump_set_mode(hp_mode);
}

/**
 * @brief Handle temperature setpoint attribute write
 * 
 * Converts Matter temperature (0.01°C units) to Celsius
 */
int handle_temperature_setpoint_write(int16_t matter_temp)
{
    float celsius = MATTER_TEMP_TO_CELSIUS(matter_temp);
    
    /* Validate temperature range */
    if (celsius < HP_TEMP_MIN || celsius > HP_TEMP_MAX) {
        LOG_ERR("Temperature out of range: %.1f°C", static_cast<double>(celsius));
        return -EINVAL;
    }
    
    return heatpump_set_temperature(celsius);
}

/**
 * @brief Handle fan mode attribute write
 * 
 * Converts Matter fan mode to heat pump fan setting
 */
int handle_fan_mode_write(uint8_t mode)
{
    const char *hp_fan = NULL;
    
    /* Convert Matter fan mode to heat pump fan */
    switch (mode) {
        case MATTER_FAN_MODE_OFF:
            hp_fan = "QUIET";  /* Map OFF to QUIET */
            break;
        
        case MATTER_FAN_MODE_LOW:
            hp_fan = "1";
            break;
        
        case MATTER_FAN_MODE_MEDIUM:
            hp_fan = "2";
            break;
        
        case MATTER_FAN_MODE_HIGH:
            hp_fan = "4";
            break;
        
        case MATTER_FAN_MODE_AUTO:
            hp_fan = "AUTO";
            break;
        
        default:
            LOG_ERR("Unsupported fan mode: %d", mode);
            return -EINVAL;
    }
    
    return heatpump_set_fan(hp_fan);
}

/**
 * @brief Read thermostat local temperature attribute
 * 
 * Returns current room temperature from heat pump
 */
int handle_local_temperature_read(int16_t *matter_temp)
{
    heatpump_status_t status;
    
    if (heatpump_get_status(&status) != 0) {
        return -EIO;
    }
    
    *matter_temp = CELSIUS_TO_MATTER_TEMP(status.roomTemperature);
    return 0;
}

/**
 * @brief Read thermostat running state attribute
 * 
 * Returns whether heat pump is actively heating/cooling
 */
int handle_running_state_read(uint16_t *state)
{
    heatpump_status_t status;
    
    if (heatpump_get_status(&status) != 0) {
        return -EIO;
    }
    
    /* TODO: Map heat pump operating state to Matter running state bits */
    *state = status.operating ? 0x0001 : 0x0000;
    
    return 0;
}

/**
 * @brief Handle custom vane control attribute write
 * 
 * Controls vertical vane position
 */
int handle_vane_position_write(uint8_t position)
{
    const char *vane = NULL;
    
    /* Map position to vane string */
    switch (position) {
        case 0: vane = "AUTO"; break;
        case 1: vane = "1"; break;
        case 2: vane = "2"; break;
        case 3: vane = "3"; break;
        case 4: vane = "4"; break;
        case 5: vane = "5"; break;
        case 6: vane = "SWING"; break;
        default:
            LOG_ERR("Invalid vane position: %d", position);
            return -EINVAL;
    }
    
    return heatpump_set_vane(vane);
}

/**
 * @brief Handle custom wide vane control attribute write
 * 
 * Controls horizontal vane position
 */
int handle_wide_vane_position_write(uint8_t position)
{
    const char *wide_vane = NULL;
    
    /* Map position to wide vane string */
    switch (position) {
        case 0: wide_vane = "<<"; break;
        case 1: wide_vane = "<"; break;
        case 2: wide_vane = "|"; break;
        case 3: wide_vane = ">"; break;
        case 4: wide_vane = ">>"; break;
        case 5: wide_vane = "<>"; break;
        case 6: wide_vane = "SWING"; break;
        default:
            LOG_ERR("Invalid wide vane position: %d", position);
            return -EINVAL;
    }
    
    return heatpump_set_wide_vane(wide_vane);
}
