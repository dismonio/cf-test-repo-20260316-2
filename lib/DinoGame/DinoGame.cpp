// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "DinoGame.h"
#include "HAL.h"
#include "MenuManager.h"

DinoGame dinoGame(HAL::buttonManager());

DinoGame* DinoGame::instance = nullptr;

DinoGame::DinoGame(ButtonManager& btnMgr)
  : display(HAL::displayProxy()),
    buttonManager(btnMgr),
    gameOver(false),
    dinoY(0),
    dinoVelocity(0),
    isJumping(false),
    isDucking(false),
    obstacleCount(0),
    obstaclesAvoidedCount(0),
    nextSpawnTime(0),
    obstacleSpeed(1.5f),
    minGapTime(800),    // minimal 0.8s between spawns
    maxGapTime(1800),   // up to 1.8s
    pterodactylUnlocked(false),
    pterodactylUnlockAt(5),
    groundScrollSpeed(1.0f)
{
    for(int i=0; i<MAX_OBSTACLES; i++){
        obstacles[i].x = -100;
        obstacles[i].isHigh = false;
        obstacles[i].passed = false;
    }

    initGround();

    DinoGame::instance = this;
}

//---------------------------------------------------------------------------------
// Reset
//---------------------------------------------------------------------------------
void DinoGame::resetGame() {
    gameOver = false;
    dinoY = 0;
    dinoVelocity = 0;
    isJumping = false;
    isDucking = false;

    obstacleCount = 0;
    obstaclesAvoidedCount = 0;
    for(int i=0; i<MAX_OBSTACLES; i++){
        obstacles[i].x = -100;
        obstacles[i].isHigh = false;
        obstacles[i].passed = false;
    }

    // Speed
    obstacleSpeed = 1.5f;
    groundScrollSpeed = 1.0f;

    // Next spawn
    unsigned long now = millis();
    nextSpawnTime = now + random(minGapTime, maxGapTime);

    // Pterodactyl unlock logic
    int extra = random(0,4); // 0..3
    pterodactylUnlockAt = 5 + extra;
    pterodactylUnlocked = false;

    initGround();
}

//---------------------------------------------------------------------------------
// Ground initialization
//---------------------------------------------------------------------------------
void DinoGame::initGround() {
    // Fill groundSegs so they tile across [0..128]
    float xPos = 0;
    for(int i=0; i<GROUND_SEGMENTS; i++){
        groundSegs[i].x = xPos;
        groundSegs[i].tileIndex = random(0, NUM_GROUND_TILES);
        xPos += 16.0f; // each tile is 16 wide
    }
}

//---------------------------------------------------------------------------------
// Update
//---------------------------------------------------------------------------------
void DinoGame::update() {

    if(gameOver) return;

    setSpeedBySlider(sliderPosition_Percentage_Inverted_Filtered);
    updateDino();
    updateGround();
    updateObstacles();
    checkCollisions();
    draw();
}

//---------------------------------------------------------------------------------
// Draw
//---------------------------------------------------------------------------------
void DinoGame::draw() {
    display.clear();

    // 1) Draw ground first
    drawGround();

    // 2) Dino
    int dinoX = 10;
    int groundY = 48;  // bottom line for Dino
    int dinoScreenY = groundY - (int)dinoY;

    if(!isDucking){
        // standing => 16×16
        display.drawXbm(dinoX, dinoScreenY-16, 16, 16, Dino_Stand_16x16);
    } else {
        // ducking => 16×8
        display.drawXbm(dinoX, dinoScreenY-8, 16, 8, Dino_Duck_16x8);
    }

    // 3) Obstacles
    //    pterodactyl => y=24 (HIGH)
    //    cactus => y=32
    for(int i=0; i<obstacleCount; i++){
        int ox = (int)obstacles[i].x;
        if(obstacles[i].isHigh){
            // must duck => higher, so you can't jump over
            display.drawXbm(ox, 20, 16, 16, Pterodactyl_16x16);
        } else {
            display.drawXbm(ox, 32, 8, 16, Cactus_8x16);
        }
    }

    // 4) Score or GameOver
    if(gameOver){
        display.drawString(35, 0, "GAME OVER!");
        display.drawString(30, 16, "Score: " + String(obstaclesAvoidedCount));
    } else {
        display.drawString(0, 0, "Score: " + String(obstaclesAvoidedCount));
    }

    display.display();
}

//---------------------------------------------------------------------------------
// Set speed via slider
//---------------------------------------------------------------------------------
void DinoGame::setSpeedBySlider(float sliderPercentage) {
    // Example: 1.0..5.0
    obstacleSpeed = 1.0f + (sliderPercentage/100.0f)*4.0f;
    // ground can scroll slightly slower for parallax
    groundScrollSpeed = 0.8f + (sliderPercentage/100.0f)*3.2f;
}

