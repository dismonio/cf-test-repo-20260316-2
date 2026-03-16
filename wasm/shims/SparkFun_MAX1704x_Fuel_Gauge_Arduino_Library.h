// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_SFE_MAX1704X_H
#define WASM_SFE_MAX1704X_H

#include <cstdint>

#define MAX1704X_MAX17048 0

class SFE_MAX1704X {
public:
    SFE_MAX1704X(int = 0) {}
    bool begin() { return true; }
    void enableDebugging() {}
    uint8_t getID() { return 0x48; }
    uint8_t getVersion() { return 0x10; }
    bool isReset(bool clear = false) { (void)clear; return false; }
    float getSOC() { return 100.0f; }
    float getVoltage() { return 4.2f; }
    float getChangeRate() { return 0.0f; }
    void setThreshold(uint8_t) {}
    uint8_t getThreshold() { return 20; }
    uint8_t getVALRTMax() { return 205; }
    uint8_t getVALRTMin() { return 195; }
    void setVALRTMax(float) {}
    void setVALRTMin(float) {}
    bool enableSOCAlert() { return true; }
    bool getAlert() { return false; }
    bool isVoltageHigh() { return false; }
    bool isVoltageLow() { return false; }
    bool isLow() { return false; }
    bool isChange() { return false; }
    bool isHibernating() { return false; }
    uint16_t getHIBRTActThr() { return 0; }
    uint16_t getHIBRTHibThr() { return 0; }
    void quickStart() {}
};

#endif
