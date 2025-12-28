# Matter CN105 Heat Pump Controller

A Matter-compatible controller for Mitsubishi heat pumps using the CN105 connector interface. Built with Zephyr RTOS for Arduino Nano Matter.

## Overview

This project enables control of Mitsubishi heat pumps (mini-splits) through the Matter smart home protocol by interfacing with the CN105 service connector. It bridges the proprietary Mitsubishi protocol with the open Matter standard, allowing integration with Apple Home, Google Home, Amazon Alexa, and other Matter-compatible ecosystems.

### Key Features

- **Matter/Thread Integration**: Full Matter protocol support over Thread network
- **Thermostat Control**: Temperature setpoint, mode (heat/cool/auto/dry/fan), on/off
- **Fan Control**: Multiple fan speeds (quiet/1/2/3/4/auto)
- **Vane Control**: Vertical and horizontal air direction
- **Status Monitoring**: Room temperature, operating state, compressor frequency
- **Bidirectional Sync**: Changes from IR remote or physical controls reflected in Matter
- **Custom Clusters**: Vendor-specific features exposed through custom Matter clusters

## Hardware Requirements

- **Arduino Nano Matter** - MGM240P-based Matter development board
- **Buck Converter** - 12V to 5V DC-DC step-down converter (e.g., Mini360 or similar)
- **SparkFun Logic Level Converter BOB-12009** - For 3.3V ↔ 5V level shifting
- **Mitsubishi Heat Pump** with CN105 connector (see compatibility list)
- **CN105 Connector** - 5-pin JST PA connector or equivalent wiring
- **Jumper Wires** - For connections between components

### Supported Heat Pump Models

This controller is designed for Mitsubishi heat pumps compatible with the CN105 interface, including:

