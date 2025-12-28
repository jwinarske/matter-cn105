#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Flash script for Arduino Nano Matter
# Helper script to flash the Matter CN105 controller firmware

set -e

BOARD="arduino_nano_matter"
BUILD_DIR="build"

echo "======================================"
echo "Matter CN105 Controller Flash Script"
echo "======================================"
echo ""

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory '$BUILD_DIR' not found"
    echo "Please build the project first with:"
    echo "  west build -b $BOARD"
    exit 1
fi

# Check if zephyr.hex exists
if [ ! -f "$BUILD_DIR/zephyr/zephyr.hex" ]; then
    echo "Error: Firmware file not found: $BUILD_DIR/zephyr/zephyr.hex"
    echo "Please build the project first"
    exit 1
fi

echo "Flashing firmware to $BOARD..."
echo ""

# Flash using west
west flash

echo ""
echo "======================================"
echo "Flashing complete!"
echo "======================================"
echo ""
echo "To monitor serial output:"
echo "  west espressif monitor"
echo "  or"
echo "  screen /dev/ttyUSB0 115200"
