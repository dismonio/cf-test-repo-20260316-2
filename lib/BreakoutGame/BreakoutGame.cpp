// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

// lib/BreakoutGame/BreakoutGame.cpp

#include "BreakoutGame.h"
#include "globals.h" // For button indices
#include "HAL.h"
#include "MenuManager.h" // AppManager integration

BreakoutGame breakoutGame(HAL::buttonManager(), HAL::audioManager()); // AppManager Integration
// Initialize the static instance pointer
BreakoutGame* BreakoutGame::instance = nullptr;

BreakoutGame::BreakoutGame(ButtonManager& btnMgr, AudioManager& audioMgr)
    : display(HAL::displayProxy()), buttonManager(btnMgr), audioManager(audioMgr),
      brickSoundsEnabled(true) // Initialize brick crash sounds to enabled
{
    instance = this; // Set the static instance pointer
    resetButtonIndex = button_BottomRightIndex; // Use the appropriate button index
}

void BreakoutGame::begin() {
    registerButtonCallbacks(); // AppManager integration
    // Reset the game state
    resetGame();

    // Register button callback for the reset button
    buttonManager.registerCallback(resetButtonIndex, BreakoutGame::onButtonEvent);
}

void BreakoutGame::end() {
    // Unregister the button callback
    unregisterButtonCallbacks();

    // Stop any playing sounds
    audioManager.stopTone();
}

// Static button event handler
void BreakoutGame::onButtonEvent(const ButtonEvent& event) {
    if (instance) {
        if (event.buttonIndex == instance->resetButtonIndex && event.eventType == ButtonEvent_Pressed) {
            instance->resetGame();
        }
    }
}

// Reset the game to its initial state
void BreakoutGame::resetGame() {
    // Paddle
    paddleX     = (SCREEN_WIDTH - PADDLE_WIDTH) / 2.0f; 
    paddleSpeed = 2.0f;  

    // Ball
    ballX  = SCREEN_WIDTH / 2.0f;
    ballY  = SCREEN_HEIGHT / 2.0f;
    ballVX = 1.0f;
    ballVY = -1.0f;

    // Bricks: reset all to active
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            bricks[r][c] = true;
        }
    }

    // Game state
    deathCount = 0;
    gameWon    = false;

    // Start timing now
    startTime  = millis();
    totalTime  = 0U;

    // Stop any playing sounds
    audioManager.stopTone();
}

// Update method to be called in the main loop
void BreakoutGame::update() {
    if (gameWon) {
        // Game over; just draw the game
        drawGame();
        return;
    }

    // Move paddle based on accelerometer input
    movePaddle(accelX);

    // Move the ball
    moveBall();

    // Check collisions
    checkCollisions();

    // Check for victory
    checkVictory();

    // Draw the game
    drawGame();
}

// Move the paddle based on accelerometer input
void BreakoutGame::movePaddle(float accelX) {
    // Adjust paddle position based on accelerometer input
    paddleX += accelX * paddleSpeed * -0.01f;

    // Keep paddle within screen bounds
    if (paddleX < 0) {
        paddleX = 0;
    } else if (paddleX + PADDLE_WIDTH > SCREEN_WIDTH) {
        paddleX = SCREEN_WIDTH - PADDLE_WIDTH;
    }
}

// Move the ball
void BreakoutGame::moveBall() {
    ballX += ballVX;
    ballY += ballVY;
}

