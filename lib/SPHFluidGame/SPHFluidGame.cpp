// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "SPHFluidGame.h"
#include "HAL.h"

SPHFluidGame sphFluidGame(HAL::buttonManager()); // App Manager Integration
SPHFluidGame* SPHFluidGame::instance = nullptr; // App Manager Integration

//--------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------
SPHFluidGame::SPHFluidGame(ButtonManager& btnMgr)
    : display(HAL::displayProxy()),
    buttonManager(btnMgr) // AppManager Integration
{
    // Initialize simulation parameters

    /*
    Particle Count
    -------------
    SPH is O(N²). For each update, every particle checks every other particle.
    Keep numParticles small (50–200) if you want a usable frame rate on ESP/Arduino.

    Smoothing Length (smoothingLength)
    ----------------------------------
    Controls the neighborhood size for interactions. If it’s too big, everything 
    interacts too strongly. If it’s too small, you’ll get low density or super 
    local collisions.

    Stiffness (stiffness)
    ---------------------
    Higher values make the fluid act more like an incompressible liquid but 
    can cause instability.

    Viscosity
    ---------
    Smooths out velocity differences. If your fluid is “exploding” or jiggling 
    too much, bump this up.

    Time Step (dt)
    --------------
    Must be small enough to keep the simulation stable. If you see instability or 
    infinite velocities, reduce dt.

    Gravity vs. IMU
    ---------------
    In the example, we just add accelX * 5.0f or so to simulate tilt. You’ll need 
    to tweak that scaling. If you want actual gravity in the real sense, keep 
    gravityY = 9.8f; or set it lower for more subtle fluid.

    Performance
    -----------
    With naive O(N²) loops, 200 particles → 40,000 interactions each frame. That 
    might be near the limit on certain boards. You can optimize with cell grids 
    or neighbor searches if needed, but that’s more complex.

    Debug
    -----
    If you see positions/velocities go to infinity, your parameters (stiffness, 
    dt, smoothing length, etc.) might be out of balance or your IMU feed is 
    returning very large values.
    */
    currentCount     = 100;        // Default to 100 particles
    numParticles     = 100;        // Try small first to keep it real-time on MCU
    smoothingLength  = 5.0f;       // h
    restDensity      = 1.0f;       // ρ0
    stiffness        = 2.0f;       // k
    viscosity        = 0.8f;       // μ
    gravityX         = 0.0f;
    gravityY         = 9.8f;       // Earth-like gravity
    damping          = 0.99f;
    dt               = 0.1f;
    screenWidth      = 128;
    screenHeight     = 64;

    // Surface tension "stickiness" parameter
    cohesionStrength = 0.08f;  // Tweak to achieve desired "stickiness"

    // How big the particles are on screen, 
    particleRadius   = 2.0f; 

    resetParticles();
    instance = this; // AppManager Integration   
}

//--------------------------------------------------------------------------------
// Main update function
//--------------------------------------------------------------------------------
void SPHFluidGame::update()
{
    int targetCount = sliderPosition_Percentage_Inverted_Filtered;
    if (targetCount != currentCount) {
        sphFluidGame.setParticleCount(targetCount);
        currentCount = targetCount;
    }

    // Combine tilt acceleration with gravity
    // Tweak scale if you want weaker/stronger tilt
    float ax = gravityX + accelX * -0.10f;
    float ay = gravityY + accelY * 0.10f;

    // 1) Compute densities & pressures
    computeDensityPressure();

    // 2) Compute all forces (pressure, viscosity, cohesion, external accel)
    computeForces(ax, ay);

    // 3) Integrate positions/velocities
    integrate();

    // 3.5) Resolve collisions to prevent overlap
    resolveParticleCollisions();

    // 4) Render particles
    render();
}

