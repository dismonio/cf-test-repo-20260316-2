// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_WIFIMANAGER_H
#define WASM_WIFIMANAGER_H

#include "Arduino.h"

class WiFiManager {
public:
    WiFiManager() {}
    bool autoConnect(const char* = nullptr, const char* = nullptr) { return false; }
    void resetSettings() {}
    void setConfigPortalTimeout(unsigned long) {}
    bool startConfigPortal(const char* = nullptr, const char* = nullptr) { return false; }
    void stopConfigPortal() {}
    void process() {}
};

#endif
