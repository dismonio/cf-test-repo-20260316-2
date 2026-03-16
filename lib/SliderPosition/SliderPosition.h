// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef SLIDERPOSITION_H
#define SLIDERPOSITION_H

/*
 * SliderPosition.h
 *
 * This header file declares the functions and variables used for reading and filtering
 * the slider position. The filtering is performed using a Kalman filter and an
 * Exponential Moving Average (EMA) filter to smooth the percentage output.
 *
 * Novice-Level Comment:
 * ---------------------
 * This file tells the compiler about the functions and variables we will use
 * in our slider position code. It ensures that when other parts of the program
 * use these functions, the compiler knows how they are defined.
 */

// Function declarations for filter initialization and update

// Initializes the slider position filters and variables.
void sliderPositionFilterInit();

// Updates the EMA filter for the slider position (optional if needed elsewhere).
void sliderPositionFilterUpdate();

// Function declarations for reading and Kalman filter operations

// Reads the slider position and applies filtering.
void sliderPositionRead(int sliderVoltagePin);

// Initializes the Kalman filter state.
void sliderPositionKalmanFilterInit();

// Utility functions for clamping, integer conversion, and rate limiting

// Clamps a float value between minVal and maxVal and converts it to an integer.
int clampAndConvertToInt(float value, int minVal, int maxVal);

// Applies a rate limit to the currentValue based on the newValue and rateLimit.
void applyRateLimit(float &currentValue, float newValue, float rateLimit);

#endif // SLIDERPOSITION_H