// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "BewilderYourBrain.h"
#include "HAL.h"

BewilderYourBrain* BewilderYourBrain::instance = nullptr;

BewilderYourBrain bewilderYourBrainApp(HAL::buttonManager());

BewilderYourBrain::BewilderYourBrain(ButtonManager& btnMgr)
    : buttonManager(btnMgr),
      display(HAL::displayProxy()),
      lastUpdateTime(0)
{
    instance = this;
}

void BewilderYourBrain::begin() {
    initializeLines();
    lastUpdateTime = millis();
    buttonManager.registerCallback(button_SelectIndex, onBackPressed);
}

void BewilderYourBrain::end() {
    buttonManager.unregisterCallback(button_SelectIndex);
}

void BewilderYourBrain::update() {
    unsigned long now = millis();
    if (now - lastUpdateTime >= FRAME_INTERVAL_MS) {
        updateLines();
        lastUpdateTime = now;
    }

    display.clear();
    for (int i = 0; i < NUM_LINES; i++) {
        const Line& line = lines[i];
        for (int j = 0; j < POINTS_PER_LINE - 1; j++) {
            display.drawLine(
                static_cast<int>(line.points[j].x),
                static_cast<int>(line.points[j].y),
                static_cast<int>(line.points[j + 1].x),
                static_cast<int>(line.points[j + 1].y)
            );
        }
    }
    display.display();
}

void BewilderYourBrain::initializeLines() {
    for (int i = 0; i < NUM_LINES; i++) {
        Line& line = lines[i];
        for (int j = 0; j < POINTS_PER_LINE; j++) {
            Point& p = line.points[j];
            p.x = random(0, SCREEN_WIDTH);
            p.y = random(0, SCREEN_HEIGHT);
            p.dx = ((float)random(-100, 101)) / 100.0f;
            p.dy = ((float)random(-100, 101)) / 100.0f;
        }
    }
}

void BewilderYourBrain::updateLines() {
    for (int i = 0; i < NUM_LINES; i++) {
        Line& line = lines[i];
        for (int j = 0; j < POINTS_PER_LINE; j++) {
            Point& p = line.points[j];
            p.x += p.dx;
            p.y += p.dy;
            bounce(p);
        }
    }
}

void BewilderYourBrain::bounce(Point& p) {
    if (p.x <= 0 || p.x >= SCREEN_WIDTH - 1) {
        p.dx = -p.dx;
        p.x = constrain(p.x, 0.0f, (float)(SCREEN_WIDTH - 1));
    }
    if (p.y <= 0 || p.y >= SCREEN_HEIGHT - 1) {
        p.dy = -p.dy;
        p.y = constrain(p.y, 0.0f, (float)(SCREEN_HEIGHT - 1));
    }
}

void BewilderYourBrain::onBackPressed(const ButtonEvent& event) {
    if (instance) {
        MenuManager::instance().returnToMenu();
    }
}
