// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef PIXEL_WATERFALL_GAME_H
#define PIXEL_WATERFALL_GAME_H

#include <vector>
#include "DisplayProxy.h"
#include "ButtonManager.h"
#include "MenuManager.h"

class PixelWaterfallGame {
public:
    struct Pixel {
        float x, y;
        float vx, vy;
    };

    PixelWaterfallGame(ButtonManager& btnMgr);

    void begin();
    void update();
    void end();

    void resetPixels();

private:
    static constexpr int SCREEN_WIDTH  = 128;
    static constexpr int SCREEN_HEIGHT = 64;
    static constexpr int NUM_PIXELS = 2500;

    ButtonManager& buttonManager;
    DisplayProxy& display;
    std::vector<Pixel> pixels;
    float inertia;
    float damping;

    static void onBackPressed(const ButtonEvent& event);
    static PixelWaterfallGame* instance;
};

extern PixelWaterfallGame pixelWaterfallGame;

#endif
