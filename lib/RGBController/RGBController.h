// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef RGBCONTROLLER_H
#define RGBCONTROLLER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// RGB LED control functions — uses HAL::strip() for NeoPixel access.
// updateStrip() is throttled to ~30fps via dirty flag.
void initRGB();
void updateStrip();

void red();
void green();
void blue();
void white();
void halfWHITE();
void setRandomColors();
void setDeterminedColorsFront(uint8_t colorR, uint8_t colorG, uint8_t colorB, uint8_t colorW);
void setDeterminedColorsAll(uint8_t colorR, uint8_t colorG, uint8_t colorB, uint8_t colorW);
void setColorsOff();
void markDirty();
void invalidateColorCache();
void rainbow(int wait);
void mapToRainbow(int input, uint8_t dim, uint8_t &red, uint8_t &green, uint8_t &blue);

#endif
