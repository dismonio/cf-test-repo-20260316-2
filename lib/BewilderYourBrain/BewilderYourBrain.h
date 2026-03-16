// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef BEWILDER_YOUR_BRAIN_H
#define BEWILDER_YOUR_BRAIN_H

#include "DisplayProxy.h"
#include "ButtonManager.h"
#include "MenuManager.h"

class BewilderYourBrain {
public:
    static const int SCREEN_WIDTH  = 128;
    static const int SCREEN_HEIGHT = 64;
    static const int NUM_LINES     = 4;
    static const int POINTS_PER_LINE = 4;

    BewilderYourBrain(ButtonManager& btnMgr);

    void begin();
    void update();
    void end();

private:
    ButtonManager& buttonManager;
    DisplayProxy& display;

    struct Point {
        float x, y;
        float dx, dy;
    };

    struct Line {
        Point points[POINTS_PER_LINE];
    };

    Line lines[NUM_LINES];

    unsigned long lastUpdateTime;
    static const unsigned long FRAME_INTERVAL_MS = 30;

    void initializeLines();
    void updateLines();
    void bounce(Point& p);

    static void onBackPressed(const ButtonEvent& event);
    static BewilderYourBrain* instance;
};

extern BewilderYourBrain bewilderYourBrainApp;

#endif
