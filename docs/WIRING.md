# Wiring Diagram - Matter Controller for Mitsubishi CN105

## Overview

This document provides detailed wiring instructions for connecting the Arduino Nano Matter (3.3V logic) to the Mitsubishi heat pump CN105 connector (5V logic) using a SparkFun Logic Level Converter (BOB-12009).

**⚠️ WARNING: Read all safety warnings before proceeding with installation.**

## Components Required

1. **Arduino Nano Matter** - 3.3V logic microcontroller with Matter support
2. **Buck Converter** - 12V to 5V DC-DC step-down converter (e.g., Mini360, LM2596, or similar)
3. **SparkFun Logic Level Converter BOB-12009** - Bidirectional logic level converter
4. **Mitsubishi CN105 Connector** - 5-pin JST PA connector (or equivalent)
5. **Jumper Wires** - For connections

## Mitsubishi CN105 Pinout

The CN105 connector on Mitsubishi heat pumps (MSZ-GS06NA and compatible models) uses a 5-pin JST PA connector:

```
CN105 Connector Pinout (looking at the heat pump PCB connector):
┌─────────────────┐
│ 1  2  3  4  5   │
└─────────────────┘

Pin 1: 12V    (DC power - used to power buck converter)
Pin 2: GND    (Ground - Common reference)
Pin 3: 5V     (5V DC power output from heat pump)
Pin 4: TX     (Heat pump transmit, 5V logic)
Pin 5: RX     (Heat pump receive, 5V logic)
```

**Important Notes:**
- Pin 1 (12V) connects to the buck converter input to provide power for the Arduino
- Pin 2 (GND) is the common ground for all components
- Pin 3 provides 5V power (used to power the logic level converter)
- Pins 4 and 5 use 5V TTL serial communication at 2400 baud, 8E1 (8 data bits, even parity, 1 stop bit)

## Buck Converter (12V to 5V)

A buck converter (DC-DC step-down converter) is used to convert the CN105's 12V output to 5V for powering the Arduino Nano Matter.

### Recommended Buck Converter

- **Adafruit 5V 1A Buck Converter** (Product ID: 4739)
  - Fixed 5V output at up to 1000mA
  - Input voltage range: 4.5V to 32V
  - High efficiency (up to 90%)
  - Compact size with mounting holes
  - Built-in overcurrent and thermal protection
  - Available at: https://www.adafruit.com/product/4739

### Buck Converter Configuration

The Adafruit 4739 buck converter has a fixed 5V output and does not require adjustment:

1. **Input Voltage**: 12V from CN105 Pin 1
2. **Output Voltage**: Fixed 5V output (no adjustment needed)
3. **Output Current**: Up to 1000mA (Arduino Nano Matter typically draws 100-200mA)

**Connection Procedure:**
1. Connect 12V input from CN105 Pin 1 to VIN on buck converter
2. Connect GND from CN105 Pin 2 to GND on buck converter
3. Verify output voltage with multimeter before connecting to Arduino (should read 5.0V ±0.1V)
4. Connect VOUT (5V) to Arduino Nano Matter VIN pin
5. Connect GND to Arduino GND

### Adafruit 4739 Buck Converter Pinout

```
Adafruit 4739 Buck Converter:
┌──────────────────┐
│                  │
│  VIN    VOUT     │  VIN:  12V input (from CN105 Pin 1)
│  GND    GND      │  GND:  Ground (from CN105 Pin 2)
│                  │  VOUT: 5V output (to Arduino Nano Matter VIN)
│  [Enable Pin]    │  GND:  Ground output (to Arduino GND)
└──────────────────┘
```

**Note:** The Adafruit 4739 has an enable pin that can be left unconnected (default enabled) or connected to a control signal if power control is needed.

**Important:**
- The Adafruit 4739 has a fixed 5V output, eliminating adjustment errors
- Rated for continuous operation with 12V input
- Built-in overcurrent and thermal protection for safety
- Compact design with mounting holes for secure installation

## SparkFun BOB-12009 Logic Level Converter

The SparkFun BOB-12009 is a bidirectional logic level converter that safely converts between 3.3V and 5V logic levels.

