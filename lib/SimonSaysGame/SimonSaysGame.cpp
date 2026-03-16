// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "SimonSaysGame.h"
#include "globals.h"  // Replace with your actual global button index definitions
#include "HAL.h"

SimonSaysGame simonSaysGame(HAL::buttonManager(), HAL::audioManager()); // AppManager Integration
// Initialize static instance pointer
SimonSaysGame* SimonSaysGame::instance = nullptr;

// Define how the Simon game indexes map to your actual hardware buttons
const int SimonSaysGame::buttonMappings[4] = {
    button_TopLeftIndex,  // game button 0 = this physical button
    button_TopRightIndex, // game button 1 = ...
    button_MiddleLeftIndex,  // game button 2 = ...
    button_MiddleRightIndex  // game button 3 = ...
};

//--------------------- Constructor / Setup ---------------------------

SimonSaysGame::SimonSaysGame(
    ButtonManager &btnMgr,
    AudioManager &audMgr
)
    : display(HAL::displayProxy()),
      buttonManager(btnMgr),
      audioManager(audMgr),
      patternLength(0),
      patternIndex(0),
      score(0),
      currentState(WAIT_START),
      showStartTime(0),
      showStepTime(0),
      showingSquare(false),
      gapPhase(false),
      gameOverStartTime(0),
      finalWaitStartTime(0),
      currentlyPressedButton(-1)
{
    for (int i = 0; i < MAX_PATTERN; i++) {
        pattern[i] = -1;
    }

    // Set the static instance pointer to this instance
    SimonSaysGame::instance = this;
}

void SimonSaysGame::begin() {
    // Register button callbacks
    for (int i = 0; i < 4; i++) {
        buttonManager.registerCallback(buttonMappings[i], SimonSaysGame::onButtonEvent);
    }
    buttonManager.registerCallback(button_BottomLeftIndex, onButtonBackPressed);

    // Reset the game state
    resetGame();
}

void SimonSaysGame::end() {
    // Unregister callbacks
    for (int i = 0; i < 4; i++) {
        buttonManager.unregisterCallback(buttonMappings[i]);
    }

    buttonManager.unregisterCallback(button_BottomLeftIndex);
    // Stop any playing tones
    audioManager.stopTone();
}

//--------------------- Main Loop Update ---------------------------

void SimonSaysGame::update() {
    switch (currentState) {
    case WAIT_START:
        // Draw an idle screen if desired, or do nothing
        // We keep the grid blank or with a message
        drawGrid(-1);
        // The user must press any valid button to start
        break;

    case SHOW_PATTERN:
        // We are in the middle of showing the Simon pattern
        showPattern();
        break;

    case WAIT_USER:
        // We wait for user input. If a button is pressed, it is handled in onButtonEvent
        // Meanwhile, highlight the currently pressed button (if any)
        if (currentlyPressedButton >= 0) {
            drawGrid(currentlyPressedButton);
        } else {
            drawGrid(-1);
        }
        break;

    case WAIT_FINAL:
        // The user has just pressed the correct final button in the sequence.
        // We stay in this state until:
        //   1) The user releases that button
        //   2) We wait an additional FINAL_WAIT ms, then start the next round

        // If user is still holding the button, highlight it
        if (currentlyPressedButton >= 0) {
            drawGrid(currentlyPressedButton);
        } else {
            // Once the button is released, check how long we've been here
            // If finalWaitStartTime == 0, we set it now
            if (finalWaitStartTime == 0) {
                finalWaitStartTime = millis();
            }
            // Once we've waited the required time, start the next round
            if (millis() - finalWaitStartTime >= FINAL_WAIT) {
                startRound();
            } else {
                drawGrid(-1);
            }
        }
        break;

    case GAME_OVER:
        // Show "Game Over" for a while
        if (millis() - gameOverStartTime < 3000) {
            drawGameOver();
        } else {
            // After some time, reset the game, so user can start again
            resetGame();
        }
        break;
    }
}

//--------------------- Button Events ---------------------------

void SimonSaysGame::onButtonEvent(const ButtonEvent& event) {
    if (!instance) return;

    // Figure out which game button (0..3) was pressed based on your hardware index
    int pressedButtonIndex = event.buttonIndex;
    int gameButtonIndex = -1;
    for (int i = 0; i < 4; i++) {
        if (pressedButtonIndex == buttonMappings[i]) {
            gameButtonIndex = i;
            break;
        }
    }
    if (gameButtonIndex == -1) {
        return; // Not one of the SimonSays buttons
    }

    // Press
    if (event.eventType == ButtonEvent_Pressed) {
        if (instance->currentState == WAIT_START) {
            // Start the first round
            instance->startRound();
        }
        else if (instance->currentState == WAIT_USER) {
            // Register which button is pressed
            instance->currentlyPressedButton = gameButtonIndex;
            // Check if it's correct
            instance->checkUserInput(gameButtonIndex);
            // Start user-press tone
            instance->playBeepOnUserPress(gameButtonIndex);
        }
        else if (instance->currentState == WAIT_FINAL) {
            // If we’re in WAIT_FINAL, the user might press something else
            // but we typically do nothing, or you could treat it as an error.
            // For simplicity, ignore new presses in WAIT_FINAL.
        }
    }

    // Release
    else if (event.eventType == ButtonEvent_Released) {
        if (instance->currentlyPressedButton == gameButtonIndex) {
            // Stop tone
            instance->audioManager.stopTone();
            instance->currentlyPressedButton = -1;
        }
    }
}

