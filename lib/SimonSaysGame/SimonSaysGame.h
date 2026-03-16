// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef SIMON_SAYS_GAME_H
#define SIMON_SAYS_GAME_H

#include <Arduino.h>
#include <DisplayProxy.h>
#include "ButtonManager.h"
#include "AudioManager.h"
#include "MenuManager.h" // For returning back to menu

/**
 * @brief Simon Says Game Class
 */
class SimonSaysGame {
public:
    static const int MAX_PATTERN = 32;

    /**
     * @brief High-level states of the game
     */
    enum State {
        WAIT_START,   // Waiting for user to start the first round
        SHOW_PATTERN, // Currently showing the Simon pattern
        WAIT_USER,    // Waiting for user to press correct buttons
        WAIT_FINAL,   // User just got the final input correct; waiting for final release + delay
        GAME_OVER     // The user has lost
    };

    /**
     * @brief Constructor for SimonSaysGame
     */
    SimonSaysGame(
        ButtonManager &buttonMgr,
        AudioManager &audioMgr
    );

    /**
     * @brief Initialize the game and register button callbacks
     */
    void begin();

    /**
     * @brief Update the game logic (call in loop())
     */
    void update();

    /**
     * @brief Cleanup and unregister button callbacks
     */
    void end();

    /**
     * @brief Static callback for button events
     */
    static void onButtonEvent(const ButtonEvent& event);

private:
    // References
    DisplayProxy &display;
    ButtonManager &buttonManager;
    AudioManager &audioManager;

    // Pattern data
    int  pattern[MAX_PATTERN];
    int  patternLength;
    int  patternIndex;
    int  score;

    // Game state
    State currentState;
    int   currentlyPressedButton;

    // Timing for showing pattern
    unsigned long showStartTime;
    unsigned long showStepTime;
    static const unsigned long SHOW_STEP_ON  = 500; // how long to show
    static const unsigned long SHOW_STEP_OFF = 200; // gap between steps
    bool showingSquare;
    bool gapPhase;

    // Timing for game over message
    unsigned long gameOverStartTime;

    // Timing for final delay
    unsigned long finalWaitStartTime;     // when we begin the final delay
    static const unsigned long FINAL_WAIT = 750; // how long to wait after final release

    // Button indices used by this game
    static const int buttonMappings[4];

    // Singleton instance pointer for static callback
    static SimonSaysGame* instance;

    // Private methods
    void resetGame();
    void startRound();
    void extendPattern();
    void showPattern();
    void checkUserInput(int pressedButton);
    void gameOver();

    // Display helpers
    void drawGrid(int highlightIndex);
    void fillSquare(int sq);
    void drawGameOver();

    // Audio methods
    void playBeepForSquare(int sq);
    void playBeepOnUserPress(int sq);

    // App Manager Integration
    static void onButtonBackPressed(const ButtonEvent& event);
};

extern SimonSaysGame simonSaysGame;

#endif
