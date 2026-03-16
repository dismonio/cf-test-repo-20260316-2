// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "Flashlight.h"
#include "RGBController.h"   // For LED strip control and pixel definitions
#include "globals.h"         // For sliderPosition_8Bits, flashlightStatus, etc.
#include "HAL.h"           // For HAL::displayProxy()
#include "MenuManager.h"     // For returning to the menu

Flashlight flashlight(HAL::buttonManager());

Flashlight* Flashlight::instance = nullptr;

Flashlight::Flashlight(ButtonManager& btnMgr) :
    buttonManager(btnMgr),
    display(HAL::displayProxy())  // Use the DisplayProxy from HAL
{
    instance = this;
}

void Flashlight::update() {

     // Clear and set up the display for the flashlight screen
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 18, "Flashlight");
    display.display();
    
    // Use brightness derived from sliderPosition_8Bits for consistency
    uint8_t brightness = sliderPosition_8Bits_Inverted_Filtered;

    // Set specific pixels for flashlight mode using RGBController's strip and pixel definitions
    // strip.setPixelColor(pixel_Back, strip.Color(brightness, brightness, brightness, brightness));
    // strip.setPixelColor(pixel_Front_Top, strip.Color(brightness, brightness, brightness, brightness));
    // strip.setPixelColor(pixel_Front_Middle, strip.Color(brightness, brightness, brightness, brightness));
    // strip.setPixelColor(pixel_Front_Bottom, strip.Color(brightness, brightness, brightness, brightness));
    // strip.show();
    setDeterminedColorsAll(0, 0, 0, brightness);
}

void Flashlight::onButtonBackPressed(const ButtonEvent& event)
{    // Press
    if (event.eventType == ButtonEvent_Released){
        ESP_LOGI(TAG_MAIN, "onButtonBackPressed => calling end() + returning to menu...");
        instance->end();
        MenuManager::instance().returnToMenu();
    } 
}

void Flashlight::begin() {
    ESP_LOGI(TAG_MAIN, "begin() => registering app callbacks...");
    registerButtonCallbacks();
}

void Flashlight::end() {
    ESP_LOGI(TAG_MAIN, "end() => unregistering app callbacks...");
    setColorsOff();
    unregisterButtonCallbacks();
}

void Flashlight::registerButtonCallbacks() {
    // Register callbacks for the buttons
    buttonManager.registerCallback(button_BottomLeftIndex, onButtonBackPressed);
}

void Flashlight::unregisterButtonCallbacks() {
    // Unregister callbacks
    buttonManager.unregisterCallback(button_BottomLeftIndex);
}

// Global functions for the app system
void flashlightBegin() {
    flashlight.begin();
}

void flashlightEnd() {
    flashlight.end();
}

void flashlightUpdate() {
    flashlight.update();
}