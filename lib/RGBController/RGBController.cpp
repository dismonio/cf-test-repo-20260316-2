// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "RGBController.h"
#include "globals.h"
#include "HAL.h"

typedef void (*RGBW)(void);
RGBW colors[] = {red, green, blue, white, halfWHITE};
int colorLength = (sizeof(colors) / sizeof(RGBW));

// Reference alias for `HAL::strip()`
static Adafruit_NeoPixel& strip = HAL::strip();

// --- Update Throttle ---
static bool     s_dirty       = false;
static uint32_t s_lastShowMs  = 0;
static uint16_t s_minInterval = 33; // ~30 FPS cap

// Cache of last "set all" color to avoid redundant updates
static uint8_t s_lastR = 0, s_lastG = 0, s_lastB = 0, s_lastW = 0;
static bool    s_lastAllValid = false;

void markDirty() { s_dirty = true; }
void invalidateColorCache() { s_lastAllValid = false; }

static void colorSet(uint32_t c, uint8_t /*wait*/) {
  // This is a blanket setter; it invalidates last-all cache.
  s_lastAllValid = false;
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  markDirty();
}

void initRGB() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  s_lastR = s_lastG = s_lastB = s_lastW = 0;
  s_lastAllValid = true;
  s_lastShowMs = millis();
  s_dirty = false;
}

// Throttled show: only when dirty AND interval elapsed
void updateStrip() {
  uint32_t now = millis();
  if (!s_dirty) return;
  if ((now - s_lastShowMs) < s_minInterval) return;

  strip.show();
  s_lastShowMs = now;
  s_dirty = false;
}

void red()         { colorSet(strip.Color(0, 25, 0, 0), 0); }
void green()       { colorSet(strip.Color(25, 0, 0, 0), 0); }
void blue()        { colorSet(strip.Color(0, 0, 25, 0), 0); }
void white()       { colorSet(strip.Color(0, 0, 0, 50), 0); }
void halfWHITE()   { colorSet(strip.Color(0, 0, 0, 25), 0); }

void setRandomColors() {
  s_lastAllValid = false;
  uint8_t maxBrightness = 10;
  for (int i = 1; i < (int)strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(random(0, maxBrightness),
                                       random(0, maxBrightness),
                                       random(0, maxBrightness), 0));
  }
  markDirty();
}

void setDeterminedColorsFront(uint8_t colorR, uint8_t colorG, uint8_t colorB, uint8_t colorW) {
  s_lastAllValid = false;
  for (int i = 1; i < (int)strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(colorR, colorG, colorB, colorW));
  }
  markDirty();
}

void setDeterminedColorsAll(uint8_t colorR, uint8_t colorG, uint8_t colorB, uint8_t colorW) {
  // Skip if identical to last "all" request
  if (s_lastAllValid &&
      colorR == s_lastR && colorG == s_lastG &&
      colorB == s_lastB && colorW == s_lastW) {
    return;
  }

  for (int i = 0; i < (int)strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(colorR, colorG, colorB, colorW));
  }

  s_lastR = colorR; s_lastG = colorG; s_lastB = colorB; s_lastW = colorW;
  s_lastAllValid = true;
  markDirty();
}

void setColorsOff() {
  if (s_lastAllValid && s_lastR == 0 && s_lastG == 0 && s_lastB == 0 && s_lastW == 0) {
    return;
  }
  for (int i = 0; i < (int)strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  s_lastR = s_lastG = s_lastB = s_lastW = 0;
  s_lastAllValid = true;
  markDirty();
}

void rainbow(int /*wait*/) {
  s_lastAllValid = false;
  static long firstPixelHue = 0;
  firstPixelHue += 256;
  if (firstPixelHue >= 5 * 65536) firstPixelHue = 0;

  for (int i = 1; i < (int)strip.numPixels(); i++) {
    int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
  }
  markDirty();
}

void mapToRainbow(int input, uint8_t dim, uint8_t &red, uint8_t &green, uint8_t &blue) {
    input = input < 0 ? 0 : (input > 4095 ? 4095 : input);
    dim = dim > 255 ? 255 : dim;

    float normalized = (float)input / 4095.0;

    float x, y, z;
    if (normalized < 0.2) {
        x = 1.0; y = normalized / 0.2; z = 0.0;
    } else if (normalized < 0.4) {
        x = 1.0; y = 1.0; z = 0.0;
    } else if (normalized < 0.6) {
        float t = (normalized - 0.4) / 0.2;
        x = 1.0 - t; y = 1.0; z = 0.0;
    } else if (normalized < 0.8) {
        float t = (normalized - 0.6) / 0.2;
        x = 0.0; y = 1.0 - t; z = t;
    } else {
        float t = (normalized - 0.8) / 0.2;
        x = t; y = 0.0; z = 1.0;
    }

    red   = (uint8_t)(y * dim);
    green = (uint8_t)(x * dim);
    blue  = (uint8_t)(z * dim);
}
