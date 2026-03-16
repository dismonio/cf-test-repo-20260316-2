// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef REACTION_TIME_GAME_H
#define REACTION_TIME_GAME_H

#include "DisplayProxy.h"
#include "ButtonManager.h"
#include "MenuManager.h" // For returning to menu

/**
 * @brief Reaction Time Game Class
 */
class ReactionTimeGame {
public:
    /**
     * @brief Constructor for ReactionTimeGame
     * 
     * @param buttonManager Reference to the ButtonManager instance
     */
    ReactionTimeGame(ButtonManager& buttonManager);

    /**
     * @brief Standard AppManager integration:
     *  Called when app is started from the menu
     */
    void begin();

    /**
     * @brief Standard AppManager integration:
     *  Called when app is closed or user goes back to menu
     */
    void end();

    /**
     * @brief Standard AppManager integration:
     *  Called every loop; we use it to internally call the original update(millis_NOW).
     */
    void update();

    /**
     * @brief Static callback function to handle button presses for ReactionTimeGame
     * 
     * @param ev ButtonEvent struct containing event details
     */
    static void reactionButtonPressedCallback(const ButtonEvent& ev);

    /**
     * @brief Static callback function for the "Back to menu" button
     */
    static void onButtonBackPressed(const ButtonEvent& ev);

    static ReactionTimeGame* instance;


    /**
     * @brief Update method to be called periodically by external code 
     *        (kept for backward compatibility if you manually call update(millis_NOW))
     * 
     * @param millis_NOW Current time in milliseconds
     */
    void update(unsigned long millis_NOW);

    /**
     * @brief Reset the game to its initial state
     */
    void resetGame();

private:
    DisplayProxy& display;
    ButtonManager& buttonManager;

    unsigned long startTime;
    unsigned long reactionTime;
    unsigned long randomDelayEnd;
    bool gameStarted;
    bool waitingForReaction;
    bool delayActive;
    bool messageDisplayed;

    /**
     * @brief Handle a button press event
     */
    void handleButtonPress();

    /**
     * @brief Start the reaction time game
     * 
     * @param millis_NOW Current time in milliseconds
     */
    void startGame(unsigned long millis_NOW);

    /**
     * @brief Display the reaction time to the user
     */
    void displayReactionTime();
};

extern ReactionTimeGame reactionTimeGame;

#endif
