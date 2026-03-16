// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#pragma once

#include <Arduino.h>
#include <esp_gap_bt_api.h>
#include <esp_bt_device.h>

struct BTScanResult {
    char name[64];
    esp_bd_addr_t address;
    int rssi;
};

class BTScanner {
public:
    bool begin();
    bool startScan(int durationSec = 10);
    void stopScan();
    void end();

    bool isScanning() const;
    int getResultCount() const;
    BTScanResult getResult(int index);

private:
    static BTScanner* _instance;
    static void gapCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param);

    static const int MAX_RESULTS = 16;
    BTScanResult _results[MAX_RESULTS];
    volatile int _resultCount = 0;
    volatile bool _scanning = false;
    bool _btInitialized = false;
    bool _ownsStack = false;  // true if we initialized bluedroid (vs reusing A2DPStream's)
    mutable portMUX_TYPE _mutex = portMUX_INITIALIZER_UNLOCKED;

    bool addResult(const char* name, esp_bd_addr_t addr, int rssi);
};
