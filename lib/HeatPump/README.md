# HeatPump Library

This directory will contain the adapted SwiCago/HeatPump library for use with Zephyr RTOS.

## Original Source

- **Repository**: https://github.com/SwiCago/HeatPump
- **License**: GPL-3.0
- **Description**: Arduino library for controlling Mitsubishi heat pumps via the CN105 connector

## Adaptations Needed

The original Arduino library needs to be adapted for Zephyr RTOS:

1. **UART Communication**: Replace Arduino Serial with Zephyr UART API
2. **Timing**: Replace Arduino `millis()` with Zephyr `k_uptime_get()`
3. **Memory Management**: Use Zephyr's memory pools if needed
4. **Threading**: Use Zephyr threads instead of Arduino loop
5. **Callbacks**: Adapt callback mechanism for Zephyr environment

## Integration Status

⚠️ **Not yet integrated** - This is a placeholder for future work.

## License Compliance

This project is licensed under GPL-3.0 to comply with the SwiCago/HeatPump library license requirements.