//---------------------------------------------------------------------------------
// Static button callbacks
//---------------------------------------------------------------------------------
void DinoGame::jumpButtonCallback(const ButtonEvent &ev) {
    if(ev.eventType == ButtonEvent_Pressed && instance) {
        instance->handleJump();
    }
}

void DinoGame::duckButtonCallback(const ButtonEvent &ev) {
    if(instance){
        instance->handleDuck(ev.eventType);
    }
}

void DinoGame::resetButtonCallback(const ButtonEvent &ev) {
    if(ev.eventType == ButtonEvent_Pressed && instance){
        instance->handleReset();
    }
}

void DinoGame::endButtonCallback(const ButtonEvent &ev) {
    if(ev.eventType == ButtonEvent_Released && instance){
        ESP_LOGI(TAG_MAIN, "onButtonBackPressed => calling end() + returning to menu...");
        instance->end();
        MenuManager::instance().returnToMenu();
    }
}

//---------------------------------------------------------------------------------
// Private event handlers
//---------------------------------------------------------------------------------
void DinoGame::handleJump() {
    if(!isJumping && !isDucking && dinoY == 0){
        isJumping = true;
        dinoVelocity = 5.0f;  // jump velocity
    }
}

void DinoGame::handleDuck(ButtonEventType evType) {
    if(evType == ButtonEvent_Pressed){
        if(!isJumping){
            isDucking = true;
        }
    } else if(evType == ButtonEvent_Released){
        isDucking = false;
    }
}

void DinoGame::handleReset() {
    resetGame();
}

//---------------------------------------------------------------------------------
// Dino Physics
//---------------------------------------------------------------------------------
void DinoGame::updateDino() {
    if(isJumping){
        dinoY += dinoVelocity;
        dinoVelocity -= 0.4f; // gravity
        if(dinoY < 0){
            dinoY = 0;
            isJumping = false;
            dinoVelocity = 0;
        }
    }
}

//---------------------------------------------------------------------------------
// Ground
//---------------------------------------------------------------------------------
void DinoGame::updateGround() {
    // Scroll each segment left by groundScrollSpeed
    // If a segment goes off the left (x < -16), reposition it to the far right
    // with a new random tile, continuing the "endless" effect.
    float farRight = 0;
    for(int i=0; i<GROUND_SEGMENTS; i++){
        groundSegs[i].x -= groundScrollSpeed;
        if(groundSegs[i].x < -16){
            // find the current farthest right
            for(int j=0; j<GROUND_SEGMENTS; j++){
                if(groundSegs[j].x > farRight) {
                    farRight = groundSegs[j].x;
                }
            }
            // place this segment just beyond the farRight
            groundSegs[i].x = farRight + 16;
            groundSegs[i].tileIndex = random(0, NUM_GROUND_TILES);
        }
    }
}

void DinoGame::drawGround() {
    // Each segment is 16 wide, 8 high, drawn at y=56
    for(int i=0; i<GROUND_SEGMENTS; i++){
        int sx = (int)groundSegs[i].x;
        int tileIdx = groundSegs[i].tileIndex;
        // read pointer from array in PROGMEM
        const unsigned char* tilePtr = GroundTiles[tileIdx];
        display.drawXbm(sx, 56, 16, 8, tilePtr);
    }
}

//---------------------------------------------------------------------------------
// Obstacles
//---------------------------------------------------------------------------------
void DinoGame::updateObstacles() {
    unsigned long now = millis();
    // If it's time to spawn the next obstacle
    if(now >= nextSpawnTime) {
        spawnObstacle();
        // pick new spawn time
        nextSpawnTime = now + random(minGapTime, maxGapTime);
    }

    int dinoX = 10;
    // Move obstacles, track passing
    for(int i=0; i<obstacleCount; i++){
        obstacles[i].x -= obstacleSpeed;
        if(!obstacles[i].passed && obstacles[i].x < dinoX){
            obstacles[i].passed = true;
            obstaclesAvoidedCount++;
            if(!pterodactylUnlocked && obstaclesAvoidedCount >= pterodactylUnlockAt){
                pterodactylUnlocked = true;
            }
        }
    }

    // Remove offscreen
    int newCount=0;
    for(int i=0; i<obstacleCount; i++){
        if(obstacles[i].x > -20){
            obstacles[newCount++] = obstacles[i];
        }
    }
    obstacleCount = newCount;
}

void DinoGame::spawnObstacle() {
    if(obstacleCount < MAX_OBSTACLES) {
        Obstacle obs;
        obs.x = 128;     // start at the right edge
        obs.passed = false;
        // If not unlocked => always cactus
        // else random
        if(!pterodactylUnlocked){
            obs.isHigh = false; 
        } else {
            obs.isHigh = (random(0,2) == 1);
        }

        obstacles[obstacleCount++] = obs;
    }
}

