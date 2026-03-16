// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_PREFERENCES_H
#define WASM_PREFERENCES_H

#include <cstdint>

class Preferences {
public:
    bool begin(const char*, bool readOnly = false) { (void)readOnly; return true; }
    void end() {}
    int getInt(const char*, int defaultValue = 0) { return defaultValue; }
    void putInt(const char*, int) {}
    bool getBool(const char*, bool defaultValue = false) { return defaultValue; }
    void putBool(const char*, bool) {}
};

#endif
