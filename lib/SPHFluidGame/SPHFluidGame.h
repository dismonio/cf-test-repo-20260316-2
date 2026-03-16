// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#pragma once

#include <vector>
#include <math.h>
#include <Arduino.h>
#include "DisplayProxy.h" 
#include "ButtonManager.h" // For returning back to menu
#include "MenuManager.h" // For returning back to menu

class SPHFluidGame {
public:
    struct Particle {
        float x, y;
        float vx, vy;
        float density;
        float pressure;
    };

    SPHFluidGame(ButtonManager& btnMgr);

    // Updates the simulation by one step, taking external acceleration (e.g., from an IMU).
    void update();
    void begin(); // AppManager integration
    void end(); // AppManager integration

    // Resets/re-randomizes particles.
    void resetParticles();

    // Adjusts the number of particles rendered
    void setParticleCount(int newCount);

private:
    int currentCount; // For tracking the current number of particles

    // ---- SPH parameters ----
    int   numParticles;       // e.g., 100 or 200 for microcontroller
    float smoothingLength;    // h
    float restDensity;        // ρ0
    float stiffness;          // k
    float viscosity;          // μ
    float gravityX;           // external accel in X
    float gravityY;           // external accel in Y
    float damping;            // boundary damping factor
    float dt;                 // timestep
    float particleRadius;     // e.g. 2.0, how big the particle is on screen

    // -- Surface Tension (cohesion) parameter --
    float cohesionStrength;   // Additional force pulling particles together

    // ---- Display / screen ----
    DisplayProxy& display;
    int screenWidth;
    int screenHeight;

    // ---- Particle container ----
    std::vector<Particle> particles;

    // Core SPH steps
    void computeDensityPressure();
    void computeForces(float ax, float ay);
    void resolveParticleCollisions();
    void integrate();
    void render();

    // AppManager Integration
    ButtonManager& buttonManager;
    static void buttonPressedCallback(const ButtonEvent& event);
    static void onButtonBackPressed(const ButtonEvent& event);
    static SPHFluidGame* instance; // To allow static callbacks
};

extern SPHFluidGame sphFluidGame; // App Manager integration