//--------------------------------------------------------------------------------
// Reset particles with random positions
//--------------------------------------------------------------------------------
void SPHFluidGame::resetParticles()
{
    particles.clear();
    particles.reserve(numParticles);

    for (int i = 0; i < numParticles; i++) {
        Particle p;
        p.x        = random(screenWidth);
        p.y        = random(screenHeight);
        p.vx       = 0.0f;
        p.vy       = 0.0f;
        p.density  = restDensity;
        p.pressure = 0.0f;
        particles.push_back(p);
    }
}

//--------------------------------------------------------------------------------
// Adjust the number of rendered particles
//--------------------------------------------------------------------------------
void SPHFluidGame::setParticleCount(int newCount) {
    // Clamp to valid range, just in case
    if (newCount < 1) {
        newCount = 1;
    }
    numParticles = newCount;

    // Now reset (randomize) the particle array
    resetParticles();
}

//--------------------------------------------------------------------------------
// 1) Compute density & pressure
//--------------------------------------------------------------------------------
void SPHFluidGame::computeDensityPressure()
{
    float h2      = smoothingLength * smoothingLength;
    float mass    = 1.0f;  // Uniform mass per particle
    float poly6K  = 315.0f / (64.0f * (float)M_PI * powf(smoothingLength, 9));

    // O(N^2) loop for simplicity
    for (int i = 0; i < numParticles; i++) {
        Particle &pi = particles[i];
        pi.density   = 0.0f;

        for (int j = 0; j < numParticles; j++) {
            Particle &pj = particles[j];

            float dx = pi.x - pj.x;
            float dy = pi.y - pj.y;
            float r2 = dx*dx + dy*dy;

            if (r2 < h2) {
                float t = (h2 - r2);
                pi.density += mass * poly6K * t * t * t;
            }
        }
        // Pressure equation: P = k(ρ - ρ0), clamp to zero min
        pi.pressure = stiffness * (pi.density - restDensity);
        if (pi.pressure < 0.0f) {
            pi.pressure = 0.0f;
        }
    }
}

//--------------------------------------------------------------------------------
// 2) Compute forces (pressure, viscosity, and surface tension / cohesion)
//--------------------------------------------------------------------------------
void SPHFluidGame::computeForces(float ax, float ay)
{
    float h      = smoothingLength;
    float h2     = h * h;
    float mass   = 1.0f;
    // Spiky kernel constant for pressure
    float spikyK = -45.0f / ((float)M_PI * powf(h, 6));
    // Viscosity kernel constant
    float viscoK = 45.0f  / ((float)M_PI * powf(h, 6));

    // Clear accelerations in vx, vy updates
    for (int i = 0; i < numParticles; i++) {
        Particle &pi = particles[i];

        // We'll accumulate forces into (fx, fy), then convert to acceleration
        float fx = 0.0f;
        float fy = 0.0f;

        // Pairwise interactions
        for (int j = 0; j < numParticles; j++) {
            if (i == j) continue;
            Particle &pj = particles[j];

            float dx  = pi.x - pj.x;
            float dy  = pi.y - pj.y;
            float r2  = dx*dx + dy*dy;

            if (r2 < h2 && r2 > 0.000001f) {
                float r    = sqrtf(r2);
                float invR = 1.0f / r;

                // ------------------
                // Pressure force
                // ------------------
                float avgP  = (pi.pressure + pj.pressure) / 2.0f;
                float spiky = spikyK * powf(h - r, 2); // spiky grad magnitude
                float fPres = mass * avgP / pj.density * spiky; 

                fx += fPres * dx * invR;
                fy += fPres * dy * invR;

                // ------------------
                // Viscosity force
                // ------------------
                float vijx   = pj.vx - pi.vx;
                float vijy   = pj.vy - pi.vy;
                float lapVis = viscoK * (h - r);

                fx += viscosity * vijx * lapVis;
                fy += viscosity * vijy * lapVis;

                // ------------------
                // Cohesion / "Surface Tension"
                // ------------------
                // A simple cohesion pull for neighbors:
                float pull = cohesionStrength * (h - r);
                // Negative sign => attraction
                fx -= pull * (dx * invR);
                fy -= pull * (dy * invR);
            }
        }

        // External acceleration: F = m*a ≈ ρ * a
        fx += ax * pi.density;
        fy += ay * pi.density;

        // Convert total force to acceleration and integrate in velocity
        float invDen = 1.0f / pi.density;
        pi.vx += fx * invDen * dt;
        pi.vy += fy * invDen * dt;
    }
}

