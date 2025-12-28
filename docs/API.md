# API Documentation

## Heat Pump Driver API

### Initialization

```c
#include "heatpump_driver.h"

// Initialize the driver
int ret = heatpump_init();
if (ret != 0) {
    // Handle error
}

// Connect to heat pump
ret = heatpump_connect();
```

### Reading State

```c
// Get current settings
heatpump_settings_t settings;
heatpump_get_settings(&settings);

printf("Power: %s\n", settings.power);
printf("Mode: %s\n", settings.mode);
printf("Temperature: %.1f°C\n", settings.temperature);

// Get current status
heatpump_status_t status;
heatpump_get_status(&status);

printf("Room Temperature: %.1f°C\n", status.roomTemperature);
printf("Operating: %s\n", status.operating ? "Yes" : "No");
```

### Controlling the Heat Pump

```c
// Set power
heatpump_set_power("ON");  // or "OFF"

// Set mode
heatpump_set_mode("HEAT");  // "HEAT", "COOL", "DRY", "FAN", "AUTO"

// Set temperature (16-31°C)
heatpump_set_temperature(22.0);

// Set fan speed
heatpump_set_fan("AUTO");  // "AUTO", "QUIET", "1", "2", "3", "4"

// Set vane position
heatpump_set_vane("AUTO");  // "AUTO", "1"-"5", "SWING"

// Set wide vane (horizontal)
heatpump_set_wide_vane("|");  // "<<", "<", "|", ">", ">>", "<>", "SWING"
```

### Batch Updates

```c
// Update multiple settings at once (more efficient)
heatpump_settings_t new_settings = {
    .power = "ON",
    .mode = "HEAT",
    .temperature = 22.0,
    .fan = "AUTO",
    .vane = "AUTO",
    .wideVane = "|",
    .iSee = false
};

heatpump_update_settings(&new_settings);
```

### Callbacks

```c
// Register callback for settings changes
void on_settings_changed(heatpump_settings_t settings) {
    printf("Settings changed: mode=%s, temp=%.1f\n", 
           settings.mode, settings.temperature);
}

heatpump_set_settings_callback(on_settings_changed);

// Register callback for status changes
void on_status_changed(heatpump_status_t status) {
    printf("Status changed: room_temp=%.1f, operating=%d\n",
           status.roomTemperature, status.operating);
}

heatpump_set_status_callback(on_status_changed);
```

### Synchronization

```c
// Periodic sync (should be called regularly)
void periodic_task(void) {
    heatpump_sync();
}

// Check connection status
if (heatpump_is_connected()) {
    // Heat pump is connected and responding
} else {
    // No connection to heat pump
}
```

## Matter Integration API

### Initialization

```c
#include "matter_integration.h"

// Initialize Matter stack
int ret = matter_init();

// Start Matter server
matter_start();
```

### Attribute Synchronization

```c
// Update Matter attributes from heat pump state
matter_update_attributes();

// Process attribute write requests from Matter controller
matter_process_attribute_writes();
```

### Attribute Handlers

```c
#include "attribute_handlers.h"

// Handle thermostat mode change
handle_thermostat_mode_write(MATTER_THERMOSTAT_MODE_HEAT);

// Handle temperature setpoint change (in 0.01°C units)
int16_t matter_temp = CELSIUS_TO_MATTER_TEMP(22.0);
handle_temperature_setpoint_write(matter_temp);

// Read current temperature
int16_t current_temp;
handle_local_temperature_read(&current_temp);
float celsius = MATTER_TEMP_TO_CELSIUS(current_temp);
```

## State Synchronization API

```c
#include "state_sync.h"

// Initialize state synchronization
state_sync_init();

// Sync from heat pump to Matter
state_sync_hp_to_matter();

// Sync from Matter to heat pump
state_sync_matter_to_hp();

// Periodic sync (call from main loop)
state_sync_periodic();
```

## Data Structures

### heatpump_settings_t

```c
typedef struct {
    const char* power;        // "ON", "OFF"
    const char* mode;         // "HEAT", "DRY", "COOL", "FAN", "AUTO"
    float temperature;        // 16-31°C
    const char* fan;          // "AUTO", "QUIET", "1"-"4"
    const char* vane;         // "AUTO", "1"-"5", "SWING"
    const char* wideVane;     // "<<", "<", "|", ">", ">>", "<>", "SWING"
    bool iSee;               // i-See sensor status
    bool connected;          // Connection status
} heatpump_settings_t;
```

### heatpump_status_t

```c
typedef struct {
    float roomTemperature;       // Current room temperature (°C)
    bool operating;              // True if actively heating/cooling
    int compressorFrequency;     // Compressor frequency (Hz)
} heatpump_status_t;
```

### heatpump_timers_t

```c
typedef struct {
    const char* mode;            // "NONE", "OFF", "ON", "BOTH"
    int onMinutesSet;           // Minutes until ON timer
    int onMinutesRemaining;     // Minutes remaining for ON
    int offMinutesSet;          // Minutes until OFF timer
    int offMinutesRemaining;    // Minutes remaining for OFF
} heatpump_timers_t;
```

## Error Codes

- `0`: Success
- `-EINVAL`: Invalid argument
- `-EIO`: I/O error (communication failure)
- `-ENOSYS`: Not yet implemented
- `-EBUSY`: Operation in progress
- `-ETIMEDOUT`: Operation timed out

## Threading Considerations

The heat pump driver uses UART interrupts for receiving data. Callbacks may be called from interrupt context. Keep callback handlers short and non-blocking.

For thread-safe operation, use Zephyr synchronization primitives (mutexes, semaphores) when accessing shared state.

## Example: Complete Integration

```c
#include <zephyr/kernel.h>
#include "heatpump_driver.h"
#include "matter_integration.h"
#include "state_sync.h"

void settings_changed(heatpump_settings_t settings) {
    // Update Matter attributes when heat pump settings change
    matter_update_attributes();
}

void status_changed(heatpump_status_t status) {
    // Update Matter temperature when status changes
    matter_update_attributes();
}

int main(void) {
    // Initialize subsystems
    heatpump_init();
    matter_init();
    state_sync_init();
    
    // Register callbacks
    heatpump_set_settings_callback(settings_changed);
    heatpump_set_status_callback(status_changed);
    
    // Connect to heat pump
    heatpump_connect();
    
    // Start Matter
    matter_start();
    
    // Main loop
    while (1) {
        heatpump_sync();
        matter_process_attribute_writes();
        state_sync_periodic();
        
        k_sleep(K_MSEC(100));
    }
    
    return 0;
}
```
