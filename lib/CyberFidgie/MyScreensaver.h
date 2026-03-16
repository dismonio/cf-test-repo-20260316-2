// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef MYSCREENSAVER_H
#define MYSCREENSAVER_H
#include "ButtonManager.h"
#include "MenuManager.h"
#include "DisplayProxy.h"
#include "HAL.h"
#include <Arduino.h>

extern const unsigned char myScreensaverApp_frame_0_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_1_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_2_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_3_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_4_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_5_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_6_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_7_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_8_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_9_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_10_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_11_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_12_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_13_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_14_bits[] PROGMEM;
extern const unsigned char myScreensaverApp_frame_15_bits[] PROGMEM;

class MyScreensaverApp {
public:
    MyScreensaverApp(ButtonManager&);
    void begin();
    void end();
    void update();
    static void onButtonBackPressed(const ButtonEvent&);
    static MyScreensaverApp* instance;
private:
    ButtonManager& buttonManager;
    DisplayProxy& display;
    unsigned long lastFrameTime;
    int currentFrame;
};
extern MyScreensaverApp myScreensaverApp;
#endif