// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef EYE_H
#define EYE_H
#include "ButtonManager.h"
#include "MenuManager.h"
#include "DisplayProxy.h"
#include "HAL.h"
#include <Arduino.h>

extern const unsigned char eyeApp_frame_0_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_1_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_2_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_3_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_4_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_5_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_6_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_7_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_8_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_9_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_10_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_11_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_12_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_13_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_14_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_15_bits[] PROGMEM;
extern const unsigned char eyeApp_frame_16_bits[] PROGMEM;

class EyeApp {
public:
    EyeApp(ButtonManager&);
    void begin();
    void end();
    void update();
    static void onButtonBackPressed(const ButtonEvent&);
    static EyeApp* instance;
private:
    ButtonManager& buttonManager;
    DisplayProxy& display;
    unsigned long lastFrameTime;
    int currentFrame;
};
extern EyeApp eyeApp;
#endif