- **MSZ-GS Series** (MSZ-GS06NA, MSZ-GS09NA, MSZ-GS12NA, etc.)
- **MSZ-GE Series**
- **MSZ-FH Series**
- **MSZ-GL Series**
- **MUZ-FH Series**
- Many other models - see [SwiCago/HeatPump compatibility list](https://github.com/SwiCago/HeatPump#supported-models)

**Note**: Not all features (e.g., i-See sensor, wide vane) are available on all models.

## Software Requirements

- **Zephyr RTOS SDK** (version 3.5 or later)
- **West** - Zephyr meta-tool
- **CMake** (3.20+)
- **Python 3** with pip
- **Git**
- **ARM GCC Toolchain**

## Quick Start

### 1. Hardware Setup

⚠️ **WARNING**: Read the detailed [Wiring Diagram](docs/WIRING.md) before connecting any hardware. Improper wiring can damage your equipment.

**Summary**:
1. Connect CN105 12V (Pin 1) to buck converter input
2. Connect buck converter 5V output to Arduino Nano Matter power
3. Connect CN105 connector to SparkFun BOB-12009 (5V side) for data lines
4. Connect BOB-12009 (3.3V side) to Arduino Nano Matter UART
5. Ensure common ground between buck converter, voltage translator, and Arduino Nano Matter

See [docs/WIRING.md](docs/WIRING.md) for complete wiring instructions with diagrams.

### 2. Software Setup

```bash
# Install Zephyr SDK (follow official guide)
# https://docs.zephyrproject.org/latest/develop/getting_started/

# Clone this repository into your Zephyr workspace
cd ~/zephyrproject
git clone https://github.com/jwinarske/matter-cn105.git
cd matter-cn105

# Build the firmware
west build -b arduino_nano_matter

# Flash to device
west flash
```

See [docs/INSTALLATION.md](docs/INSTALLATION.md) for detailed installation instructions.

### 3. Commission with Matter

```bash
# Example with chip-tool (Matter CLI tool)
chip-tool pairing onnetwork 1 20202021
```

The device will appear as a thermostat in your Matter controller.

## Documentation

- **[Wiring Diagram](docs/WIRING.md)** - Detailed hardware connection guide with safety warnings
- **[Installation Guide](docs/INSTALLATION.md)** - Complete setup and installation instructions
- **[API Documentation](docs/API.md)** - Software API reference and examples
- **[Matter Cluster Mapping](docs/MATTER_CLUSTERS.md)** - How heat pump features map to Matter

## Project Structure

```
matter-cn105/
├── CMakeLists.txt           # Zephyr build configuration
├── prj.conf                 # Zephyr project configuration
├── Kconfig                  # Configuration options
├── README.md                # This file
├── LICENSE                  # GPLv3 license
├── boards/
│   └── arduino_nano_matter.overlay  # Device tree overlay for UART
├── src/
│   ├── main.c                       # Main entry point
│   ├── heatpump_driver.cpp          # Heat pump driver implementation
│   ├── heatpump_driver.h            # Driver interface
│   ├── matter_integration.cpp       # Matter stack integration
│   ├── state_sync.cpp               # Bidirectional state sync
│   └── attribute_handlers.cpp       # Matter attribute callbacks
├── include/
│   ├── heatpump_types.h     # Heat pump data structures
│   └── matter_config.h      # Matter cluster definitions
├── lib/
│   └── HeatPump/            # Adapted SwiCago/HeatPump library
├── docs/
│   ├── WIRING.md            # Wiring diagram
│   ├── INSTALLATION.md      # Installation guide
│   ├── API.md               # API documentation
│   └── MATTER_CLUSTERS.md   # Matter cluster mapping
└── scripts/
    └── flash.sh             # Helper flash script
```

## Matter Capabilities

### Standard Clusters

- **On/Off (0x0006)**: Power control
- **Thermostat (0x0201)**: Temperature and mode control
- **Fan Control (0x0202)**: Fan speed control
- **Temperature Measurement (0x0402)**: Current temperature reading

### Custom Vendor Clusters

- **Vane Control (0xFFF1FC01)**: Vertical air vane position
- **Wide Vane Control (0xFFF1FC02)**: Horizontal air vane position
- **Heat Pump Status (0xFFF1FC03)**: Compressor frequency and operating state
- **i-See Control (0xFFF1FC04)**: i-See sensor control (if available)

See [docs/MATTER_CLUSTERS.md](docs/MATTER_CLUSTERS.md) for detailed cluster mapping.

## Safety and Disclaimer

⚠️ **IMPORTANT SAFETY INFORMATION**:

1. **High Voltage**: Heat pumps contain dangerous high-voltage components. Always disconnect power before working on the unit.
2. **Warranty**: Connecting to the CN105 connector may void your heat pump warranty.
3. **Proper Installation**: Ensure all connections are secure and properly insulated.
4. **Professional Installation**: If you're not comfortable with electrical work, hire a qualified technician.
5. **Liability**: The authors accept no liability for damage, injury, or loss resulting from use of this project.

Read the complete safety warnings in [docs/WIRING.md](docs/WIRING.md).

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes with clear commit messages
4. Test thoroughly
5. Submit a pull request

## Credits

This project builds upon excellent prior work:

- **[SwiCago/HeatPump](https://github.com/SwiCago/HeatPump)** - Arduino library for Mitsubishi heat pumps (GPL-3.0)
- **[ncaunt/meldec](https://github.com/ncaunt/meldec)** - Python implementation of the MELCloud protocol
- **Arduino Nano Matter** - Silicon Labs MGM240P-based Matter development board
- **Matter SDK** - Connectivity Standards Alliance (CSA) Matter protocol implementation

## License

This project is licensed under the **GNU General Public License v3.0** (GPL-3.0) to comply with the SwiCago/HeatPump library license requirements.

See [LICENSE](LICENSE) for the full license text.

Key points:
- You may use, modify, and distribute this code
- Any derivative works must also be GPL-3.0 licensed
- No warranty is provided

## Support

- **Issues**: Please report bugs and feature requests via [GitHub Issues](https://github.com/jwinarske/matter-cn105/issues)
- **Discussions**: For questions and discussions, use [GitHub Discussions](https://github.com/jwinarske/matter-cn105/discussions)

## Roadmap

Future enhancements:

- [ ] Matter OTA (Over-The-Air) firmware updates
- [ ] Timer control integration
- [ ] Energy monitoring (if supported by heat pump)
- [ ] Multi-zone support for multi-split systems
- [ ] Enhanced diagnostic information
- [ ] Web-based configuration interface

## Version

**Version**: 0.1.0 (Initial Development)

**Status**: Work in Progress (WIP)

---

**Made with ❄️ for climate control**
