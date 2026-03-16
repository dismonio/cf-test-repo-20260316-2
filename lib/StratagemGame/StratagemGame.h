// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef STRATAGEM_GAME_H
#define STRATAGEM_GAME_H

#include <Arduino.h>
#include <DisplayProxy.h>
#include "ButtonManager.h"
#include "AudioManager.h"
#include "MenuManager.h"

// Forward-declare our sprite-drawing function or any references
// We will include the actual sprite arrays in the .cpp
class StratagemGame {
public:
    /**
     * @brief States of the game
     */
    enum State {
        WAIT_START,
        RUNNING,
        HITLAG,
        GAME_OVER
    };

    StratagemGame(ButtonManager &btnMgr, AudioManager &audioMgr);

    void begin();
    void update();
    void end();

private:
    // References
    DisplayProxy &display;
    ButtonManager &buttonManager;
    AudioManager &audioManager;

    // Singleton instance pointer for static callbacks
    static StratagemGame* instance;

    // Game state
    State currentState;

    // Score
    int score;

    // Time tracking
    static const unsigned long TOTAL_TIME     = 10000UL; // ms
    static const unsigned long HITLAG_TIMEOUT = 200UL;   // ms
    static const unsigned long TIME_BONUS     = 1000UL;   // ms
    unsigned long timeRemaining;
    unsigned long lastUpdateTime;
    unsigned long hitlagStartTime;

    // A minimal struct for a single stratagem
    struct Stratagem {
        const char* name;
        const char* sequence; // e.g. "UDRLU"
        const char* image;    // optional
    };

    // The “active” stratagem and a set of all possible
    Stratagem currentStratagem;
    static const Stratagem s_stratagemData[];  // we embed them in .cpp
    static const int NUM_STRATAGEMS;

    // Sequence progress index
    int currentSequenceIndex;

    // Helpers
    void resetGame();
    void pickRandomStratagem();
    void checkInput(char direction);
    void handleHitlag();
    void gameOver();

    // Drawing
    void drawScreen();
    void drawGameOver();
    void drawRunning();
    void drawArrowSprite(char direction, int16_t x, int16_t y, bool filled);

    // Audio
    void playDirectionSound(char direction);

    // Static callbacks
    static void onButtonEvent(const ButtonEvent& event);
    static void onButtonBackPressed(const ButtonEvent& event);
    static void onButtonEnterPressed(const ButtonEvent& event);
};

extern StratagemGame stratagemGame;

#endif // STRATAGEM_GAME_H