### BOB-12009 Pinout

```
High Voltage Side (HV)     Low Voltage Side (LV)
┌──────────────────────────────────────┐
│  HV    GND   HV1   HV2   HV3   HV4  │
│                                       │
│  LV    GND   LV1   LV2   LV3   LV4  │
└──────────────────────────────────────┘
```

**Pin Functions:**
- **HV**: High voltage reference (connect to 5V from CN105)
- **LV**: Low voltage reference (connect to 3.3V from Arduino)
- **GND**: Ground (must be common between both sides)
- **HV1-HV4**: High voltage signal channels (5V side)
- **LV1-LV4**: Low voltage signal channels (3.3V side)

### BOB-12009 Connections for This Project

Only 2 channels are needed for UART communication:

- **HV** → CN105 Pin 3 (5V)
- **LV** → Arduino Nano Matter 3.3V output
- **GND (both)** → Common ground
- **HV1** → CN105 Pin 4 (TX from heat pump)
- **LV1** → Arduino Nano Matter RX pin
- **HV2** → CN105 Pin 5 (RX to heat pump)
- **LV2** → Arduino Nano Matter TX pin
- **HV3, HV4, LV3, LV4** → Not used (leave disconnected)

## Arduino Nano Matter Connections

The Arduino Nano Matter uses 3.3V logic and must not be directly connected to 5V signals.

### Arduino Nano Matter UART Pins

The Arduino Nano Matter (based on MGM240P) hardware UART pins:
- **TX (Transmit)**: Pin D1 (or GPIO as configured in device tree)
- **RX (Receive)**: Pin D0 (or GPIO as configured in device tree)
- **3.3V**: 3.3V power output
- **GND**: Ground

**Note**: The exact pin mapping depends on the device tree overlay configuration. See `boards/arduino_nano_matter.overlay` for specific GPIO assignments.

### Arduino Nano Matter to BOB-12009 Connections

- **3.3V** → BOB-12009 LV
- **GND** → BOB-12009 GND (and common ground)
- **TX (D1)** → BOB-12009 LV2
- **RX (D0)** → BOB-12009 LV1

## Complete Wiring Diagram

```
Mitsubishi Heat Pump CN105     Adafruit 4739 Buck Converter   SparkFun BOB-12009             Arduino Nano Matter
┌─────────────────────┐         ┌──────────────┐               Logic Level Converter          ┌──────────────────┐
│                     │         │              │                                               │                  │
│  Pin 1: 12V  ───────┼─────────┼─── VIN      │                                               │                  │
│                     │         │              │                                               │                  │
│  Pin 2: GND  ───────┼─────────┼─── GND      │                                               │                  │
│                     │         │              │                                               │                  │
│                     │         │     VOUT ────┼───────────────────────────────────────────────┼─── VIN (5V)     │
│                     │         │              │                                               │                  │
│                     │         │     GND  ────┼───────────────────────┬───────────────────────┼─── GND          │
│                     │         └──────────────┘                       │                       │                  │
│  Pin 3: 5V   ────────────────────────────┬─── HV (5V reference)     │                       │                  │
│                     │                     │                          │                       │                  │
│  Pin 4: TX   ────────────────────────────┼─── HV1                   │                       │                  │
│  (5V logic)         │                     │                          │                       │                  │
│                     │                     │     LV1 ─────────────────┼───────────────────────┼─── RX (D0)      │
│  Pin 5: RX   ────────────────────────────┼─── HV2                   │                       │    (3.3V logic) │
│  (5V logic)         │                     │                          │                       │                  │
│                     │                     │     LV2 ─────────────────┼───────────────────────┼─── TX (D1)      │
└─────────────────────┘                     │                          │                       │    (3.3V logic) │
                                            │                          │                       │                  │
                                            │     LV (3.3V ref) ───────┼───────────────────────┼─── 3.3V         │
                                            │                          │                       │                  │
                                            └─── GND (HV side) ────────┴───────────────────────┼─── GND          │
                                                                                               │                  │
                                                                                               └──────────────────┘

Common Ground: All GND connections (CN105 Pin 2, Adafruit 4739 GND, BOB-12009 GND, Arduino GND) must be tied together
```

