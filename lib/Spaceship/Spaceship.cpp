// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "Spaceship.h"
#include "globals.h"
#include "HAL.h"
#include "MenuManager.h"
#include "RGBController.h"

Spaceship spaceshipApp(HAL::buttonManager());
Spaceship* Spaceship::instance = nullptr;

Spaceship::Spaceship(ButtonManager& btnMgr) :
    buttonManager(btnMgr),
    display(HAL::displayProxy()),
    overlayMode(Overlay_None),
    speed(6.0f),
    speedTarget(6.0f),
    cruiseSpeed(6.0f),
    yaw(0.0f),
    yawRate(0.0f),
    yawRateTarget(0.0f),
    turnLeftHeld(false),
    turnRightHeld(false),
    lastMs(0),
    lastShootingMs(0) {
    instance = this;
}

float Spaceship::clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void Spaceship::begin() {
    registerButtonCallbacks();

    lastMs = millis();
    lastShootingMs = millis();

    // Seed stars uniformly across the full depth range
    for (int i = 0; i < kStarCount; ++i) {
        resetStar(i, false, true);
    }

    setColorsOff();
}

void Spaceship::end() {
    setColorsOff();
    unregisterButtonCallbacks();
}

void Spaceship::registerButtonCallbacks() {
    buttonManager.registerCallback(button_BottomLeftIndex, onButtonBackPressed);
    buttonManager.registerCallback(button_LeftIndex, onButtonLeft);
    buttonManager.registerCallback(button_RightIndex, onButtonRight);
    buttonManager.registerCallback(button_UpIndex, onButtonUp);
    buttonManager.registerCallback(button_DownIndex, onButtonDown);
}

void Spaceship::unregisterButtonCallbacks() {
    buttonManager.unregisterCallback(button_BottomLeftIndex);
    buttonManager.unregisterCallback(button_LeftIndex);
    buttonManager.unregisterCallback(button_RightIndex);
    buttonManager.unregisterCallback(button_UpIndex);
    buttonManager.unregisterCallback(button_DownIndex);
}

void Spaceship::onButtonBackPressed(const ButtonEvent& event) {
    if (event.eventType == ButtonEvent_Released) {
        instance->end();
        MenuManager::instance().returnToMenu();
    }
}

void Spaceship::onButtonLeft(const ButtonEvent& event) {
    if (event.eventType == ButtonEvent_Pressed) {
        instance->turnLeftHeld = true;
    } else if (event.eventType == ButtonEvent_Released) {
        instance->turnLeftHeld = false;
    }
}

void Spaceship::onButtonRight(const ButtonEvent& event) {
    if (event.eventType == ButtonEvent_Pressed) {
        instance->turnRightHeld = true;
    } else if (event.eventType == ButtonEvent_Released) {
        instance->turnRightHeld = false;
    }
}

void Spaceship::onButtonUp(const ButtonEvent& event) {
    if (event.eventType != ButtonEvent_Released) return;
    int m = (int)instance->overlayMode - 1;
    if (m < 0) m = (int)Overlay_YawRate;
    instance->overlayMode = (OverlayMode)m;
}

void Spaceship::onButtonDown(const ButtonEvent& event) {
    if (event.eventType != ButtonEvent_Released) return;
    int m = (int)instance->overlayMode + 1;
    if (m > (int)Overlay_YawRate) m = (int)Overlay_None;
    instance->overlayMode = (OverlayMode)m;
}

// resetStar: place a star at the far end of the tunnel.
// The key insight for full-screen coverage: we need to distribute stars
// across the SCREEN-SPACE rectangle, not just a narrow world-space band.
// So we pick a random screen position and reverse-project to get world x,y.
void Spaceship::resetStar(int i, bool forceShooting, bool randomDepth) {
    // Pick a depth
    float z;
    if (randomDepth) {
        // Uniform distribution across full depth range
        long rz = random(200L, 5000L);
        z = (float)rz / 1000.0f;
    } else {
        // Respawn at far end
        long rz = random(3500L, 5000L);
        z = (float)rz / 1000.0f;
    }

    // Pick a random screen-space target position covering the FULL display
    // and reverse-project to world coordinates at this depth.
    // Screen goes 0..127 x, 0..63 y. VP is at (64, 32).
    // Forward projection: sx = vpX + x * xScale / z, sy = vpY + y * yScale / z
    // Reverse: x = (sx - vpX) * z / xScale, y = (sy - vpY) * z / yScale
    //
    // We sample screen positions with margin beyond edges so stars stream
    // in from outside the visible area too.
    long rsx = random(-20L, 148L);   // wider than 0..127
    long rsy = random(-10L, 74L);    // wider than 0..63

    const float xScale = 48.0f;
    const float yScale = 48.0f;

    stars[i].x = ((float)rsx - 64.0f) * z / xScale;
    stars[i].y = ((float)rsy - 32.0f) * z / yScale;
    stars[i].z = z;

    long rv = random(-20L, 21L);
    stars[i].vx = (float)rv / 1000.0f;

    stars[i].kind = 0;
    stars[i].life = 0.0f;

    if (forceShooting) {
        stars[i].kind = 1;
        stars[i].life = 1.0f;
        long rvs = random(-180L, 181L);
        stars[i].vx = (float)rvs / 1000.0f;
        // Shooting stars start closer
        long rzs = random(1200L, 2800L);
        stars[i].z = (float)rzs / 1000.0f;
        // Re-project for new depth
        stars[i].x = ((float)rsx - 64.0f) * stars[i].z / xScale;
        stars[i].y = ((float)rsy - 32.0f) * stars[i].z / yScale;
    }
}

