# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Find Zephyr. This also loads Zephyr's build system.
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(matter_cn105_controller VERSION 0.1.0)

# Add application source files
target_sources(app PRIVATE
    src/main.c
    src/heatpump_driver.cpp
    src/matter_integration.cpp
    src/state_sync.cpp
    src/attribute_handlers.cpp
)

# Add include directories
target_include_directories(app PRIVATE
    include
    src
)

# TODO: Add Matter SDK library when integrating
# TODO: Add any additional dependencies
