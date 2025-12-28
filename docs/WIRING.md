# Wiring Diagram - Matter Controller for Mitsubishi CN105

## Overview

This document provides detailed wiring instructions for connecting the Arduino Nano Matter (3.3V logic) to the Mitsubishi heat pump CN105 connector (5V logic) using a SparkFun Logic Level Converter (BOB-12009).

**⚠️ WARNING: Read all safety warnings before proceeding with installation.**

## Components Required

1. **Arduino Nano Matter** - 3.3V logic microcontroller with Matter support
2. **SparkFun Logic Level Converter BOB-12009** - Bidirectional logic level converter
3. **Mitsubishi CN105 Connector** - 5-pin JST PA connector (or equivalent)
4. **Jumper Wires** - For connections
5. **Power Supply** (optional) - If not using CN105 5V power

## Mitsubishi CN105 Pinout

The CN105 connector on Mitsubishi heat pumps (MSZ-GS06NA and compatible models) uses a 5-pin JST PA connector:

```
CN105 Connector Pinout (looking at the heat pump PCB connector):
┌─────────────────┐
│ 1  2  3  4  5   │
└─────────────────┘

Pin 1: 12V    (DC power - NOT USED in this project)
Pin 2: GND    (Ground - Common reference)
Pin 3: 5V     (5V DC power output from heat pump)
Pin 4: TX     (Heat pump transmit, 5V logic)
Pin 5: RX     (Heat pump receive, 5V logic)
```

**Important Notes:**
- Pin 1 (12V) should NOT be connected - we don't use this voltage
- Pin 3 provides 5V power (can power the logic level converter)
- Pins 4 and 5 use 5V TTL serial communication at 2400 baud, 8E1 (8 data bits, even parity, 1 stop bit)

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
Mitsubishi Heat Pump CN105          SparkFun BOB-12009             Arduino Nano Matter
┌─────────────────────┐              Logic Level Converter          ┌──────────────────┐
│                     │                                              │                  │
│  Pin 1: 12V  ────────────── NOT CONNECTED                         │                  │
│                     │                                              │                  │
│  Pin 2: GND  ────────────────────────┬─── GND (HV side)           │                  │
│                     │                 │                            │                  │
│  Pin 3: 5V   ────────────────────────┼─── HV (5V reference)       │                  │
│                     │                 │                            │                  │
│  Pin 4: TX   ────────────────────────┼─── HV1                     │                  │
│  (5V logic)         │                 │                            │                  │
│                     │                 │     LV1 ───────────────────┼─── RX (D0)      │
│  Pin 5: RX   ────────────────────────┼─── HV2                     │    (3.3V logic) │
│  (5V logic)         │                 │                            │                  │
│                     │                 │     LV2 ───────────────────┼─── TX (D1)      │
└─────────────────────┘                 │                            │    (3.3V logic) │
                                        │                            │                  │
                                        │     LV (3.3V reference) ───┼─── 3.3V         │
                                        │                            │                  │
                                        └─── GND (LV side) ──────────┼─── GND          │
                                                                     │                  │
                                                                     └──────────────────┘

