# Installation Guide

## Prerequisites

### Hardware Requirements

- **Arduino Nano Matter** board
- **Buck Converter** - Adafruit 5V 1A Buck Converter (Product ID: 4739)
- **SparkFun Logic Level Converter BOB-12009**
- **Mitsubishi Heat Pump** with CN105 connector (e.g., MSZ-GS06NA)
- **CN105 Connector** (5-pin JST PA connector) or equivalent wiring
- **USB Cable** for programming and optional power
- **Jumper Wires** for connections

### Software Requirements

- **Zephyr RTOS SDK** (version 3.x or later)
- **West** (Zephyr's meta-tool)
- **CMake** (version 3.20 or later)
- **Python 3** (for build tools)
- **Git**

## Setting Up the Development Environment

### 1. Install Zephyr SDK

Follow the official Zephyr Getting Started Guide:
https://docs.zephyrproject.org/latest/develop/getting_started/index.html

```bash
# Example for Linux
west init ~/zephyrproject
cd ~/zephyrproject
west update
west zephyr-export
pip3 install -r ~/zephyrproject/zephyr/scripts/requirements.txt
```

### 2. Clone This Repository

```bash
cd ~/zephyrproject
git clone https://github.com/jwinarske/matter-cn105.git
cd matter-cn105
```

### 3. Build the Firmware

```bash
west build -b arduino_nano_matter
```

### 4. Flash the Firmware

```bash
west flash
# or use the helper script
./scripts/flash.sh
```

## Hardware Installation

⚠️ **WARNING**: Read all safety warnings in `docs/WIRING.md` before proceeding!

### Step 1: Wiring

Follow the detailed wiring instructions in [`docs/WIRING.md`](WIRING.md).

Key connections:
- CN105 Pin 1 (12V) → Buck Converter input
- Buck Converter output (5V) → Arduino Nano Matter power
- CN105 data pins ↔ SparkFun BOB-12009 (5V side)
- BOB-12009 (3.3V side) ↔ Arduino Nano Matter UART
- Common ground between buck converter, voltage translator, and Arduino Nano Matter

### Step 2: Power

**Important**: Before applying power, verify Adafruit 4739 output voltage is 5.0V (±0.1V). The Adafruit 4739 has a fixed 5V output and requires no adjustment.

Options for powering the system:
1. **Adafruit 4739 Buck Converter from CN105** (recommended for production): 
   - Use 12V from CN105 Pin 1 through Adafruit 4739 to power Arduino
   - Provides integrated single-power-source solution with fixed 5V output
2. **USB Power** (recommended for development): 
   - Power Arduino via USB for easy testing and debugging
   - Can still be used alongside CN105 connection

### Step 3: Physical Installation

1. Turn off power to the heat pump
2. Open the heat pump indoor unit cover
3. Locate the CN105 connector on the control board
4. Connect your wiring harness (including Adafruit 4739 buck converter)
5. Route wires safely away from high voltage components
6. Mount the Adafruit 4739 securely using its mounting holes (use adhesive or mounting bracket)
7. Secure the Arduino and logic level converter
8. Ensure all connections are insulated and secure
9. Close the cover

## Matter Commissioning

### 1. Put Device in Commissioning Mode

The device should automatically enter commissioning mode on first boot.

### 2. Commission with Matter Controller

Use a Matter-compatible controller (e.g., Apple Home, Google Home, or chip-tool):

```bash
# Example with chip-tool
chip-tool pairing onnetwork 1 20202021
```

### 3. Configure Heat Pump

Once commissioned, the heat pump will appear as a thermostat in your Matter ecosystem.

## Verification

### Check Serial Output

```bash
screen /dev/ttyUSB0 115200
```

You should see:
- Initialization messages
- Heat pump connection status
- Matter commissioning information

### Test Basic Functions

Try changing:
- Temperature setpoint
- Operating mode (heat/cool/auto)
- Fan speed

## Troubleshooting

### Device Not Connecting to Heat Pump

1. Check UART connections (TX/RX not swapped)
2. Verify logic level converter power
3. Check common ground
4. Verify Adafruit 4739 is providing stable 5V to Arduino
5. Monitor serial output for errors

### Matter Commissioning Fails

1. Ensure device is in commissioning mode
2. Check Thread network credentials
3. Verify Matter controller compatibility
4. Check logs for specific errors

### No Response from Heat Pump

1. Verify heat pump model compatibility
2. Check CN105 connector pin assignment
3. Confirm UART baud rate (2400, 8E1)
4. Test with known-working hardware

## Additional Resources

- [Wiring Diagram](WIRING.md)
- [API Documentation](API.md)
- [Matter Cluster Mapping](MATTER_CLUSTERS.md)
- [SwiCago/HeatPump Library](https://github.com/SwiCago/HeatPump)
