// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "SerialDisplayApp.h"
#include "HAL.h"
#include "globals.h" // If you need logging or other global references

// Define static pointer
SerialDisplayApp* SerialDisplayApp::instance = nullptr;

// Define the global instance for the AppManager
SerialDisplayApp serialDisplayApp(HAL::buttonManager());

// -----------------------------------------------
// Constructor
// -----------------------------------------------
SerialDisplayApp::SerialDisplayApp(ButtonManager& btnMgr)
    : buttonManager(btnMgr)
{
    instance = this; // store pointer so static callbacks can reach 'this'
}

// -----------------------------------------------
// AppManager Integration
// -----------------------------------------------
void SerialDisplayApp::begin() {
    // Register your scroll up, scroll down, toggle, back button, etc.
    // Pick whichever hardware buttons you want:
    buttonManager.registerCallback(button_MiddleLeftIndex, onButtonScrollUp);
    buttonManager.registerCallback(button_MiddleRightIndex, onButtonScrollDown);
    buttonManager.registerCallback(button_BottomRightIndex, onButtonToggleScrollMode);
    buttonManager.registerCallback(button_BottomLeftIndex, onButtonBackPressed);

    // Optional: If you want to reset the screen each time you open the app:
    // currentScrollMode = PIXEL_SCROLL; 
    // lineCount = 0;
    // ...
    // or drawDefaultInfoScreen();

    ESP_LOGI(TAG_MAIN, "[SerialDisplayApp] begin() => callbacks registered");
}

void SerialDisplayApp::end() {
    // Unregister all callbacks used by this app
    buttonManager.unregisterCallback(button_MiddleLeftIndex);
    buttonManager.unregisterCallback(button_MiddleRightIndex);
    buttonManager.unregisterCallback(button_BottomRightIndex);
    buttonManager.unregisterCallback(button_BottomLeftIndex);

    ESP_LOGI(TAG_MAIN, "[SerialDisplayApp] end() => callbacks unregistered");
}

void SerialDisplayApp::update() {
    // Just draw the serial data screen each frame
    drawSerialDataScreen();
}

// -----------------------------------------------
// Static Callbacks -> Forward to instance
// -----------------------------------------------
void SerialDisplayApp::onButtonBackPressed(const ButtonEvent& ev) {
    if (instance) {
        instance->handleButtonBack(ev);
    }
}

void SerialDisplayApp::onButtonScrollUp(const ButtonEvent& ev) {
    if (instance) {
        instance->handleScrollUp(ev);
    }
}

void SerialDisplayApp::onButtonScrollDown(const ButtonEvent& ev) {
    if (instance) {
        instance->handleScrollDown(ev);
    }
}

void SerialDisplayApp::onButtonToggleScrollMode(const ButtonEvent& ev) {
    if (instance) {
        instance->handleToggleScrollMode(ev);
    }
}

// -----------------------------------------------
// Non-static handler methods
// -----------------------------------------------
void SerialDisplayApp::handleButtonBack(const ButtonEvent& ev) {
    if (ev.eventType == ButtonEvent_Released) {
        ESP_LOGI(TAG_MAIN, "[SerialDisplayApp] Back button pressed => returning to menu...");
        end();
        MenuManager::instance().returnToMenu();
    }
}

void SerialDisplayApp::handleScrollUp(const ButtonEvent& ev) {
    // Only act on pressed or released as you prefer
    if (ev.eventType == ButtonEvent_Pressed) {
        ::handleScrollUp();  // calls your existing global function
        ESP_LOGI(TAG_MAIN, "[SerialDisplayApp] Scroll Up Pressed");
    }
}

void SerialDisplayApp::handleScrollDown(const ButtonEvent& ev) {
    if (ev.eventType == ButtonEvent_Pressed) {
        ::handleScrollDown();
        ESP_LOGI(TAG_MAIN, "[SerialDisplayApp] Scroll Down Pressed");
    }
}

void SerialDisplayApp::handleToggleScrollMode(const ButtonEvent& ev) {
    if (ev.eventType == ButtonEvent_Pressed) {
        ::toggleScrollMode();
        ESP_LOGI(TAG_MAIN, "[SerialDisplayApp] Toggle Scroll Mode Pressed");
    }
}
