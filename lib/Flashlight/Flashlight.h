// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef FLASHLIGHT_H
#define FLASHLIGHT_H

#include "DisplayProxy.h"
#include "ButtonManager.h"

extern bool flashlightStatus;

class Flashlight {
public:
    Flashlight(ButtonManager& btnMgr);

    void begin();
    void update();
    void end();

private:
    ButtonManager& buttonManager;
    DisplayProxy& display;

    // For handling button presses
    static Flashlight* instance; // To allow static callbacks

    void registerButtonCallbacks();
    void unregisterButtonCallbacks();

    static void onButtonBackPressed(const ButtonEvent& event);
};

// Declare that there's a global object called below somewhere for AppDefs to use
// must be after the reference class definition exists
extern Flashlight flashlight;

// Declare global functions
void flashlightBegin();
void flashlightEnd();
void flashlightUpdate();

#endif  // FLASHLIGHT_H