//--------------------------------------------------------------------------------
// 3) Integrate positions and apply boundary damping
//--------------------------------------------------------------------------------
void SPHFluidGame::integrate()
{
    for (int i = 0; i < numParticles; i++) {
        Particle &p = particles[i];

        // Update positions
        p.x += p.vx * dt;
        p.y += p.vy * dt;

        // Bounce off boundaries
        if (p.x < 0) {
            p.x = 0;
            p.vx *= -damping;
        } else if (p.x >= screenWidth) {
            p.x = screenWidth - 1;
            p.vx *= -damping;
        }

        if (p.y < 0) {
            p.y = 0;
            p.vy *= -damping;
        } else if (p.y >= screenHeight) {
            p.y = screenHeight - 1;
            p.vy *= -damping;
        }
    }
}

//--------------------------------------------------------------------------------
// 3.5) Check for particle collisions
//--------------------------------------------------------------------------------
void SPHFluidGame::resolveParticleCollisions() {
    // Choose a minimum distance that you don’t want particles to violate.
    // For 1-pixel “radius,” you might want minDist = 2.0. 
    // But start with something small like 1.0 or 2.0 and see how it looks.
    // The distance at which particles “collide”
    float minDist = 2.0f * particleRadius;
    float minDistSq = minDist * minDist;

    // Simple O(N^2) loop again
    for (int i = 0; i < numParticles; i++) {
        for (int j = i + 1; j < numParticles; j++) {
            float dx = particles[j].x - particles[i].x;
            float dy = particles[j].y - particles[i].y;
            float distSq = dx * dx + dy * dy;

            if (distSq < minDistSq && distSq > 0.000001f) {
                float dist = sqrtf(distSq);

                // Overlap amount
                float overlap = 0.5f * (minDist - dist);

                // Normalize the vector from i to j
                dx /= dist;
                dy /= dist;

                // Push each particle so they're just at minDist
                particles[i].x -= dx * overlap;
                particles[i].y -= dy * overlap;
                particles[j].x += dx * overlap;
                particles[j].y += dy * overlap;
            }
        }
    }
}

//--------------------------------------------------------------------------------
// 4) Render (plot) particles on the OLED
//--------------------------------------------------------------------------------
void SPHFluidGame::render()
{
    display.clear();

    // If your library requires specifying the color, do it here:
    // display.setColor(WHITE);

    for (int i = 0; i < numParticles; i++) {
        int cx = static_cast<int>(particles[i].x);
        int cy = static_cast<int>(particles[i].y);

        // Draw a filled circle for each particle
        for (int dy = -particleRadius; dy <= particleRadius; dy++) {
            for (int dx = -particleRadius; dx <= particleRadius; dx++) {
                float distSq = dx*dx + dy*dy;
                float rSq = particleRadius * particleRadius;
                if (distSq <= rSq) {
                    int px = cx + dx;
                    int py = cy + dy;
                    if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
                        display.setPixel(px, py);
                    }
                }
            }
        }
    }

    display.display();
}

void SPHFluidGame::begin() {
    // Register button callbacks
    buttonManager.registerCallback(button_BottomLeftIndex, onButtonBackPressed);
}

void SPHFluidGame::end() {
    // Unregister callbacks
    ESP_LOGI(TAG_MAIN, "end() => unregistering button callbacks...");
    buttonManager.unregisterCallback(button_BottomLeftIndex);
}

void SPHFluidGame::onButtonBackPressed(const ButtonEvent& event)
{    // Press
    if (event.eventType == ButtonEvent_Released){
        ESP_LOGI(TAG_MAIN, "onButtonBackPressed => calling end() + returning to menu...");
        instance->end();
        MenuManager::instance().returnToMenu();
    } 
}