// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef SERIALDISPLAY_H
#define SERIALDISPLAY_H

#include <Arduino.h>

// Scroll mode
enum ScrollMode {
    LINE_SCROLL,
    PIXEL_SCROLL
};
extern ScrollMode currentScrollMode;

// Serial data / text scrolling
extern bool newDataReceived;
extern bool showingDefaultScreen;
extern int  lineCount;
extern int  currentLine;
extern bool isScreenUpdated;
extern int  previousLine;
extern int  scrollOffset;
extern int  previousScrollOffset;
extern String dataLines[];
extern String incomingData;
extern unsigned long lastDataTime;
extern const unsigned long dataTimeout;

extern const int maxLinesOnScreen;
extern const int lineHeight;
extern const int displayHeight;
extern int maxScrollOffset;

void serialDataScreenProcessor();
void handleScrollUp();
void handleScrollDown();
void toggleScrollMode();
void updateScrollPositionFromSlider();
void drawSerialDataScreen();
void drawDefaultInfoScreen();

#endif