//---------------------------------------------------------------------------------
// Pixel-perfect collisions
//---------------------------------------------------------------------------------
void DinoGame::checkCollisions() {
    if(obstacleCount == 0) return;

    int dinoX = 10;
    // Dino’s top Y on screen
    int groundY = 48;
    int dinoScreenY = groundY - (int)dinoY;

    // pick dino sprite
    const unsigned char* dinoSprite;
    int dinoW, dinoH, dinoTopY;
    if(!isDucking){
        dinoSprite = Dino_Stand_16x16;
        dinoW=16; dinoH=16;
        dinoTopY = dinoScreenY-16;
    } else {
        dinoSprite = Dino_Duck_16x8;
        dinoW=16; dinoH=8;
        dinoTopY = dinoScreenY-8;
    }

    // check each obstacle
    for(int i=0; i<obstacleCount; i++){
        int ox = (int)obstacles[i].x;
        if(obstacles[i].isHigh){
            // pterodactyl => must duck => y=16
            if(pixelCollides(dinoSprite, dinoW, dinoH, dinoX, dinoTopY,
                             Pterodactyl_16x16, 16,16, ox,20))
            {
                gameOver=true;
                return;
            }
        } else {
            // cactus => y=32
            if(pixelCollides(dinoSprite, dinoW, dinoH, dinoX, dinoTopY,
                             Cactus_8x16, 8,16, ox,32))
            {
                gameOver=true;
                return;
            }
        }
    }
}

// same pixelCollides(...) as before
bool DinoGame::pixelCollides(
    const unsigned char* spriteA, int wA,int hA,int xA,int yA,
    const unsigned char* spriteB, int wB,int hB,int xB,int yB)
{
    // bounding box check
    int Aleft=xA, Aright=xA+wA-1, Atop=yA, Abottom=yA+hA-1;
    int Bleft=xB, Bright=xB+wB-1, Btop=yB, Bbottom=yB+hB-1;

    if(Aright < Bleft || Aleft > Bright || Abottom < Btop || Atop > Bbottom){
        return false;
    }

    int overlapLeft   = max(Aleft, Bleft);
    int overlapRight  = min(Aright, Bright);
    int overlapTop    = max(Atop, Btop);
    int overlapBottom = min(Abottom, Bbottom);

    for(int py=overlapTop; py<=overlapBottom; py++){
        for(int px=overlapLeft; px<=overlapRight; px++){
            // sprite A local coords
            int aCol = px - Aleft;
            int aRow = py - Atop;
            int bytesPerRowA=(wA+7)/8;
            int byteIndexA=aRow*bytesPerRowA+(aCol/8);
            int bitIndexA=7-(aCol%8);
            unsigned char dataA = pgm_read_byte(&spriteA[byteIndexA]);
            bool pixelA = (dataA >> bitIndexA) & 1;

            // sprite B local coords
            int bCol = px - Bleft;
            int bRow = py - Btop;
            int bytesPerRowB=(wB+7)/8;
            int byteIndexB=bRow*bytesPerRowB+(bCol/8);
            int bitIndexB=7-(bCol%8);
            unsigned char dataB = pgm_read_byte(&spriteB[byteIndexB]);
            bool pixelB = (dataB >> bitIndexB) & 1;

            if(pixelA && pixelB){
                return true;
            }
        }
    }
    return false;
}


//---------------------------------------------------------------------------------
// Register button callbacks
//---------------------------------------------------------------------------------
void DinoGame::registerButtonCallbacks() {
    buttonManager.registerCallback(button_MiddleLeftIndex, jumpButtonCallback);
    buttonManager.registerCallback(button_MiddleRightIndex, duckButtonCallback);
    buttonManager.registerCallback(button_BottomRightIndex, resetButtonCallback);

    // Exit App
    buttonManager.registerCallback(button_BottomLeftIndex, endButtonCallback);
}

//---------------------------------------------------------------------------------
// Unregister button callbacks
//---------------------------------------------------------------------------------
void DinoGame::unregisterButtonCallbacks() {
    // Unregister callbacks
    buttonManager.unregisterCallback(button_MiddleLeftIndex);
    buttonManager.unregisterCallback(button_MiddleRightIndex);
    buttonManager.unregisterCallback(button_BottomRightIndex);
    
    buttonManager.unregisterCallback(button_BottomLeftIndex);
}

//---------------------------------------------------------------------------------
// Initialize or begin the app
//---------------------------------------------------------------------------------
void DinoGame::begin() {
    ESP_LOGI(TAG_MAIN, "begin() => registering callbacks...");
    registerButtonCallbacks();
    resetGame();
}

//---------------------------------------------------------------------------------
// End the app
//---------------------------------------------------------------------------------
void DinoGame::end() {
    ESP_LOGI(TAG_MAIN, "end() => unregistering callbacks...");
    unregisterButtonCallbacks();
}