Common Ground: All GND connections must be tied together
```

## Detailed Connection Table

| From                  | To                         | Wire Color Suggestion | Notes                    |
|-----------------------|----------------------------|-----------------------|--------------------------|
| CN105 Pin 2 (GND)     | BOB-12009 GND (HV side)    | Black                 | Common ground            |
| CN105 Pin 3 (5V)      | BOB-12009 HV               | Red                   | 5V reference for HV side |
| CN105 Pin 4 (TX)      | BOB-12009 HV1              | Yellow                | Heat pump transmits      |
| CN105 Pin 5 (RX)      | BOB-12009 HV2              | Green                 | Heat pump receives       |
| BOB-12009 LV1         | Arduino RX (D0)            | Orange                | Arduino receives         |
| BOB-12009 LV2         | Arduino TX (D1)            | Blue                  | Arduino transmits        |
| BOB-12009 LV          | Arduino 3.3V               | Red                   | 3.3V reference for LV side|
| BOB-12009 GND (LV side)| Arduino GND               | Black                 | Common ground            |

## Power Supply Recommendations

### Option 1: Power from CN105 (Recommended for testing)

The CN105 connector provides 5V power (Pin 3) which can power the logic level converter. However, **DO NOT** use this to power the Arduino Nano Matter directly:

- Use CN105 5V only for the BOB-12009 HV reference
- Power the Arduino Nano Matter via USB or a separate 3.3V regulator
- This prevents potential power supply issues and provides isolation

### Option 2: External Power Supply (Recommended for production)

For permanent installations:

1. **Arduino Nano Matter**: Powered via USB or dedicated 5V supply with onboard regulator
2. **Logic Level Converter**: Powered from CN105 5V (Pin 3) for HV side, Arduino 3.3V for LV side
3. **Ground**: All grounds must be connected together (common reference)

**Important**: Ensure adequate current capacity if using CN105 5V for additional components. The CN105 5V supply is intended for low-power accessories.

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

2. **ESD PROTECTION**: Use proper ESD protection when handling electronic components. Discharge static electricity before touching the heat pump PCB.

3. **PROPER INSTALLATION**: 
   - Ensure all connections are secure and properly insulated
   - Do not leave exposed conductors that could short circuit
   - Use appropriate gauge wire for all connections
   - Double-check polarity before applying power

4. **VOLTAGE LEVELS**:
   - Never connect 5V signals directly to Arduino Nano Matter (3.3V) pins
   - Always use the logic level converter for voltage translation
   - Verify voltage levels with a multimeter before making connections

5. **POWER SUPPLY**:
   - Do not draw excessive current from CN105 5V supply
   - Use a separate power supply for Arduino if needed
   - Ensure proper voltage regulation

### ⚠️ WARRANTY AND LIABILITY

1. **WARRANTY VOID**: Opening the heat pump and connecting to internal connectors will likely **VOID THE WARRANTY**. Proceed at your own risk.

2. **PROFESSIONAL INSTALLATION**: If you are not comfortable with electrical work, hire a qualified technician.

3. **COMPLIANCE**: Ensure your installation complies with local electrical codes and regulations.

4. **DAMAGE RISK**: Incorrect wiring can damage the heat pump controller, Arduino, or both. Verify all connections before applying power.

5. **NO LIABILITY**: The authors and contributors of this project accept no liability for any damage, injury, or loss resulting from following these instructions.

### ⚠️ TESTING RECOMMENDATIONS

1. **Bench Test First**: 
   - Test all connections on a workbench before installing in the heat pump
   - Use a multimeter to verify connections and voltage levels
   - Test serial communication before connecting to the actual heat pump

2. **Incremental Testing**:
   - First, test the Arduino Nano Matter alone
   - Then add the logic level converter and verify signal integrity
   - Finally, connect to the heat pump and test communication

3. **Monitor Temperature**: Ensure no components overheat during operation

4. **Documentation**: Document your specific installation with photos for future reference

## Troubleshooting

### No Communication with Heat Pump

1. **Check Connections**: Verify all wiring matches the diagram above
2. **Verify Voltage Levels**: Use a multimeter to check 5V and 3.3V supplies
3. **Check Serial Parameters**: Ensure UART is configured for 2400 baud, 8E1
4. **Test Continuity**: Check for broken wires or poor connections
5. **Verify Ground**: Ensure common ground connection between all components

### Intermittent Communication

1. **Check Wire Quality**: Poor quality or loose connections can cause intermittent issues
2. **EMI/RFI**: Keep wires away from high-voltage AC lines and motors
3. **Cable Length**: Keep wires as short as practical (< 6 inches/15cm if possible)
4. **Power Supply**: Ensure stable power supply voltage

### Heat Pump Not Responding

1. **Verify TX/RX**: Ensure TX and RX are not swapped
2. **Check Logic Levels**: Verify the logic level converter is working correctly
3. **Protocol Timing**: Ensure the software is implementing correct protocol timing
4. **Heat Pump State**: Some heat pumps may not respond when in certain states

## Additional Resources

- **SwiCago/HeatPump Library**: https://github.com/SwiCago/HeatPump
- **SparkFun BOB-12009 Guide**: https://www.sparkfun.com/products/12009
- **Arduino Nano Matter Documentation**: https://docs.arduino.cc/hardware/nano-matter/
- **Mitsubishi Service Manuals**: Contact Mitsubishi for official documentation

## Revision History

- **v1.0** (2025-12-28): Initial wiring diagram documentation

---

**Remember**: Safety first! When in doubt, consult a professional HVAC technician or electrician.