## Detailed Connection Table

| From                    | To                         | Wire Color Suggestion | Notes                            |
|-------------------------|----------------------------|-----------------------|----------------------------------|
| CN105 Pin 1 (12V)       | Adafruit 4739 VIN          | Red                   | 12V power input                  |
| CN105 Pin 2 (GND)       | Adafruit 4739 GND          | Black                 | Common ground                    |
| CN105 Pin 2 (GND)       | BOB-12009 GND (HV side)    | Black                 | Common ground                    |
| CN105 Pin 3 (5V)        | BOB-12009 HV               | Red                   | 5V reference for HV side         |
| CN105 Pin 4 (TX)        | BOB-12009 HV1              | Yellow                | Heat pump transmits              |
| CN105 Pin 5 (RX)        | BOB-12009 HV2              | Green                 | Heat pump receives               |
| Adafruit 4739 VOUT      | Arduino VIN (or 5V pin)    | Red                   | 5V power to Arduino (fixed 5V)   |
| Adafruit 4739 GND       | Arduino GND                | Black                 | Common ground                    |
| BOB-12009 LV1           | Arduino RX (D0)            | Orange                | Arduino receives                 |
| BOB-12009 LV2           | Arduino TX (D1)            | Blue                  | Arduino transmits                |
| BOB-12009 LV            | Arduino 3.3V               | Red                   | 3.3V reference for LV side       |
| BOB-12009 GND (LV side) | Arduino GND                | Black                 | Common ground                    |

**Critical**: All GND connections (CN105 Pin 2, Adafruit 4739 GND, BOB-12009 GND on both sides, Arduino GND) must be connected together to establish a common ground reference.

## Power Supply Recommendations

### Option 1: Adafruit 4739 Buck Converter from CN105 12V (Recommended for Production)

The recommended approach is to use the Adafruit 4739 buck converter to step down the CN105's 12V (Pin 1) to 5V for the Arduino Nano Matter:

**Advantages:**
- Single power source from heat pump
- No external power supply needed
- Clean and integrated installation
- Reliable power from heat pump's regulated supply
- Fixed 5V output eliminates adjustment errors
- Built-in overcurrent and thermal protection

**Configuration:**
1. **Buck Converter Input**: Connect CN105 Pin 1 (12V) to VIN and Pin 2 (GND) to GND
2. **Buck Converter Output**: Connect VOUT (5V) to Arduino Nano Matter VIN pin
3. **Logic Level Converter**: Powered from CN105 Pin 3 (5V) for HV side, Arduino 3.3V for LV side
4. **Common Ground**: All grounds must be connected together

**Important Considerations:**
- The Adafruit 4739 provides a fixed 5V output (no adjustment needed)
- Rated for up to 1000mA output (Arduino typically draws 100-200mA)
- Built-in overcurrent and thermal protection for safety
- The CN105 12V supply is designed for accessories and should handle the Arduino's power requirements

### Option 2: USB Power (Recommended for Development/Testing)

For initial testing and development, power the Arduino via USB:

**Advantages:**
- Easy to connect and disconnect for development
- No risk of damaging Arduino with incorrect voltage
- Can monitor serial output during development
- Isolated power supply for testing

**Configuration:**
1. **Arduino Power**: USB cable to computer or USB power adapter
2. **Logic Level Converter**: Powered from CN105 Pin 3 (5V) for HV side, Arduino 3.3V for LV side
3. **Common Ground**: All grounds must be connected together (USB GND via Arduino)

**Note**: This option requires the Arduino to be accessible for USB connection, making it less suitable for permanent installations.

### Option 3: External 5V Power Supply (Alternative)

Use a separate regulated 5V power supply if the CN105 12V is insufficient or for specialized applications:

**Configuration:**
1. **External Supply**: Regulated 5V power supply (at least 500mA)
2. **Arduino Power**: Connect 5V supply to Arduino VIN pin
3. **Logic Level Converter**: Can use external 5V or CN105 Pin 3 (5V) for HV side
4. **Common Ground**: All grounds must be connected together, including external supply ground

