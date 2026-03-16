// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "PixelWaterfallGame.h"
#include "HAL.h"

PixelWaterfallGame* PixelWaterfallGame::instance = nullptr;

PixelWaterfallGame pixelWaterfallGame(HAL::buttonManager());

PixelWaterfallGame::PixelWaterfallGame(ButtonManager& btnMgr)
    : buttonManager(btnMgr),
      display(HAL::displayProxy()),
      inertia(0.01f),
      damping(0.98f)
{
    instance = this;
}

void PixelWaterfallGame::begin() {
    resetPixels();
    buttonManager.registerCallback(button_SelectIndex, onBackPressed);
}

void PixelWaterfallGame::end() {
    buttonManager.unregisterCallback(button_SelectIndex);
    pixels.clear();
    pixels.shrink_to_fit();
}

void PixelWaterfallGame::update() {
    float normalizedAx = accelX / 1030.0f;
    float normalizedAy = accelY / 1030.0f;

    for (auto& pixel : pixels) {
        pixel.vx += normalizedAx * inertia;
        pixel.vy += normalizedAy * inertia;
        pixel.vx *= damping;
        pixel.vy *= damping;
        pixel.x += pixel.vx;
        pixel.y += pixel.vy;

        if (pixel.x < 0) { pixel.x = 0; pixel.vx *= -0.5f; }
        else if (pixel.x >= SCREEN_WIDTH) { pixel.x = SCREEN_WIDTH - 1; pixel.vx *= -0.5f; }
        if (pixel.y < 0) { pixel.y = 0; pixel.vy *= -0.5f; }
        else if (pixel.y >= SCREEN_HEIGHT) { pixel.y = SCREEN_HEIGHT - 1; pixel.vy *= -0.5f; }
    }

    display.clear();
    for (const auto& pixel : pixels) {
        int px = static_cast<int>(pixel.x);
        int py = static_cast<int>(pixel.y);
        if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
            display.setPixel(px, py);
        }
    }
    display.display();
}

void PixelWaterfallGame::resetPixels() {
    pixels.clear();
    pixels.reserve(NUM_PIXELS);
    for (int i = 0; i < NUM_PIXELS; ++i) {
        Pixel p;
        p.x = static_cast<float>(random(SCREEN_WIDTH));
        p.y = static_cast<float>(random(SCREEN_HEIGHT));
        p.vx = 0.0f;
        p.vy = 0.0f;
        pixels.push_back(p);
    }
}

void PixelWaterfallGame::onBackPressed(const ButtonEvent& event) {
    if (instance) {
        MenuManager::instance().returnToMenu();
    }
}
