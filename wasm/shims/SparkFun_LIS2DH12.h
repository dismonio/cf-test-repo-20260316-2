// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_SPARKFUN_LIS2DH12_H
#define WASM_SPARKFUN_LIS2DH12_H

#include <cstdint>

class SPARKFUN_LIS2DH12 {
public:
    bool begin() { return true; }
    float getX() { return 0.0f; }
    float getY() { return 0.0f; }
    float getZ() { return 1.0f; }
    float getTemperature() { return 25.0f; }
    void setDataRate(uint8_t) {}
    void setScale(uint8_t) {}
};

#endif
