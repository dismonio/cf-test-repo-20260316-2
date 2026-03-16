// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_ADAFRUIT_NEOPIXEL_H
#define WASM_ADAFRUIT_NEOPIXEL_H

#include <cstdint>

#define NEO_GRBW 0x06
#define NEO_GRB  0x01
#define NEO_KHZ800 0x0100

class Adafruit_NeoPixel {
public:
    Adafruit_NeoPixel() : _numPixels(0) {}
    Adafruit_NeoPixel(uint16_t n, int16_t, uint8_t = NEO_GRB + NEO_KHZ800)
        : _numPixels(n) {
        for (int i = 0; i < 8; i++) {
            _r[i] = _g[i] = _b[i] = _w[i] = 0;
        }
    }

    void begin() {}

    void show() {
        _needsShow = true;
    }

    void setPixelColor(uint16_t n, uint32_t c) {
        if (n >= _numPixels || n >= 8) return;
        _w[n] = (c >> 24) & 0xFF;
        _r[n] = (c >> 16) & 0xFF;
        _g[n] = (c >>  8) & 0xFF;
        _b[n] =  c        & 0xFF;
    }

    static uint32_t Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) {
        return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }

    uint32_t ColorHSV(uint16_t hue, uint8_t sat = 255, uint8_t val = 255) {
        // Simplified HSV-to-RGB for WASM shim (matches Adafruit_NeoPixel algorithm)
        uint32_t r, g, b;

        // Hue is 0-65535, map to 0-1535 (6 sectors of 256)
        hue = (hue * 1536L + 32768) / 65536;
        uint8_t lo = hue & 0xFF;
        uint8_t hi = 255 - lo;

        switch ((hue >> 8) % 6) {
            case 0: r = 255; g = lo;  b = 0;   break;
            case 1: r = hi;  g = 255; b = 0;   break;
            case 2: r = 0;   g = 255; b = lo;  break;
            case 3: r = 0;   g = hi;  b = 255; break;
            case 4: r = lo;  g = 0;   b = 255; break;
            default: r = 255; g = 0;  b = hi;  break;
        }

        // Apply saturation
        uint32_t s1 = 1 + sat;
        r = ((r * s1) >> 8) + 255 - sat;
        g = ((g * s1) >> 8) + 255 - sat;
        b = ((b * s1) >> 8) + 255 - sat;

        // Apply value (brightness)
        uint32_t v1 = 1 + val;
        r = (r * v1) >> 8;
        g = (g * v1) >> 8;
        b = (b * v1) >> 8;

        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }

    static uint32_t gamma32(uint32_t c) { return c; }

    uint16_t numPixels() const { return _numPixels; }

    uint32_t getPixelColor(uint16_t n) const {
        if (n >= _numPixels || n >= 8) return 0;
        return Color(_r[n], _g[n], _b[n], _w[n]);
    }

    void getLedRGBW(uint16_t n, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &w) const {
        if (n >= _numPixels || n >= 8) { r=g=b=w=0; return; }
        r = _r[n]; g = _g[n]; b = _b[n]; w = _w[n];
    }

    bool needsShow() const { return _needsShow; }
    void clearNeedsShow() { _needsShow = false; }

private:
    uint16_t _numPixels;
    uint8_t _r[8], _g[8], _b[8], _w[8];
    bool _needsShow = false;
};

#endif
