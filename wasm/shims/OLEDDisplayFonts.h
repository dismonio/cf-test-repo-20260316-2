// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_OLEDDISPLAY_FONTS_H
#define WASM_OLEDDISPLAY_FONTS_H

#include <cstdint>

// ThingPulse OLEDDisplay font format:
// Byte 0: Max width  |  Byte 1: Height  |  Byte 2: First char  |  Byte 3: Char count
// Jump table: 4 bytes per char [pos_hi, pos_lo, size, width]
// Bitmap data: column-major, ceil(height/8) bytes per column
//
// These are minimal 6x10 bitmap fonts for the WASM emulator POC.
// They're functional but simpler than the real ArialMT fonts.
// The real ThingPulse fonts can be downloaded later for pixel-perfect rendering.

// Minimal 6x10 font covering ASCII 32-126 (95 chars)
// Each char: up to 6 columns, 2 bytes per column (10 pixels high, ceil(10/8)=2 bytes)
// Max data per char: 12 bytes

extern const uint8_t ArialMT_Plain_10[];
extern const uint8_t ArialMT_Plain_16[];
extern const uint8_t ArialMT_Plain_24[];

#endif
