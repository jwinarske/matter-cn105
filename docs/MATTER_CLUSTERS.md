# Matter Cluster Mapping

This document describes how Mitsubishi heat pump features are mapped to Matter clusters and attributes.

## Device Type

**Matter Device Type**: Thermostat (0x0301)

The heat pump is exposed as a Matter thermostat device, which provides standard climate control capabilities.

## Standard Matter Clusters

### 1. Identify Cluster (0x0003)

Standard identification cluster for device discovery.

**Attributes:**
- `IdentifyTime` (0x0000): Time remaining in identify mode

**Commands:**
- `Identify`: Put device in identify mode

### 2. On/Off Cluster (0x0006)

Controls power state of the heat pump.

**Attributes:**
- `OnOff` (0x0000): Power state
  - `false` = OFF
  - `true` = ON

**Commands:**
- `Off`: Turn off heat pump
- `On`: Turn on heat pump
- `Toggle`: Toggle power state

**Mapping:**
- Matter On/Off → Heat Pump `power` setting ("ON"/"OFF")

### 3. Thermostat Cluster (0x0201)

Primary cluster for temperature control and mode selection.

#### Attributes

| Attribute | ID | Type | Description | Heat Pump Mapping |
|-----------|-----|------|-------------|-------------------|
| LocalTemperature | 0x0000 | int16 | Current room temperature (0.01°C) | `status.roomTemperature` |
| OccupiedCoolingSetpoint | 0x0011 | int16 | Cooling setpoint (0.01°C) | `settings.temperature` (when mode=COOL) |
| OccupiedHeatingSetpoint | 0x0012 | int16 | Heating setpoint (0.01°C) | `settings.temperature` (when mode=HEAT) |
| SystemMode | 0x001C | enum8 | Operating mode | See mode mapping below |
| ThermostatRunningState | 0x0029 | bitmap16 | Current running state | `status.operating` |

#### System Mode Mapping

| Matter Mode | Value | Heat Pump Mode |
|-------------|-------|----------------|
| Off | 0x00 | power="OFF" |
| Auto | 0x01 | mode="AUTO" |
| Cool | 0x03 | mode="COOL" |
| Heat | 0x04 | mode="HEAT" |
| Dry/Precool | 0x08 | mode="DRY" |
| FanOnly | 0x07 | mode="FAN" |

#### Running State Mapping

| Bit | Description | Heat Pump Status |
|-----|-------------|------------------|
| 0 | Heat | `operating && mode=="HEAT"` |
| 1 | Cool | `operating && mode=="COOL"` |
| 2 | Fan | `operating && mode=="FAN"` |
| 13 | Heat (2nd stage) | Reserved |
| 14 | Cool (2nd stage) | Reserved |

### 4. Fan Control Cluster (0x0202)

Controls fan speed.

#### Attributes

| Attribute | ID | Type | Description | Heat Pump Mapping |
|-----------|-----|------|-------------|-------------------|
| FanMode | 0x0000 | enum8 | Fan mode | See fan mapping below |
| FanModeSequence | 0x0001 | enum8 | Supported fan modes | Auto/Low/Med/High/On |
| PercentSetting | 0x0002 | uint8 | Fan speed percentage | Calculated from fan setting |
| PercentCurrent | 0x0003 | uint8 | Current fan speed | Calculated from fan setting |

#### Fan Mode Mapping

| Matter Fan Mode | Value | Heat Pump Fan Setting |
|-----------------|-------|----------------------|
| Off | 0x00 | "QUIET" (mapped) |
| Low | 0x01 | "1" |
| Medium | 0x02 | "2" |
| High | 0x03 | "4" |
| On | 0x04 | "3" |
| Auto | 0x05 | "AUTO" |

**Note**: Heat pump has 4 speed levels (1-4) plus AUTO and QUIET. Mapping:
- QUIET → Off/Low
- 1 → Low
- 2 → Medium
- 3 → On
- 4 → High
- AUTO → Auto

### 5. Temperature Measurement Cluster (0x0402)

Provides current temperature reading.

**Attributes:**
- `MeasuredValue` (0x0000): Current temperature in 0.01°C
  - Mapped from `status.roomTemperature`
- `MinMeasuredValue` (0x0001): Minimum measurable value
- `MaxMeasuredValue` (0x0002): Maximum measurable value

## Custom Vendor-Specific Clusters

These clusters provide access to Mitsubishi-specific features not available in standard Matter clusters.

### 1. Vane Control Cluster (0xFFF1FC01)

