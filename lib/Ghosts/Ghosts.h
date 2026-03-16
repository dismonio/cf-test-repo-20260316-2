// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef GHOSTS_H
#define GHOSTS_H
#include "ButtonManager.h"
#include "MenuManager.h"
#include "DisplayProxy.h"
#include "HAL.h"
#include <Arduino.h>

extern const unsigned char ghostsApp_frame_0_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_1_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_2_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_3_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_4_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_5_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_6_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_7_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_8_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_9_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_10_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_11_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_12_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_13_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_14_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_15_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_16_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_17_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_18_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_19_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_20_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_21_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_22_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_23_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_24_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_25_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_26_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_27_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_28_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_29_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_30_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_31_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_32_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_33_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_34_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_35_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_36_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_37_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_38_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_39_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_40_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_41_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_42_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_43_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_44_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_45_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_46_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_47_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_48_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_49_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_50_bits[] PROGMEM;
extern const unsigned char ghostsApp_frame_51_bits[] PROGMEM;

class GhostsApp {
public:
    GhostsApp(ButtonManager&);
    void begin();
    void end();
    void update();
    static void onButtonBackPressed(const ButtonEvent&);
    static GhostsApp* instance;
private:
    ButtonManager& buttonManager;
    DisplayProxy& display;
    unsigned long lastFrameTime;
    int currentFrame;
};
extern GhostsApp ghostsApp;
#endif