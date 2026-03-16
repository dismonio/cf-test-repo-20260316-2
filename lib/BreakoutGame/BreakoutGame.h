// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

// lib/BreakoutGame/BreakoutGame.h

#ifndef BREAKOUT_GAME_H
#define BREAKOUT_GAME_H

#include "DisplayProxy.h"
#include "ButtonManager.h"
#include "AudioManager.h"
#include "globals.h" // For button indices
#include <functional> // For std::function

class BreakoutGame {
public:
    BreakoutGame(ButtonManager& buttonManager, AudioManager& audioManager);

    void begin();  // Start the game
    void update(); // Update game logic
    void end();    // Cleanup and unregister callbacks
    void draw();   // Draw the game

    // Static instance pointer for callbacks
    static BreakoutGame* instance;

private:
    // AppManager Integration
    void registerButtonCallbacks();
    void unregisterButtonCallbacks();
    static void buttonPressedCallback(const ButtonEvent& event);
    static void onButtonBackPressed(const ButtonEvent& event);

    // References to hardware components
    DisplayProxy& display;
    ButtonManager& buttonManager;
    AudioManager& audioManager;

    // Game properties and state variables
    // Screen dimensions
    static constexpr int SCREEN_WIDTH  = 128;
    static constexpr int SCREEN_HEIGHT = 64;

    // Paddle properties
    float paddleX;                     // Left edge of the paddle
    static constexpr int PADDLE_Y      = SCREEN_HEIGHT - 8; // Near bottom
    static constexpr int PADDLE_WIDTH  = 20;
    static constexpr int PADDLE_HEIGHT = 3;
    float paddleSpeed; // Multiplier for paddle movement speed

    // Ball properties
    float ballX, ballY;    // Ball position
    float ballVX, ballVY;  // Ball velocity
    static constexpr int BALL_SIZE = 2; // 2x2 pixel ball

    // Bricks
    static constexpr int BRICK_ROWS   = 3;
    static constexpr int BRICK_COLS   = 8;
    static constexpr int BRICK_WIDTH  = 16; 
    static constexpr int BRICK_HEIGHT = 4;
    bool bricks[BRICK_ROWS][BRICK_COLS]; // true if brick is active

    // Game state
    int  deathCount;   // Number of times the ball hits the bottom
    bool gameWon;      // True if all bricks are destroyed

    // Timer for how long it takes to destroy all bricks
    unsigned long startTime; // When the game started/restarted
    unsigned long totalTime; // Final time it took (in ms) once game is won

    // Reset button information
    int resetButtonIndex;

    // Option to enable/disable brick hit sounds
    bool brickSoundsEnabled;

    // Private methods
    void resetGame();           // Reset the game to initial state
    void movePaddle(float Ax);  // Move the paddle based on input
    void moveBall();            // Move the ball
    void checkCollisions();     // Check for collisions
    void checkVictory();        // Check if the game is won
    void drawGame();            // Draw the game elements
    void playBounceSound();     // Play sound when the ball bounces

    // Static button event handler
    static void onButtonEvent(const ButtonEvent& event);
};

extern BreakoutGame breakoutGame;

#endif // BREAKOUT_GAME_H