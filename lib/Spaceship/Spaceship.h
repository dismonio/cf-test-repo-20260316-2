// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "DisplayProxy.h"
#include "ButtonManager.h"

class Spaceship {
public:
    Spaceship(ButtonManager& btnMgr);
    void begin();
    void update();
    void end();

private:
    ButtonManager& buttonManager;
    DisplayProxy& display;

    static Spaceship* instance;

    // Buttons
    void registerButtonCallbacks();
    void unregisterButtonCallbacks();
    static void onButtonBackPressed(const ButtonEvent& event);
    static void onButtonLeft(const ButtonEvent& event);
    static void onButtonRight(const ButtonEvent& event);
    static void onButtonUp(const ButtonEvent& event);
    static void onButtonDown(const ButtonEvent& event);

    // Overlay mode
    enum OverlayMode : uint8_t {
        Overlay_None = 0,
        Overlay_Speed = 1,
        Overlay_YawRate = 2
    };

    struct Star {
        float x;        // world x position
        float y;        // world y position (vertical spread)
        float z;        // depth
        float vx;       // lateral drift
        uint8_t kind;   // 0 = normal, 1 = shooting
        float life;
    };

    static const int kStarCount = 90;

    // State
    OverlayMode overlayMode;

    // Speed model â slider is now throttle
    float speed;
    float speedTarget;
    float cruiseSpeed;

    // Turning model â left/right buttons drive yaw
    float yaw;
    float yawRate;
    float yawRateTarget;
    bool turnLeftHeld;
    bool turnRightHeld;

    // Stars
    Star stars[kStarCount];

    // Timing
    unsigned long lastMs;
    unsigned long lastShootingMs;

    // Helpers
    void resetStar(int i, bool forceShooting, bool randomDepth);
    void updatePhysics(float dt);
    void drawScene();
    void drawWireShip(int16_t cx, int16_t cy, float yawRadians);
    void drawOverlay();

    static float clampf(float v, float lo, float hi);
};

extern Spaceship spaceshipApp;

#endif