### Power Supply Selection Guide

| Use Case                  | Recommended Option | Notes                                    |
|---------------------------|--------------------|------------------------------------------|
| Permanent Installation    | Buck Converter     | Clean, integrated, single power source   |
| Development/Testing       | USB Power          | Easy testing and debugging               |
| Bench Testing             | USB Power          | Isolate from heat pump during testing    |
| High-Reliability Systems  | Buck Converter     | No external cables or supplies needed    |

**Important**: Regardless of the power option chosen, ensure proper voltage regulation and that all grounds are connected together to establish a common reference.

## Serial Communication Parameters

The Mitsubishi heat pump CN105 interface uses:
- **Baud Rate**: 2400 bps
- **Data Bits**: 8
- **Parity**: Even
- **Stop Bits**: 1
- **Flow Control**: None

Configure the Arduino Nano Matter UART with these exact parameters (8E1, 2400 baud).

## Safety Warnings

### ⚠️ ELECTRICAL SAFETY

1. **HIGH VOLTAGE WARNING**: The heat pump contains dangerous high voltage AC circuits. The CN105 connector is low voltage (5V/12V DC), but always disconnect power before working on the heat pump.

2. **ADAFRUIT 4739 BUCK CONVERTER SAFETY**:
   - The Adafruit 4739 has a fixed 5V output (no adjustment needed)
   - Use a reliable multimeter to verify output voltage is 5.0V before connecting to Arduino
   - Check polarity: VIN to CN105 12V, GND to CN105 GND, VOUT to Arduino VIN
   - Built-in overcurrent protection (up to 1000mA)
   - Ensure adequate ventilation for the buck converter during operation

3. **ESD PROTECTION**: Use proper ESD protection when handling electronic components. Discharge static electricity before touching the heat pump PCB.

4. **PROPER INSTALLATION**: 
   - Ensure all connections are secure and properly insulated
   - Do not leave exposed conductors that could short circuit
   - Use appropriate gauge wire for all connections (22-24 AWG recommended)
   - Double-check polarity before applying power
   - Verify Adafruit 4739 connections: VIN to 12V, GND to ground, VOUT to Arduino VIN

5. **VOLTAGE LEVELS**:
   - Never connect 5V signals directly to Arduino Nano Matter (3.3V) pins
   - Always use the logic level converter for voltage translation
   - Verify voltage levels with a multimeter before making connections
   - Verify Adafruit 4739 VOUT is 5.0V (±0.1V) before connecting to Arduino

6. **POWER SUPPLY**:
   - The Adafruit 4739 provides stable regulated 5V output
   - Use multimeter to verify output voltage is within specification
   - Do not exceed 1000mA output current rating
   - Use a separate power supply for Arduino if needed during testing
   - Ensure proper ventilation for the buck converter

### ⚠️ WARRANTY AND LIABILITY

1. **WARRANTY VOID**: Opening the heat pump and connecting to internal connectors will likely **VOID THE WARRANTY**. Proceed at your own risk.

2. **PROFESSIONAL INSTALLATION**: If you are not comfortable with electrical work, hire a qualified technician.

3. **COMPLIANCE**: Ensure your installation complies with local electrical codes and regulations.

4. **DAMAGE RISK**: Incorrect wiring can damage the heat pump controller, Arduino, or both. Verify all connections before applying power.

5. **NO LIABILITY**: The authors and contributors of this project accept no liability for any damage, injury, or loss resulting from following these instructions.

### ⚠️ TESTING RECOMMENDATIONS

1. **Adafruit 4739 Buck Converter Testing**:
   - Test Adafruit 4739 with 12V input BEFORE connecting to Arduino
   - Verify output voltage is 5.0V (±0.1V) using multimeter
   - The fixed 5V output requires no adjustment
   - Verify output voltage stability under no-load condition
   - Test with a 100Ω resistor load to verify regulation
   - Check for excessive ripple voltage (<100mV peak-to-peak)
   - Confirm buck converter doesn't overheat during operation
   - Verify enable pin behavior if using power control (default is enabled)

