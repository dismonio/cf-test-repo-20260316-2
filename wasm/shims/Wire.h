// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_WIRE_H
#define WASM_WIRE_H

#include <cstdint>

class TwoWire {
public:
    void begin(int sda = -1, int scl = -1) { (void)sda; (void)scl; }
    void beginTransmission(uint8_t) {}
    uint8_t endTransmission(bool stop = true) { (void)stop; return 0; }
    uint8_t requestFrom(uint8_t, uint8_t) { return 0; }
    int available() { return 0; }
    int read() { return 0; }
    void write(uint8_t) {}
};

extern TwoWire Wire;

#endif