void Spaceship::update() {
    unsigned long now = millis();
    float dt = (float)(now - lastMs) / 1000.0f;
    lastMs = now;

    if (dt < 0.0f) dt = 0.0f;
    if (dt > 0.05f) dt = 0.05f;

    updatePhysics(dt);

    display.clear();
    drawScene();
    drawOverlay();
    display.display();
}

void Spaceship::updatePhysics(float dt) {
    // --- Steering: left/right buttons drive yaw ---
    const float maxYawRate = 90.0f * 3.1415926f / 180.0f;

    if (turnLeftHeld && !turnRightHeld) {
        yawRateTarget = -maxYawRate;
    } else if (turnRightHeld && !turnLeftHeld) {
        yawRateTarget = maxYawRate;
    } else {
        yawRateTarget = 0.0f;
    }

    const float yawRateFollow = 8.0f;
    yawRate += (yawRateTarget - yawRate) * (1.0f - expf(-yawRateFollow * dt));

    const float yawLimit = 60.0f * 3.1415926f / 180.0f;
    yaw += yawRate * dt;
    yaw = clampf(yaw, -yawLimit, yawLimit);

    if (yaw >= yawLimit && yawRate > 0.0f) yawRate = 0.0f;
    if (yaw <= -yawLimit && yawRate < 0.0f) yawRate = 0.0f;

    // Spring yaw back to center when not turning
    if (!turnLeftHeld && !turnRightHeld) {
        yaw += (-yaw) * (1.0f - expf(-2.5f * dt));
    }

    // --- Throttle: slider controls speed ---
    const float speedMin = 1.0f;
    const float speedMax = 10.0f;

    float sliderNorm = (float)sliderPosition_Percentage_Inverted_Filtered / 100.0f;
    speedTarget = speedMin + sliderNorm * (speedMax - speedMin);

    const float speedFollow = 3.0f;
    speed += (speedTarget - speed) * (1.0f - expf(-speedFollow * dt));
    speed = clampf(speed, speedMin, speedMax);

    // --- Shooting stars ---
    unsigned long now = millis();
    if (now - lastShootingMs > 1200UL) {
        lastShootingMs = now;
        long chance = random(0L, 3L);
        if (chance == 0) {
            int idx = (int)random(0L, (long)kStarCount);
            resetStar(idx, true, false);
        }
    }

    // --- Move stars ---
    float yawPush = yaw * 0.55f;
    float yawRatePush = yawRate * 0.15f;

    for (int i = 0; i < kStarCount; ++i) {
        // Forward motion â stars come towards us
        float dz = speed * dt * 0.55f;
        if (stars[i].kind == 1) dz *= 1.8f;
        stars[i].z -= dz;

        // Parallax factor: closer stars shift faster laterally
        float par = 1.0f / clampf(stars[i].z, 0.25f, 6.0f);

        // Lateral motion from turning
        stars[i].x += (stars[i].vx + yawRatePush) * dt * (0.9f + 3.0f * par);
        stars[i].x += yawPush * dt * 0.3f;

        // Shooting star life decay
        if (stars[i].kind == 1) {
            stars[i].life -= dt * 0.8f;
            if (stars[i].life < 0.0f) stars[i].life = 0.0f;
        }

        // Recycle stars that passed through or drifted too far off
        if (stars[i].z < 0.15f) {
            resetStar(i, false, false);
        }
    }

    // --- LEDs: subtle engine glow based on speed ---
    uint8_t glow = (uint8_t)clampf((speed - speedMin) / (speedMax - speedMin) * 8.0f, 0.0f, 8.0f);
    HAL::setRgbLed(pixel_Front_Top, 0, glow, 0, 0);
    HAL::setRgbLed(pixel_Front_Middle, 0, 0, glow, 0);
}