2. **Bench Test First**: 
   - Test all connections on a workbench before installing in the heat pump
   - Use a multimeter to verify connections and voltage levels
   - Test serial communication before connecting to the actual heat pump
   - Verify buck converter output remains stable under load

3. **Incremental Testing**:
   - First, test the Adafruit 4739 output voltage alone
   - Then test the Arduino Nano Matter with buck converter power
   - Add the logic level converter and verify signal integrity
   - Finally, connect to the heat pump and test communication

4. **Monitor Temperature**: Ensure no components overheat during operation, especially the buck converter under load

5. **Documentation**: Document your specific installation with photos for future reference

## Troubleshooting

### No Communication with Heat Pump

1. **Check Connections**: Verify all wiring matches the diagram above
2. **Verify Voltage Levels**: Use a multimeter to check 12V, 5V and 3.3V supplies
3. **Check Serial Parameters**: Ensure UART is configured for 2400 baud, 8E1
4. **Test Continuity**: Check for broken wires or poor connections
5. **Verify Ground**: Ensure common ground connection between all components
6. **Check Adafruit 4739 Output**: Verify Adafruit 4739 is producing stable 5V

### Arduino Not Powering On

1. **Check Adafruit 4739 Output**: Measure voltage at VOUT (should be 5.0V ±0.1V)
2. **Verify Input Voltage**: Confirm 12V is present at CN105 Pin 1 and connected to VIN
3. **Inspect Connections**: Ensure Adafruit 4739 VOUT connects to Arduino VIN
4. **Check Ground**: Verify common ground between Adafruit 4739 GND and Arduino GND
5. **Check Enable Pin**: If using enable pin, ensure it's not pulled low (default is enabled)
6. **Test with USB Power**: Try powering Arduino via USB to isolate power issues

### Adafruit 4739 Buck Converter Issues

1. **No Output Voltage**:
   - Verify 12V input voltage at VIN and GND
   - Check for short circuit on VOUT
   - Verify enable pin is not pulled low (if connected)
   - Check for damaged buck converter (replace if necessary)
   
2. **Output Voltage Out of Spec**:
   - The Adafruit 4739 has a fixed 5V output (no adjustment possible)
   - If output is not 5.0V (±0.1V), the buck converter may be damaged
   - Replace with a new Adafruit 4739 if output voltage is incorrect
   
3. **Unstable/Noisy Output**:
   - Check input voltage stability from CN105
   - Verify 12V input is within 4.5V-32V range
   - Check for loose connections
   - The Adafruit 4739 has built-in output capacitors
   
4. **Buck Converter Overheating**:
   - Check output current draw (should be <500mA for Arduino alone)
   - Ensure adequate ventilation around the module
   - The Adafruit 4739 has thermal protection and will shut down if overheated
   - Verify input voltage is not above 32V

### Intermittent Communication

1. **Check Wire Quality**: Poor quality or loose connections can cause intermittent issues
2. **EMI/RFI**: Keep wires away from high-voltage AC lines and motors
3. **Cable Length**: Keep wires as short as practical (< 6 inches/15cm if possible)
4. **Power Supply**: Ensure stable power supply voltage from Adafruit 4739
5. **Voltage Ripple**: The Adafruit 4739 has low ripple output (<50mV typical)

### Heat Pump Not Responding

1. **Verify TX/RX**: Ensure TX and RX are not swapped
2. **Check Logic Levels**: Verify the logic level converter is working correctly
3. **Protocol Timing**: Ensure the software is implementing correct protocol timing
4. **Heat Pump State**: Some heat pumps may not respond when in certain states
5. **Power Cycling**: Try power cycling both the Arduino and the heat pump

## Additional Resources

- **SwiCago/HeatPump Library**: https://github.com/SwiCago/HeatPump
- **SparkFun BOB-12009 Guide**: https://www.sparkfun.com/products/12009
- **Arduino Nano Matter Documentation**: https://docs.arduino.cc/hardware/nano-matter/
- **Mitsubishi Service Manuals**: Contact Mitsubishi for official documentation

## Revision History

- **v1.0** (2025-12-28): Initial wiring diagram documentation

---

**Remember**: Safety first! When in doubt, consult a professional HVAC technician or electrician.
