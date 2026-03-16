// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef STRATAGEM_GAME_SPRITES_H
#define STRATAGEM_GAME_SPRITES_H

#include <Arduino.h>

// -------------------------
// UP Arrow (Outlined)
const unsigned char Arrow_Up_8x8[] PROGMEM = {
  0x10, //    █
  0x38, //   █ █
  0x7C, //  █ █ █
  0x10, //    █
  0x10, //    █
  0x10, //    █
  0x10, //    █
  0x00  //
};

// -------------------------
// UP Arrow (Filled)
const unsigned char Arrow_Up_Fill_8x8[] PROGMEM = {
  0x10,
  0x38,
  0x7C,
  0x7C,
  0x7C,
  0x7C,
  0x7C,
  0x00
};

// -------------------------
// DOWN Arrow (Outlined)
const unsigned char Arrow_Down_8x8[] PROGMEM = {
  0x10, //    █
  0x10, //    █
  0x10, //    █
  0x10, //    █
  0x7C, //  █ █ █
  0x38, //   █ █
  0x10, //    █
  0x00
};

// -------------------------
// DOWN Arrow (Filled)
const unsigned char Arrow_Down_Fill_8x8[] PROGMEM = {
  0x7C,
  0x7C,
  0x7C,
  0x7C,
  0x7C,
  0x38,
  0x10,
  0x00
};

// -------------------------
// RIGHT Arrow (Outlined)
const unsigned char Arrow_Right_8x8[] PROGMEM = {
  0x10,
  0x30,
  0x70,
  0xFF,
  0x70,
  0x30,
  0x10,
  0x00
};

// -------------------------
// RIGHT Arrow (Filled)
const unsigned char Arrow_Right_Fill_8x8[] PROGMEM = {
  0xF0,
  0xF8,
  0xFC,
  0xFF,
  0xFC,
  0xF8,
  0xF0,
  0x00
};

// -------------------------
// LEFT Arrow (Outlined)
const unsigned char Arrow_Left_8x8[] PROGMEM = {
  0x08,
  0x0C,
  0x0E,
  0xFF,
  0x0E,
  0x0C,
  0x08,
  0x00
};

// -------------------------
// LEFT Arrow (Filled)
const unsigned char Arrow_Left_Fill_8x8[] PROGMEM = {
  0x1E,
  0x3E,
  0x7E,
  0xFF,
  0x7E,
  0x3E,
  0x1E,
  0x00
};

#endif