void Spaceship::drawScene() {
    const int16_t hudH = (overlayMode == Overlay_None) ? 0 : 12;

    // Vanishing point at screen center
    const int16_t vpX = 64;
    const int16_t vpY = 32;

    // Projection scale â these control the FOV.
    // Lower values = wider FOV = stars spread more across the screen.
    // With reverse-projection in resetStar using matching values,
    // stars are guaranteed to spawn across the full display area.
    const float xScale = 48.0f;
    const float yScale = 48.0f;

    for (int i = 0; i < kStarCount; ++i) {
        float z = clampf(stars[i].z, 0.20f, 6.0f);
        float inv = 1.0f / z;

        // Project world -> screen
        int16_t sx = (int16_t)(vpX + stars[i].x * xScale * inv);
        int16_t sy = (int16_t)(vpY + stars[i].y * yScale * inv);

        // Generous off-screen cull (allow streaks to start just outside)
        if (sx < -30 || sx > 157 || sy < -30 || sy > 93) continue;

        if (stars[i].kind == 0) {
            // --- Normal stars ---
            if (inv > 1.0f) {
                // Close star: draw a streak radiating from vanishing point
                int16_t dx = sx - vpX;
                int16_t dy = sy - vpY;

                int adx = dx < 0 ? -dx : dx;
                int ady = dy < 0 ? -dy : dy;
                int denom = adx + ady;
                if (denom < 1) denom = 1;

                // Streak length scales with closeness
                float lenF = 2.0f + 14.0f * (inv - 1.0f);
                int16_t len = (int16_t)clampf(lenF, 2.0f, 18.0f);

                int16_t ex = sx + (int16_t)((dx * len) / denom);
                int16_t ey = sy + (int16_t)((dy * len) / denom);

                // Clip to HUD
                if (hudH > 0 && sy < hudH && ey < hudH) continue;

                display.drawLine(sx, sy, ex, ey);
            } else if (inv > 0.6f) {
                // Medium distance: 2x2 dot
                if (hudH > 0 && sy < hudH) continue;
                display.fillRect(sx, sy, 2, 2);
            } else {
                // Far away: single pixel
                if (hudH > 0 && sy < hudH) continue;
                if (sx >= 0 && sx <= 127 && sy >= 0 && sy <= 63) {
                    display.setPixel(sx, sy);
                }
            }
        } else {
            // --- Shooting star ---
            if (hudH > 0 && sy < hudH) continue;

            float lenF = (6.0f + 16.0f * inv) * clampf(stars[i].life, 0.2f, 1.0f);
            int16_t len = (int16_t)clampf(lenF, 4.0f, 24.0f);

            int16_t dx = sx - vpX;
            int16_t dy = sy - vpY;

            int adx = dx < 0 ? -dx : dx;
            int ady = dy < 0 ? -dy : dy;
            int denom = adx + ady;
            if (denom < 1) denom = 1;

            int16_t ex = sx + (int16_t)((dx * len) / denom);
            int16_t ey = sy + (int16_t)((dy * len) / denom);

            if (hudH > 0 && ey < hudH) ey = hudH;

            display.drawLine(sx, sy, ex, ey);
            display.setPixel(sx, sy);
        }
    }

    // Draw the ship at bottom center
    drawWireShip(64, 52, yaw);
}

void Spaceship::drawWireShip(int16_t cx, int16_t cy, float yawRadians) {
    float y = clampf(yawRadians, -1.2f, 1.2f);

    int16_t skew = (int16_t)(y * 10.0f);
    int16_t noseShift = (int16_t)(y * 5.0f);

    int16_t noseX = cx + noseShift;
    int16_t noseY = cy - 12;

    int16_t leftWingX = cx - 18 + skew;
    int16_t leftWingY = cy + 6;

    int16_t rightWingX = cx + 18 + skew;
    int16_t rightWingY = cy + 6;

    int16_t tailX = cx + skew;
    int16_t tailY = cy + 10;

    // Outline
    display.drawLine(noseX, noseY, leftWingX, leftWingY);
    display.drawLine(noseX, noseY, rightWingX, rightWingY);
    display.drawLine(leftWingX, leftWingY, tailX, tailY);
    display.drawLine(rightWingX, rightWingY, tailX, tailY);

    // Center spine
    display.drawLine(noseX, noseY, tailX, tailY);
}

void Spaceship::drawOverlay() {
    if (overlayMode == Overlay_None) return;

    // Black-fill the HUD area so stars behind it are occluded
    display.setColor(BLACK);
    display.fillRect(0, 0, 128, 12);
    display.setColor(WHITE);

    // HUD border
    display.drawRect(0, 0, 128, 12);

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);

    if (overlayMode == Overlay_Speed) {
        int sp = (int)(speed * 10.0f + 0.5f);
        display.drawString(3, 0, "SPD: " + String(sp / 10) + "." + String(sp % 10));
    } else if (overlayMode == Overlay_YawRate) {
        float yrd = yawRate * 180.0f / 3.1415926f;
        int val = (int)(yrd >= 0.0f ? (yrd + 0.5f) : (yrd - 0.5f));
        display.drawString(3, 0, "YAW d/s: " + String(val));
    }
}