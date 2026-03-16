// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef EXAMPLESCREENS_H
#define EXAMPLESCREENS_H

#include "ButtonManager.h"
#include "MenuManager.h"

// --------------------------------------------------------------------
// Original draw function prototypes
// (make sure the function declarations match exactly what’s in the .cpp)
// --------------------------------------------------------------------
void drawFontFaceDemo();
void drawTextFlowDemo();
void drawTextAlignmentDemo();
void drawRectDemo();
void drawCircleDemo();
void drawImageDemo_1();
void drawImageDemo_2();
void drawImageDemo_3();
void drawImageDemo_4();
void drawBatteryProgressBar();
void drawSliderProgressBar();
void drawAccelerometerScreen();
void drawButtonCounters();
void drawTimeOnCounter();
void drawProgressBarDemo();

// --------------------------------------------------------------------
// Classes for each Example Screen app
// Each class has begin(), end(), update(), and a static callback
// --------------------------------------------------------------------

// -----------------------------------------------------
// 1) FontFaceApp
// -----------------------------------------------------
class FontFaceApp {
public:
    FontFaceApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static FontFaceApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 2) TextFlowApp
// -----------------------------------------------------
class TextFlowApp {
public:
    TextFlowApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static TextFlowApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 3) TextAlignmentApp
// -----------------------------------------------------
class TextAlignmentApp {
public:
    TextAlignmentApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static TextAlignmentApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 4) RectApp
// -----------------------------------------------------
class RectApp {
public:
    RectApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static RectApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 5) CircleApp
// -----------------------------------------------------
class CircleApp {
public:
    CircleApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static CircleApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 6) ImageDemo1App
// -----------------------------------------------------
class ImageDemo1App {
public:
    ImageDemo1App(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static ImageDemo1App* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 7) ImageDemo2App
// -----------------------------------------------------
class ImageDemo2App {
public:
    ImageDemo2App(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static ImageDemo2App* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 8) ImageDemo3App
// -----------------------------------------------------
class ImageDemo3App {
public:
    ImageDemo3App(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static ImageDemo3App* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 9) ImageDemo4App
// -----------------------------------------------------
class ImageDemo4App {
public:
    ImageDemo4App(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static ImageDemo4App* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 10) BatteryBarApp
// -----------------------------------------------------
class BatteryBarApp {
public:
    BatteryBarApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static BatteryBarApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 11) SliderBarApp
// (Calls strip.setPixelColor so we’ll do setColorsOff() in end())
// -----------------------------------------------------
class SliderBarApp {
public:
    SliderBarApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static SliderBarApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 12) AccelerometerApp
// (Uses setDeterminedColorsFront, so setColorsOff() in end())
// -----------------------------------------------------
class AccelerometerApp {
public:
    AccelerometerApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static AccelerometerApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 13) ButtonCountersApp
// -----------------------------------------------------
class ButtonCountersApp {
public:
    ButtonCountersApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static ButtonCountersApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 14) TimeOnCounterApp
// -----------------------------------------------------
class TimeOnCounterApp {
public:
    TimeOnCounterApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static TimeOnCounterApp* instance;

private:
    ButtonManager& buttonManager;
};

// -----------------------------------------------------
// 15) ProgressBarApp
// -----------------------------------------------------
class ProgressBarApp {
public:
    ProgressBarApp(ButtonManager& btnMgr);
    void begin();
    void end();
    void update();

    static void onButtonBackPressed(const ButtonEvent& event);
    static ProgressBarApp* instance;

private:
    ButtonManager& buttonManager;
};

// --------------------------------------------------------------------
// Extern declarations for each app instance
// --------------------------------------------------------------------
extern FontFaceApp         fontFaceApp;
extern TextFlowApp         textFlowApp;
extern TextAlignmentApp    textAlignmentApp;
extern RectApp             rectApp;
extern CircleApp           circleApp;
extern ImageDemo1App       imageDemo1App;
extern ImageDemo2App       imageDemo2App;
extern ImageDemo3App       imageDemo3App;
extern ImageDemo4App       imageDemo4App;
extern BatteryBarApp       batteryBarApp;
extern SliderBarApp        sliderBarApp;
extern AccelerometerApp    accelerometerApp;
extern ButtonCountersApp   buttonCountersApp;
extern TimeOnCounterApp    timeOnCounterApp;
extern ProgressBarApp      progressBarApp;

#endif // EXAMPLESCREENS_H