Controls vertical air vane position.

**Attributes:**
- `VanePosition` (0x0000): Vertical vane position (enum8)
  - 0 = AUTO
  - 1 = Position 1 (highest)
  - 2 = Position 2
  - 3 = Position 3
  - 4 = Position 4
  - 5 = Position 5 (lowest)
  - 6 = SWING

**Mapping:**
- Matter vane position → Heat Pump `settings.vane`

### 2. Wide Vane Control Cluster (0xFFF1FC02)

Controls horizontal air vane position.

**Attributes:**
- `WideVanePosition` (0x0000): Horizontal vane position (enum8)
  - 0 = Far Left ("<<")
  - 1 = Left ("<")
  - 2 = Center ("|")
  - 3 = Right (">")
  - 4 = Far Right (">>")
  - 5 = Wide ("<>")
  - 6 = SWING

**Mapping:**
- Matter wide vane position → Heat Pump `settings.wideVane`

### 3. Heat Pump Status Cluster (0xFFF1FC03)

Provides extended status information.

**Attributes:**
- `CompressorFrequency` (0x0000): Compressor frequency in Hz (uint16)
  - Mapped from `status.compressorFrequency`
- `Operating` (0x0001): Boolean indicating if actively heating/cooling
  - Mapped from `status.operating`

### 4. i-See Sensor Control Cluster (0xFFF1FC04)

Controls the i-See sensor feature (if available on the heat pump model).

**Attributes:**
- `ISeeEnabled` (0x0000): i-See sensor enabled (bool)
  - Mapped from `settings.iSee`

**Note**: Not all Mitsubishi models support i-See. Check your specific model.

## Endpoint Configuration

### Endpoint 0 (Root)

**Device Type**: Root Node

**Clusters:**
- Descriptor (0x001D)
- Basic Information (0x0028)
- General Commissioning (0x0030)
- Network Commissioning (0x0031)
- General Diagnostics (0x0033)
- Thread Network Diagnostics (0x0035)

### Endpoint 1 (Thermostat)

**Device Type**: Thermostat (0x0301)

**Server Clusters:**
- Identify (0x0003)
- On/Off (0x0006)
- Descriptor (0x001D)
- Thermostat (0x0201)
- Fan Control (0x0202)
- Temperature Measurement (0x0402)
- Vane Control (0xFFF1FC01) - Custom
- Wide Vane Control (0xFFF1FC02) - Custom
- Heat Pump Status (0xFFF1FC03) - Custom
- i-See Control (0xFFF1FC04) - Custom

## Temperature Conversion

Matter uses temperature in units of 0.01°C (hundredths of degrees Celsius).

**Conversion Macros:**
```c
#define CELSIUS_TO_MATTER_TEMP(temp) ((int16_t)((temp) * 100))
#define MATTER_TEMP_TO_CELSIUS(temp) ((float)(temp) / 100.0f)
```

**Example:**
- 22.5°C → 2250 (Matter units)
- 1850 (Matter units) → 18.5°C

## Supported Temperature Range

- **Minimum**: 16.0°C (1600 in Matter units)
- **Maximum**: 31.0°C (3100 in Matter units)
- **Step**: 0.5°C (50 in Matter units) or 1.0°C depending on model

## State Synchronization

The controller maintains bidirectional synchronization:

1. **Heat Pump → Matter**: When the heat pump state changes (via IR remote or physical controls), the Matter attributes are updated
2. **Matter → Heat Pump**: When a Matter controller changes an attribute, the heat pump settings are updated

Synchronization occurs:
- Immediately on state changes (via callbacks)
- Periodically (every 5 seconds by default)
- On explicit sync requests

## Compatibility Notes

### Supported Models

This controller is designed for Mitsubishi heat pumps compatible with the CN105 interface, including but not limited to:

- MSZ-GS/GE series
- MSZ-FH series  
- MSZ-GL series
- MUZ-FH series

Refer to the [SwiCago/HeatPump compatibility list](https://github.com/SwiCago/HeatPump#supported-models) for more models.

### Feature Availability

Not all features are available on all models:
- **i-See sensor**: Only on i-See equipped models
- **Wide vane**: Only on models with horizontal vanes
- **Compressor frequency**: May not be available on all models

The controller will report unavailable features appropriately in the Matter data model.

## Future Enhancements

Potential future additions:
- Timer control clusters
- Energy monitoring (if supported by heat pump)
- Advanced scheduling
- Multi-zone support (for multi-split systems)
- Maintenance reminders
