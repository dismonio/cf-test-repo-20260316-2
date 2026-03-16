// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "SerialDisplay.h"
#include "globals.h"
#include "HAL.h"           // For HAL::displayProxy()

// Scroll mode
ScrollMode currentScrollMode = PIXEL_SCROLL;

// Serial data / text scrolling
String incomingData = ""; // Buffer for incoming data
bool newDataReceived   = false;
bool showingDefaultScreen = true;
int  lineCount         = 0;
int  currentLine       = 0;
bool isScreenUpdated   = false;
int  previousLine      = 0;
int  scrollOffset      = 16;
int  previousScrollOffset = 0;
String dataLines[10];
unsigned long lastDataTime = 0; // Stores the last time data was received
const unsigned long dataTimeout = 5000; // Timeout period in milliseconds (e.g., 5 seconds)

const int maxLinesOnScreen = 6;       // Number of lines that fit on the screen at once
const int lineHeight = 13;            // Height of each line in pixels 13px = AerialMT_Plain_10
const int displayHeight = 64;         // Height of your display in pixel
int maxScrollOffset =  0;             // Maximum pixel offset for scrolling


void serialDataScreenProcessor() {
    newDataReceived = false;

    while (Serial.available() > 0) {
        char incomingChar = Serial.read();
        newDataReceived = true;

        if (incomingChar == '\n') {
            String parsedData[10];
            int parsedLineCount = 0;
            int startIndex = 0;
            int commaIndex;
            while ((commaIndex = incomingData.indexOf(",", startIndex)) != -1 && parsedLineCount < 10) {
                String segment = incomingData.substring(startIndex, commaIndex);
                segment.trim();
                parsedData[parsedLineCount++] = segment;
                startIndex = commaIndex + 1;
            }
            String lastSegment = incomingData.substring(startIndex);
            lastSegment.trim();
            parsedData[parsedLineCount++] = lastSegment; 

            // Preserve current scroll offset before updating
            int savedOffset = scrollOffset;

            // Always update dataLines
            lineCount = parsedLineCount;
            for (int i = 0; i < lineCount; i++) {
                dataLines[i] = parsedData[i];
            }

            // Recalculate maxScrollOffset
            maxScrollOffset = max(0, lineCount * lineHeight - displayHeight);


            // Restore scrollOffset, ensuring it's within new bounds
            if (savedOffset <= maxScrollOffset) {
                scrollOffset = savedOffset;
            } else {
                scrollOffset = maxScrollOffset;
            }

            // Instead of resetting currentLine and scrollOffset, preserve them:
            //currentLine = 0;
            //scrollOffset = 0;

            incomingData = "";
            lastDataTime = millis();
        } else {
            // Append non-newline characters to incomingData
            incomingData += incomingChar;
        }
    }

    if (millis() - lastDataTime > dataTimeout && !newDataReceived && lineCount == 0) {
        drawDefaultInfoScreen();
        isScreenUpdated = true;
    }
}

void handleScrollUp() { // Intended to handle button presses
  if (currentScrollMode == LINE_SCROLL) {
    // Line-based scrolling
    if (currentLine > 0) {
      currentLine--;
    }
  } else if (currentScrollMode == PIXEL_SCROLL) {
    // Pixel-based scrolling
    if (scrollOffset > 0) {
        scrollOffset--;
    }
  }
}

void handleScrollDown() { // Intended to handle button presses
    if (currentScrollMode == LINE_SCROLL) {
        // Line-based scrolling
        if (currentLine < lineCount - maxLinesOnScreen) {
            currentLine++;
        }
    } else if (currentScrollMode == PIXEL_SCROLL) {
        // Pixel-based scrolling
        if (scrollOffset < maxScrollOffset) {
            scrollOffset++;
        }
    }
}

void toggleScrollMode() {
    if (currentScrollMode == LINE_SCROLL) {
        currentScrollMode = PIXEL_SCROLL;
    } else {
        currentScrollMode = LINE_SCROLL;
        scrollOffset = 0; // Reset pixel offset when switching to line scroll mode
    }
}

void updateScrollPositionFromSlider() {
    if (currentScrollMode == LINE_SCROLL) {
        int newLine = map(sliderPosition_12Bits, 0, 4095, 
                          0, max(lineCount - maxLinesOnScreen, 0));
        if (newLine != previousLine) {
            currentLine = newLine;
            previousLine = newLine;
        }
    } else if (currentScrollMode == PIXEL_SCROLL) {
        int totalScrollablePixels = max(0, (lineCount * lineHeight) - displayHeight);
        int newScrollOffset = map(sliderPosition_12Bits, 0, 4095, 0, totalScrollablePixels);

        if (newScrollOffset != scrollOffset) {
            scrollOffset = newScrollOffset;
            drawSerialDataScreen();
            isScreenUpdated = true;
            previousScrollOffset = newScrollOffset; // Update previousScrollOffset if tracking changes
        }
    }
}

void drawSerialDataScreen() {
    DisplayProxy& display = HAL::displayProxy();
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);

    updateScrollPositionFromSlider();
    serialDataScreenProcessor();

    if (currentScrollMode == LINE_SCROLL) {
        // Line-based scrolling: Display lines based on currentLine
        for (int i = 0; i < maxLinesOnScreen && i + currentLine < lineCount; i++) {
            display.drawString(0, i * 10, dataLines[currentLine + i]);
        }
    } else if (currentScrollMode == PIXEL_SCROLL) {
        // Pixel-based scrolling: Apply pixel offset across all lines
        for (int i = 0; i < lineCount; i++) {
            int yPos = i * lineHeight - scrollOffset;
            // Render lines within the visible area:
            if (yPos >= -lineHeight && yPos <= displayHeight) {
                display.drawString(0, yPos, dataLines[i]);
            }
        }
    }

    display.display();
    isScreenUpdated = true;
}

void drawDefaultInfoScreen() {
    DisplayProxy& display = HAL::displayProxy();
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10); // Use an appropriate font size
    display.drawString(64, 12, "Serial Data Display");
    display.drawString(64, 22, "Waiting for Data Stream...");
    display.display();
  }