//--------------------- Game Flow Methods ---------------------------

void SimonSaysGame::resetGame() {
    patternLength = 0;
    patternIndex  = 0;
    score         = 0;
    currentState  = WAIT_START;
    currentlyPressedButton = -1;
    finalWaitStartTime = 0;
    // Clear out pattern
    for (int i = 0; i < MAX_PATTERN; i++) {
        pattern[i] = -1;
    }
    // Stop any tones
    audioManager.stopTone();
}

void SimonSaysGame::startRound() {
    extendPattern();
    patternIndex = 0;
    showingSquare = false;
    gapPhase = false;
    showStartTime = millis();
    showStepTime  = showStartTime;

    currentState = SHOW_PATTERN;
    finalWaitStartTime = 0; // reset final wait for the new round
}

void SimonSaysGame::extendPattern() {
    if (patternLength < MAX_PATTERN) {
        pattern[patternLength] = random(0, 4); // 0..3
        patternLength++;
    }
}

/**
 * @brief Show the Simon pattern one step at a time
 */
void SimonSaysGame::showPattern() {
    unsigned long now = millis();

    // If we've shown all steps
    if (patternIndex >= patternLength) {
        // Move on to user input
        currentState = WAIT_USER;
        patternIndex = 0;
        drawGrid(-1);
        return;
    }

    // Are we turning a square ON right now?
    if (!showingSquare && !gapPhase) {
        int sq = pattern[patternIndex];
        drawGrid(sq);
        showingSquare = true;
        showStepTime  = now;
        // Play beep for that square
        playBeepForSquare(sq);
    }
    // Have we shown it long enough?
    else if (showingSquare && (now - showStepTime > SHOW_STEP_ON)) {
        // Turn the square off, go to gap
        drawGrid(-1);
        showingSquare = false;
        gapPhase = true;
        showStepTime = now;
        // Stop tone
        audioManager.stopTone();
    }
    // Then we wait in gap phase for SHOW_STEP_OFF ms
    else if (gapPhase && (now - showStepTime > SHOW_STEP_OFF)) {
        gapPhase = false;
        patternIndex++;
    }
}

/**
 * @brief Check if the pressed button is correct in sequence
 */
void SimonSaysGame::checkUserInput(int pressedButton) {
    if (pressedButton == pattern[patternIndex]) {
        // Correct so far
        patternIndex++;

        // If that press completed the sequence
        if (patternIndex >= patternLength) {
            // The user has matched the entire pattern, so we move
            // to WAIT_FINAL. We do NOT immediately start a new round.
            score = patternLength;
            currentState = WAIT_FINAL;
            // finalWaitStartTime is set later, once they release the button
        }
        // else we just keep waiting for the next correct press
    } else {
        // Wrong answer -> game over
        gameOver();
    }
}

void SimonSaysGame::gameOver() {
    currentState = GAME_OVER;
    gameOverStartTime = millis();
    currentlyPressedButton = -1; 
    audioManager.stopTone();
}

//--------------------- Drawing/Display Helpers ---------------------------

void SimonSaysGame::drawGameOver() {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 20, "GAME OVER");
    String msg = "Score: ";
    msg += score;
    display.drawString(64, 40, msg);
    display.display();
}

void SimonSaysGame::drawGrid(int highlightIndex) {
    display.clear();

    // Draw horizontal and vertical lines to make a 4-square layout
    display.drawLine(0, 31, 127, 31);  // horizontal line across middle
    display.drawLine(63, 0, 63, 63);   // vertical line across middle

    // Highlight a square if specified
    if (highlightIndex != -1) {
        fillSquare(highlightIndex);
    }

    display.display();
}

void SimonSaysGame::fillSquare(int sq) {
    int xStart = 0;
    int yStart = 0;
    int width  = 63;
    int height = 31;

    switch (sq) {
    case 0: // Top-left
        xStart = 0;    yStart = 0;
        break;
    case 1: // Top-right
        xStart = 64;   yStart = 0;
        break;
    case 2: // Bottom-left
        xStart = 0;    yStart = 32;
        break;
    case 3: // Bottom-right
        xStart = 64;   yStart = 32;
        break;
    default:
        return;
    }

    display.fillRect(xStart, yStart, width, height);
}

//--------------------- Audio ---------------------------

void SimonSaysGame::playBeepForSquare(int sq) {
    // Frequencies for the pattern
    float frequencies[4] = { 261.63f, 329.63f, 392.00f, 523.25f }; // C4, E4, G4, C5
    if (sq >= 0 && sq < 4) {
        audioManager.playTone(frequencies[sq]);
    }
}

void SimonSaysGame::playBeepOnUserPress(int sq) {
    // Frequencies for user-press (usually a bit higher)
    float frequencies[4] = { 523.25f, 659.25f, 783.99f, 1046.50f }; // C5, E5, G5, C6
    if (sq >= 0 && sq < 4) {
        audioManager.playTone(frequencies[sq]);
    }
}

void SimonSaysGame::onButtonBackPressed(const ButtonEvent& event)
{    // Press
    if (event.eventType == ButtonEvent_Released){
        ESP_LOGI(TAG_MAIN, "onButtonBackPressed => calling end() + returning to menu...");
        instance->end();
        MenuManager::instance().returnToMenu();
    } 
}