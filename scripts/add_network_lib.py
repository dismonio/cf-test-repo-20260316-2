# SPDX-License-Identifier: MIT
# Copyright (c) 2023-2026 Dismo Industries LLC

"""
PlatformIO pre-build script: Network Library

The pioarduino ESP32 Arduino 3.x core split the WiFi library into WiFi + Network.
PlatformIO's LDF discovers WiFi (via ESPAsyncWebServer) but fails to follow
WiFi's internal includes to discover the Network library. This script explicitly
compiles the Network library and links it.

Fixes:
- undefined reference to NetworkInterface::* (used by WiFi AP/STA classes)
- undefined reference to NetworkEvents::*
- undefined reference to lwip_hook_ip6_input (defined in NetworkInterface.cpp)
"""
import os
from os.path import join

Import("env")

framework_dir = env.PioPlatform().get_package_dir("framework-arduinoespressif32")
network_src = join(framework_dir, "libraries", "Network", "src")

if os.path.isdir(network_src):
    # Ensure include path is available (should already be via build_flags, but be safe)
    env.Append(CPPPATH=[network_src])

    # Compile Network library sources and link them
    lib = env.BuildLibrary(
        join("$BUILD_DIR", "FrameworkNetwork"),
        network_src
    )
    env.Prepend(LIBS=[lib])
    print(f"[Network Lib] Added framework Network library from {network_src}")
else:
    print(f"[Network Lib] WARNING: Network library not found at {network_src}")
