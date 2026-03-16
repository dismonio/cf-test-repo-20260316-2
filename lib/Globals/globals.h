// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <time.h>
#include "esp_log.h"
#include "HAL.h"

// Global timers
extern unsigned long millis_NOW;
extern unsigned long millisOldHeartbeat;
extern unsigned long millis_HAL_TASK_200MS;
extern unsigned long millis_APP_TASK_200MS;
extern unsigned long millis_HAL_TASK_50MS;
extern unsigned long millis_HAL_TASK_20MS;
extern unsigned long millis_APP_TASK_20MS;
extern unsigned long millis_APP_LASTINTERACTION;
extern int TASK_20MS;
extern int TASK_50MS;
extern int TASK_200MS;
extern int TASK_LASTINTERACT;

extern const char *TAG_MAIN;

// App modes
extern int  appActive;
extern int  appActiveSaved;
extern int  appPreviously;

// Some booleans for states
extern bool reactionGameEnabled;
extern volatile bool buttonPressed;

// Button counters
constexpr int numButtons = 6;
extern int buttonCounter[numButtons];
extern int buttonCounterSaved[numButtons];

// WiFi
extern char wifiAP_SSID[];
extern bool isTryingToConnect;

// Clock
extern bool clockScreenEnabled;
extern struct tm currentTime;

// Audio beep logic
extern bool beepActive;
extern float beepFrequency;



#endif // GLOBALS_H
