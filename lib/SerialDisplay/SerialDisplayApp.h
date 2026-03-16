// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef SERIALDISPLAYAPP_H
#define SERIALDISPLAYAPP_H

#include "ButtonManager.h"
#include "MenuManager.h"
#include "SerialDisplay.h"

/**
 * @brief AppManager wrapper for the serial display functionality.
 */
class SerialDisplayApp {
public:
    /**
     * @brief Constructor requires a ButtonManager reference
     *        so we can register button callbacks.
     */
    SerialDisplayApp(ButtonManager& btnMgr);

    /**
     * @brief Called when entering the app from the menu.
     */
    void begin();

    /**
     * @brief Called when exiting the app (back to menu).
     */
    void end();

    /**
     * @brief Called every loop; we call `drawSerialDataScreen()`.
     */
    void update();

    // --- Static callbacks so we can pass them to ButtonManager
    static void onButtonBackPressed(const ButtonEvent& ev);
    static void onButtonScrollUp(const ButtonEvent& ev);
    static void onButtonScrollDown(const ButtonEvent& ev);
    static void onButtonToggleScrollMode(const ButtonEvent& ev);

    // Singleton pointer so static callbacks can access 'this'
    static SerialDisplayApp* instance;

private:
    ButtonManager& buttonManager;

    /**
     * @brief Handle the back button event (non-static).
     */
    void handleButtonBack(const ButtonEvent& ev);

    /**
     * @brief Handle the scroll-up button (non-static).
     */
    void handleScrollUp(const ButtonEvent& ev);

    /**
     * @brief Handle the scroll-down button (non-static).
     */
    void handleScrollDown(const ButtonEvent& ev);

    /**
     * @brief Handle toggling the scroll mode (non-static).
     */
    void handleToggleScrollMode(const ButtonEvent& ev);
};

// Global instance for the AppManager
extern SerialDisplayApp serialDisplayApp;

#endif // SERIALDISPLAYAPP_H
