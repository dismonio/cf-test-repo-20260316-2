// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef GRAVEYARDSCREENSAVER_H
#define GRAVEYARDSCREENSAVER_H
#include "ButtonManager.h"
#include "MenuManager.h"
#include "DisplayProxy.h"
#include "HAL.h"
#include <Arduino.h>

extern const unsigned char graveyardScreensaverApp_frame_0_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_1_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_2_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_3_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_4_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_5_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_6_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_7_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_8_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_9_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_10_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_11_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_12_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_13_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_14_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_15_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_16_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_17_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_18_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_19_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_20_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_21_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_22_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_23_bits[] PROGMEM;
extern const unsigned char graveyardScreensaverApp_frame_24_bits[] PROGMEM;

class GraveyardScreensaverApp {
public:
    GraveyardScreensaverApp(ButtonManager&);
    void begin();
    void end();
    void update();
    static void onButtonBackPressed(const ButtonEvent&);
    static GraveyardScreensaverApp* instance;
private:
    ButtonManager& buttonManager;
    DisplayProxy& display;
    unsigned long lastFrameTime;
    int currentFrame;
};
extern GraveyardScreensaverApp graveyardScreensaverApp;
#endif