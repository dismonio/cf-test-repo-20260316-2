# SPDX-License-Identifier: MIT
# Copyright (c) 2023-2026 Dismo Industries LLC

@echo off
REM Change directory to .pio/build/adafruit_feather_esp32_v2
cd /d "%~dp0.pio\build\adafruit_feather_esp32_v2"

REM Run esptool with the given parameters
esptool --chip esp32 merge_bin -o merged_firmware.bin --flash_mode dio --flash_freq 80m --flash_size 8MB 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin

pause