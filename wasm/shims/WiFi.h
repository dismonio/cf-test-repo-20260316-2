// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_WIFI_H
#define WASM_WIFI_H

#include <cstdint>
#include "Arduino.h"

typedef enum {
    WL_NO_SHIELD = 255,
    WL_IDLE_STATUS = 0,
    WL_NO_SSID_AVAIL = 1,
    WL_SCAN_COMPLETED = 2,
    WL_CONNECTED = 3,
    WL_CONNECT_FAILED = 4,
    WL_CONNECTION_LOST = 5,
    WL_DISCONNECTED = 6
} wl_status_t;

class WiFiClass {
public:
    wl_status_t status() { return WL_DISCONNECTED; }
    void mode(int) {}
    void disconnect(bool = false) {}
    String macAddress() { return String("00:00:00:00:00:00"); }
    int32_t RSSI() { return -100; }
};

extern WiFiClass WiFi;

#endif
