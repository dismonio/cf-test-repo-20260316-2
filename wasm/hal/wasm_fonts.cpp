// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

// Real ThingPulse font data (MIT licensed) for pixel-perfect rendering.
// The _data arrays are static in the included header; our extern pointers
// (declared in SSD1306Wire.h) simply alias them.

#include <cstdint>
#include "Arduino.h"
#include "OLEDDisplayFonts_real.h"

const uint8_t* ArialMT_Plain_10 = ArialMT_Plain_10_data;
const uint8_t* ArialMT_Plain_16 = ArialMT_Plain_16_data;
const uint8_t* ArialMT_Plain_24 = ArialMT_Plain_24_data;
