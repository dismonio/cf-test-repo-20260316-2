// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_APP_MANAGER_H
#define WASM_APP_MANAGER_H

enum AppIndex {
    APP_MENU = 0,
    APP_FLASHLIGHT,
    APP_DINO_GAME,
    APP_BREAKOUT_GAME,
    APP_SIMON_SAYS,
    APP_REACTION_TIME,
    APP_MATRIX_SCREENSAVER,
    APP_EYE,
    APP_COUNT
};

class AppManager {
public:
    static AppManager& instance() {
        static AppManager s;
        return s;
    }
    void switchToApp(AppIndex) {}
    void loop() {}
private:
    AppManager() {}
};

#endif