// Check for collisions with walls, paddle, and bricks
void BreakoutGame::checkCollisions() {
    // Left/right walls
    if (ballX <= 0) {
        ballX = 0;
        ballVX *= -1;
        // Removed sound for wall collision
    } else if (ballX + BALL_SIZE >= SCREEN_WIDTH) {
        ballX = SCREEN_WIDTH - BALL_SIZE;
        ballVX *= -1;
        // Removed sound for wall collision
    }

    // Top wall
    if (ballY <= 0) {
        ballY = 0;
        ballVY *= -1;
        // Removed sound for wall collision
    }

    // Bottom wall -> "death"
    if (ballY + BALL_SIZE >= SCREEN_HEIGHT) {
        deathCount++;
        ballX  = SCREEN_WIDTH / 2.0f;
        ballY  = SCREEN_HEIGHT / 2.0f;
        ballVX = (random(2) ? 1.0f : -1.0f);
        ballVY = -1.0f;
        return;
    }

    // Paddle collision
    if ((ballY + BALL_SIZE) >= PADDLE_Y && ballY <= (PADDLE_Y + PADDLE_HEIGHT)) {
        if ((ballX + BALL_SIZE) >= paddleX && ballX <= (paddleX + PADDLE_WIDTH)) {
            ballY = PADDLE_Y - BALL_SIZE;
            ballVY *= -1;
            playBounceSound(); // Play sound on paddle hit
            millis_APP_LASTINTERACTION = millis_NOW; // Reset last interaction time for keep-alive
        }
    }

    // Bricks collision
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            if (!bricks[r][c]) continue;

            int brickX = c * BRICK_WIDTH;
            int brickY = r * BRICK_HEIGHT;
            if ((ballX + BALL_SIZE) >= brickX &&
                ballX <= (brickX + BRICK_WIDTH) &&
                (ballY + BALL_SIZE) >= brickY &&
                ballY <= (brickY + BRICK_HEIGHT))
            {
                bricks[r][c] = false;
                ballVY *= -1;

                // Adjust ball position to prevent sticking
                if (ballVY > 0) {
                    ballY = brickY + BRICK_HEIGHT;
                } else {
                    ballY = brickY - BALL_SIZE;
                }

                // Play sound when breaking a brick if enabled
                if (brickSoundsEnabled) {
                    playBounceSound();
                    millis_APP_LASTINTERACTION = millis_NOW; // Reset last interaction time for keep-alive
                }

                return; 
            }
        }
    }
}

// Check if all bricks are destroyed
void BreakoutGame::checkVictory() {
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            if (bricks[r][c]) {
                // Found an active brick
                return; 
            }
        }
    }
    // No bricks found: player wins!
    gameWon    = true;
    totalTime  = millis() - startTime; // Calculate how long it took in ms

    // Stop any playing sounds
    audioManager.stopTone();
}

// Play a sound when the ball bounces
void BreakoutGame::playBounceSound() {
    // Frequency for bounce sound
    float freq = 600.0f;
    // Duration in milliseconds
    int durationMs = 100; // Play sound for 100 ms

    // Play the tone
    audioManager.playTone(freq, durationMs);
}

// Draw the game on the display
void BreakoutGame::draw() {
    // Drawing is handled in drawGame()
    drawGame();
}

// Draw everything on the display
void BreakoutGame::drawGame() {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);

    // If the user has won, display a "YOU WIN" message, death count, and time
    if (gameWon) {
        display.drawString(64, 4, "YOU WIN!");

        String deathMsg = "Deaths: ";
        deathMsg += deathCount;
        display.drawString(64, 14, deathMsg);

        // Show how many seconds it took
        float seconds = totalTime / 1000.0f;
        String timeMsg = "Time: ";
        timeMsg += String(seconds, 2); // 2 decimal places
        timeMsg += "s";
        display.drawString(64, 24, timeMsg);

        display.drawString(64, 34, "Press Btn to Reset");
        display.display();
        return;
    }

    // Draw the paddle
    display.fillRect(static_cast<int>(paddleX), PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT);

    // Draw the ball
    display.fillRect(static_cast<int>(ballX), static_cast<int>(ballY), BALL_SIZE, BALL_SIZE);

    // Draw bricks
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            if (!bricks[r][c]) continue;
            int brickX = c * BRICK_WIDTH;
            int brickY = r * BRICK_HEIGHT;
            display.fillRect(brickX, brickY, BRICK_WIDTH - 1, BRICK_HEIGHT - 1); // -1 for spacing
        }
    }

    // Show current death count
    String deathMsg = "Deaths: ";
    deathMsg += deathCount;
    display.drawString(64, 20, deathMsg);

    display.display();
}

// AppManager integration

void BreakoutGame::registerButtonCallbacks() {
    // Exit App
    buttonManager.registerCallback(button_BottomLeftIndex, onButtonBackPressed);
}

void BreakoutGame::unregisterButtonCallbacks() {
    // Unregister callbacks
    buttonManager.unregisterCallback(button_BottomLeftIndex);
}

void BreakoutGame::onButtonBackPressed(const ButtonEvent& event)
{    // Press
    if (event.eventType == ButtonEvent_Released){
        ESP_LOGI(TAG_MAIN, "onButtonBackPressed => calling end() + returning to menu...");
        instance->end();
        MenuManager::instance().returnToMenu();
    } 
}