// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef DINOGAME_H
#define DINOGAME_H

#include <Arduino.h>
#include "DisplayProxy.h"
#include "ButtonManager.h"
#include "DinoSprites.h"

/**
 * DinoGame with:
 * - 3 buttons (jump, duck, reset)
 * - Pixel-perfect collisions
 * - Score
 * - Multiple ground tiles for a scrolling “bumpy” ground
 * - Higher pterodactyl Y => must duck
 * - Random obstacle spacing (with min gap) => multiple on screen
 */

class DinoGame {
public:
    DinoGame(ButtonManager& btnMgr);

    void begin();
    void end();

    void update();
    void draw();

    void resetGame();
    void setSpeedBySlider(float sliderPercentage);

    static void jumpButtonCallback(const ButtonEvent &ev);
    static void duckButtonCallback(const ButtonEvent &ev);
    static void resetButtonCallback(const ButtonEvent &ev);
    static void endButtonCallback(const ButtonEvent &ev);

    static DinoGame* instance;

private:
    // Refs
    DisplayProxy& display;
    ButtonManager& buttonManager;

    // Button callbacks
    void registerButtonCallbacks();
    void unregisterButtonCallbacks();

    // Dino
    bool  gameOver;
    float dinoY;
    float dinoVelocity;
    bool  isJumping;
    bool  isDucking;

    // Obstacles
    struct Obstacle {
        float x;
        bool  isHigh;   // pterodactyl (duck needed) or cactus
        bool  passed;   // track if we counted it as avoided
    };
    static const int MAX_OBSTACLES = 8;
    Obstacle obstacles[MAX_OBSTACLES];
    int obstacleCount;

    unsigned long obstaclesAvoidedCount;

    // Obstacle spawn timing
    unsigned long nextSpawnTime;   // when the next obstacle spawns
    float obstacleSpeed;
    unsigned long minGapTime;      // minimal time between spawns
    unsigned long maxGapTime;      // max time between spawns (so we do get obstacles)
    bool   pterodactylUnlocked;    
    int    pterodactylUnlockAt;

    // Ground scrolling
    static const int GROUND_SEGMENTS = 10; // how many 16px wide segments across 128px (plus extras)
    struct GroundSegment {
        float x;          // left position
        int   tileIndex;  // which tile from GroundTiles[]
    };
    GroundSegment groundSegs[GROUND_SEGMENTS];
    float groundScrollSpeed; // typically same or smaller than obstacleSpeed

    void initGround();
    void updateGround();
    void drawGround();

    // Timers/logic
    void handleJump();
    void handleDuck(ButtonEventType evType);
    void handleReset();
    void handleEnd();

    void updateDino();
    void updateObstacles();
    void spawnObstacle();
    void checkCollisions();

    bool pixelCollides(
        const unsigned char* spriteA, int wA, int hA, int xA, int yA,
        const unsigned char* spriteB, int wB, int hB, int xB, int yB
    );
};

// Declare that there's a global object called below somewhere for AppDefs to use
// must be after the reference class definition exists
extern DinoGame dinoGame;

#endif
