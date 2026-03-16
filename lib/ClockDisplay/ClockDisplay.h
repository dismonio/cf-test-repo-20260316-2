// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef CLOCK_DISPLAY_H
#define CLOCK_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include "DisplayProxy.h"
#include <time.h>
#include <Preferences.h>
#include "ButtonManager.h"

/*
 * Class: ClockDisplay
 * -------------------
 * This class implements a clock app that uses the ESP32 system time when available.
 * If WiFi/NTP isn’t available the module uses a low‐accuracy internal timer (via millis)
 * and displays a “Low Accuracy” notice under the time.
 *
 * The update() method is non-blocking so that you can interrupt it (i.e. switch apps) at any time.
 */
class ClockDisplay {
public:
    // Constructor
    ClockDisplay();

    // Called once at startup (or when switching to the clock app). It loads saved time data,
    // and if WiFi is connected, it calls configTzTime() so that the system time will update.
    void begin();

    // Non-blocking update function that should be called from the main loop.
    // It updates the internal clock (either using system time or our internal timer) and refreshes the display.
    void update();

    // Save the current time (and current millis) to Preferences. This helps us recover the time
    // after deep sleep if WiFi/NTP isn’t available.
    void saveTime();

    // Optionally, add a reset() method if you want to reinitialize on app switch.
    void reset();

    void end();    // Unregister callbacks and cleanup

private:
    DisplayProxy& m_display;  // Reference to the display instance
    ButtonManager& buttonManager; // Reference to the button manager instance

    Preferences m_preferences;  // For storing time data between deep sleep sessions

    bool m_accurateTime;    // True if time has been synced via NTP (system time is “good”)
    bool m_initialized; // New: tracks if begin() has been successfully run.
    time_t m_currentTime;   // The current epoch time
    unsigned long m_lastUpdateMillis;  // For scheduling display refreshes
    const unsigned long m_updateInterval = 1000; // Update display every 1 second

    // For low-accuracy (fallback) mode: these variables form the base of our internal timer.
    time_t m_internalBaseTime;       // The last known time saved to NVS (or set during begin)
    unsigned long m_internalBaseMillis; // The millis() value at which the internal base time was recorded

    // Attempt to sync the system time (from NTP) if WiFi is available.
    // This is non-blocking; if successful, m_accurateTime is set true.
    void attemptTimeSync();

    // Format and draw the time on the display. In addition to the main time, it will display
    // a “Low Accuracy” label if we haven’t yet synced the accurate time.
    void displayTime();

    // Returns true if the system time is valid (i.e. it has been updated via NTP).
    bool isSystemTimeValid();

    /* Buttons */

    // For handling button presses
    static ClockDisplay* instance; // To allow static callbacks

    void registerButtonCallbacks();
    void unregisterButtonCallbacks();

    // Button callback functions
    static void buttonPressedCallback(const ButtonEvent& event);
    void handleButtonEvent(const ButtonEvent& event); // Define this function

    // We will have one callback function for each button
    static void onButtonLeftPressed(const ButtonEvent& event);
    static void onButtonRightPressed(const ButtonEvent& event);
    static void onButtonUpPressed(const ButtonEvent& event);
    static void onButtonDownPressed(const ButtonEvent& event);
    static void onButtonBackPressed(const ButtonEvent& event);
    static void onButtonSelectPressed(const ButtonEvent& event);
};

// Declare that there's a global object called below somewhere for AppDefs to use
// must be after the reference class definition exists
extern ClockDisplay clockDisplay;

#endif // CLOCK_DISPLAY_H