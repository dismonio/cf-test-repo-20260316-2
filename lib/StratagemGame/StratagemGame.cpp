// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "StratagemGame.h"
#include "HAL.h"
#include "globals.h" // If needed for button_*Index, but we are now using new button_*Index variables from HAL
#include "StratagemGameSprites.h" // For our 8x8 arrow sprites

// -------------------------------
// Instantiate global instance
// -------------------------------
StratagemGame stratagemGame(HAL::buttonManager(), HAL::audioManager());

// -------------------------------
// Static instance pointer
// -------------------------------
StratagemGame* StratagemGame::instance = nullptr;

// ---------------------------------------------------
// Embed some sample Stratagem data
// (like from HD2-sequences.json) in a static array.
// If it’s large, consider storing in PROGMEM.
// ---------------------------------------------------
const StratagemGame::Stratagem StratagemGame::s_stratagemData[] = {
    { "Reinforce",              "UDRLU",      "reinforce.svg" },
    { "Resupply",               "DDUR",       "resupply.svg" },
    { "SOS Beacon",             "UDRU",       "sos_beacon.svg" },
    { "SEAF Artillery",         "RUUD",       "seaf_artillery.svg" },
    { "Hellbomb",               "DULDURDU",   "hellbomb.svg" },
    { "Prospecting Drill",      "DDLRDD",     "prospecting_drill.svg" },
    { "Seismic Probe",          "UULRDD",     "seismic_probe.svg" },
    { "Super Earth Flag",       "DUDU",       "super_earth_flag.svg" },
    { "Upload Data",            "LRUUU",      "upload_data.svg" },
    { "Machine Gun",            "DLDUR",      "machine_gun.svg" },
    { "Anti-Materiel Rifle",    "DLRUD",      "anti-materiel_rifle.svg" },
    { "Stalwart",               "DLDUUL",     "stalwart.svg" },
    { "Expendable Anti-Tank",   "DDLUR",      "expendable_anti-tank.svg" },
    { "Recoilless Rifle",       "DLRRL",      "recoilless_rifle.svg" },
    { "Flamethrower",           "DLUDU",      "flamethrower.svg" },
    { "Autocannon",             "DLDUUR",     "autocannon.svg" },
    { "Railgun",                "DRDULR",     "railgun.svg" },
    { "Spear",                  "DDUDD",      "spear.svg" },
    { "Quasar Canon LAS-99",    "DDULR",      "quasar_canon.svg" },
    { "Airburst Rocket Launcher","DUULR",     "airburst_rocket_launcher.svg" },
    { "Orbital Gatling Barrage","RDLUU",      "orbital_gatling_barrage.svg" },
    { "Orbital Airburst Strike","RRR",        "orbital_airburst_strike.svg" },
    { "Orbital 120MM HE Barrage","RRDLRD",    "orbital_120mm_he_barrage.svg" },
    { "Orbital 380MM HE Barrage","RDUULDD",   "orbital_380mm_he_barrage.svg" },
    { "Orbital Walking Barrage","RDRDRD",     "orbital_walking_barrage.svg" },
    { "Orbital Laser",          "RDURD",      "orbital_laser.svg" },
    { "Orbital Railcannon Strike","RUDDR",    "orbital_railcannon_strike.svg" },
    { "Eagle Strafing Run",     "URR",        "eagle_strafing_run.svg" },
    { "Eagle Airstrike",        "URDR",       "eagle_airstrike.svg" },
    { "Eagle Cluster Bomb",     "URDDR",      "eagle_cluster_bomb.svg" },
    { "Eagle Napalm Airstrike", "URDU",       "eagle_napalm_airstrike.svg" },
    { "Jump Pack",              "DUUDU",      "jump_pack.svg" },
    { "Eagle Smoke Strike",     "URUD",       "eagle_smoke_strike.svg" },
    { "Eagle 110MM Rocket Pods","URUL",       "eagle_110mm_rocket_pods.svg" },
    { "Eagle 500KG Bomb",       "URDDD",      "eagle_500kg_bomb.svg" },
    { "Eagle Rearm",            "UULUR",      "eagle_rearm.svg" },
    { "Orbital Precision Strike","RRU",       "orbital_precision_strike.svg" },
    { "Orbital Gas Strike",     "RRDR",       "orbital_gas_strike.svg" },
    { "Orbital EMS Strike",     "RRLD",       "orbital_ems_strike.svg" },
    { "Orbital Smoke Strike",   "RRDU",       "orbital_smoke_strike.svg" },
    { "HMG Emplacement",        "DULRRL",     "hmg_emplacement.svg" },
    { "Shield Generator Pack",  "DULRLR",     "shield_generator_pack.svg" },
    { "Tesla Tower",            "DURULR",     "tesla_tower.svg" },
    { "Anti-Personnel Minefield","DLUR",      "anti-personnel_minefield.svg" },
    { "Supply Pack",            "DLDUUD",     "supply_pack.svg" },
    { "Grenade Launcher",       "DLULD",      "grenade_launcher.svg" },
    { "Laser Cannon",           "DLDUL",      "laser_cannon.svg" },
    { "Incendiary Mines",       "DLLD",       "incendiary_mines.svg" },
    { "Guard Dog Rover",        "DULURR",     "guard_dog_rover.svg" },
    { "Ballistic Shield Backpack","DLDDUL",   "ballistic_shield_backpack.svg" },
    { "Arc Thrower",            "DRDULL",     "arc_thrower.svg" },
    { "Shield Generator Relay", "DDLRLR",     "shield_generator_relay.svg" },
    { "Heavy Machine Gun MG-101","DLUDD",     "heavy_machine_gun.svg" },
    { "Machine Gun Sentry",     "DURRU",      "machine_gun_sentry.svg" },
    { "Gatling Sentry",         "DURL",       "gatling_sentry.svg" },
    { "Mortar Sentry",          "DURRD",      "mortar_sentry.svg" },
    { "Guard Dog",              "DULURD",     "guard_dog.svg" },
    { "Autocannon Sentry",      "DURULU",     "autocannon_sentry.svg" },
    { "Rocket Sentry",          "DURRL",      "rocket_sentry.svg" },
    { "EMS Mortar Sentry",      "DURDR",      "ems_mortar_sentry.svg" },
    { "Patriot Exosuit",        "LDRULDD",    "patriot_exosuit.svg" },
    { "Commando",               "DLUDR",      "commando.svg" },
    { "Dark Fluid Vessel",      "ULRDUU",     "dark_fluid_vessel.svg" },
    { "Emancipator Exosuit",    "LDRULDU",    "emancipator_exosuit.svg" },
    { "Hive Breaker Drill",     "LUDRDD",     "hive_breaker_drill.svg" },
    { "Tectonic Drill",         "UDUDUD",     "tectonic_drill.svg" }
};

// Count the number of entries
const int StratagemGame::NUM_STRATAGEMS =
    sizeof(StratagemGame::s_stratagemData)/sizeof(StratagemGame::s_stratagemData[0]);

// ---------------------------------------------------
// Constructor
// ---------------------------------------------------
StratagemGame::StratagemGame(ButtonManager &btnMgr, AudioManager &audioMgr)
: display(HAL::displayProxy())
, buttonManager(btnMgr)
, audioManager(audioMgr)
{
    instance = this;
    currentState         = WAIT_START;
    score                = 0;
    timeRemaining        = TOTAL_TIME;
    lastUpdateTime       = 0;
    hitlagStartTime      = 0;
    currentSequenceIndex = 0;
}

// ---------------------------------------------------
// begin()
// ---------------------------------------------------
void StratagemGame::begin() 
{
    // Use the new button indices from HAL or your environment
    // e.g. button_LeftIndex, button_RightIndex, button_UpIndex, button_DownIndex, button_SelectIndex
    buttonManager.registerCallback(button_LeftIndex,  onButtonEvent);
    buttonManager.registerCallback(button_RightIndex, onButtonEvent);
    buttonManager.registerCallback(button_UpIndex,    onButtonEvent);
    buttonManager.registerCallback(button_DownIndex,  onButtonEvent);

    // “Back” or “Select” button:
    buttonManager.registerCallback(button_SelectIndex, onButtonBackPressed);
    buttonManager.registerCallback(button_EnterIndex,  onButtonEnterPressed);

    resetGame();
}

// ---------------------------------------------------
// end()
// ---------------------------------------------------
void StratagemGame::end()
{
    buttonManager.unregisterCallback(button_LeftIndex);
    buttonManager.unregisterCallback(button_RightIndex);
    buttonManager.unregisterCallback(button_UpIndex);
    buttonManager.unregisterCallback(button_DownIndex);
    buttonManager.unregisterCallback(button_SelectIndex);

    audioManager.stopTone();
    //audioManager.stopFile();
}

// ---------------------------------------------------
// update()
// ---------------------------------------------------
void StratagemGame::update()
{
    unsigned long now = millis();

    switch (currentState) {
        case WAIT_START:
            // Just draw a "Press a direction to start" screen
            drawScreen();
            break;

        case RUNNING:
        {
            // Compute time delta
            if (lastUpdateTime == 0) {
                lastUpdateTime = now;
            }
            unsigned long delta = now - lastUpdateTime;
            lastUpdateTime = now;

            // Decrease timeRemaining
            if (delta < timeRemaining) {
                timeRemaining -= delta;
            } else {
                timeRemaining = 0;
            }

            // If time runs out, game over
            if (timeRemaining == 0) {
                gameOver();
                return;
            }

            drawScreen();
            break;
        }

        case HITLAG:
            handleHitlag();
            break;

        case GAME_OVER:
            // Show final score
            drawGameOver();
            // Could reset automatically or wait for user’s “select/back” press
            break;
    }
}

// ---------------------------------------------------
// resetGame()
// ---------------------------------------------------
void StratagemGame::resetGame()
{
    currentState         = WAIT_START;
    score                = 0;
    timeRemaining        = TOTAL_TIME;
    lastUpdateTime       = 0;
    hitlagStartTime      = 0;
    currentSequenceIndex = 0;
}

// ---------------------------------------------------
// pickRandomStratagem()
// ---------------------------------------------------
void StratagemGame::pickRandomStratagem()
{
    if (NUM_STRATAGEMS == 0) {
        // fallback
        currentStratagem = { "No Data", "", "none.png" };
        return;
    }
    int idx = random(0, NUM_STRATAGEMS);
    currentStratagem = s_stratagemData[idx];
    currentSequenceIndex = 0;
}

// ---------------------------------------------------
// checkInput(char direction)
// ---------------------------------------------------
void StratagemGame::checkInput(char direction)
{
    if (!currentStratagem.sequence || strlen(currentStratagem.sequence) == 0) {
        return;
    }

    // Next needed char
    char needed = currentStratagem.sequence[currentSequenceIndex];
    if (direction == needed) {
        // Correct
        currentSequenceIndex++;
        playDirectionSound(direction);

        // Completed the entire sequence?
        if (currentSequenceIndex >= (int)strlen(currentStratagem.sequence)) {
            score++;
            timeRemaining += TIME_BONUS; // reward time
            currentState = HITLAG;
            hitlagStartTime = millis();
            lastUpdateTime = 0;
        }
    }
    else {
        // Wrong: reset partial progress
        currentSequenceIndex = 0;
        // Possibly play an “error beep”
        audioManager.playTone(220.0f, 150);
    }
}

// ---------------------------------------------------
// handleHitlag()
//   Wait out the short pause after finishing a sequence
// ---------------------------------------------------
void StratagemGame::handleHitlag()
{
    if (millis() - hitlagStartTime >= HITLAG_TIMEOUT) {
        // pick new stratagem, resume
        currentState = RUNNING;
        pickRandomStratagem();
        lastUpdateTime = millis(); 
    }
    else {
        drawScreen(); // optional, to keep display updated
    }
}

// ---------------------------------------------------
// gameOver()
// ---------------------------------------------------
void StratagemGame::gameOver()
{
    currentState = GAME_OVER;
    audioManager.playTone(150.0f, 300);  // or load a “game over” file
}

// ---------------------------------------------------
// onButtonEvent()
// ---------------------------------------------------
void StratagemGame::onButtonEvent(const ButtonEvent& event)
{
    if (!instance) return;
    if (event.eventType != ButtonEvent_Pressed) return; // only on press

    StratagemGame &g = *instance;

    if (g.currentState == WAIT_START) {
        // Start the game
        g.currentState = RUNNING;
        g.pickRandomStratagem();
        g.lastUpdateTime = millis();
        return;
    }

    if (g.currentState == GAME_OVER || g.currentState == HITLAG) {
        // ignore or handle differently if you wish
        return;
    }

    // So we are in RUNNING:
    char direction = '\0';
    // Map the hardware button index to 'U','D','L','R'
    if      (event.buttonIndex == button_LeftIndex)  direction = 'L';
    else if (event.buttonIndex == button_RightIndex) direction = 'R';
    else if (event.buttonIndex == button_UpIndex)    direction = 'U';
    else if (event.buttonIndex == button_DownIndex)  direction = 'D';
    else return;

    g.checkInput(direction);
}

// ---------------------------------------------------
// onButtonBackPressed()
//   For the “Select” or “Back” button
// ---------------------------------------------------
void StratagemGame::onButtonBackPressed(const ButtonEvent& event)
{
    if (!instance) return;
    if (event.eventType == ButtonEvent_Released) {
        instance->end();
        MenuManager::instance().returnToMenu();
    }
}

// ---------------------------------------------------
// onButtonEnterPressed()
//   For the “Select” or “Back” button
// ---------------------------------------------------
void StratagemGame::onButtonEnterPressed(const ButtonEvent& event)
{
    if (!instance) return;
    if (event.eventType == ButtonEvent_Released) {
        instance->resetGame();
    }
}

// ---------------------------------------------------
// drawScreen()
// ---------------------------------------------------
void StratagemGame::drawScreen()
{
    display.clear();

    switch (currentState) {
    case WAIT_START:
        display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        display.drawString(64, 32, "Press an arrow\nto START");
        display.display();
        return;

    case RUNNING:
    case HITLAG:
        drawRunning();
        return;

    case GAME_OVER:
        drawGameOver();
        return;
    }
}

// ---------------------------------------------------
// drawRunning()
//   Show name, time, score, plus the arrow sequence
// ---------------------------------------------------
void StratagemGame::drawRunning()
{
    // Title
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, String("Strat: ")+ currentStratagem.name);

    // Time & Score
    display.drawString(0, 12, String("Time: ")+ (timeRemaining/1000.0f));
    display.drawString(0, 24, String("Score: ")+ score);

    // Draw the arrow sequence
    // We'll show up to 8 arrows in a row, each 8x8 plus some spacing
    const char* seq = currentStratagem.sequence;
    int len = strlen(seq);
    int xStart = 0;
    int yStart = 36;
    for (int i = 0; i < len; i++) {
        bool isCompleted = (i < currentSequenceIndex);
        // Each arrow is 8 wide, let’s space them horizontally
        int16_t xPos = xStart + i * 10;
        drawArrowSprite(seq[i], xPos, yStart, isCompleted);
    }

    // Convert timeRemaining to a 0..100 integer for drawProgressBar:
    int progressValue = (int)(100.0f * ((float)timeRemaining / (float)TOTAL_TIME));

    // Pick a location and size for the progress bar:
    // For a 128x64 display, let's place it at y=52 or 54 with a height of ~8–10.
    display.drawProgressBar(
        /* x     = */ 9,
        /* y     = */ 54,
        /* width = */ 108,
        /* height= */ 8,
        /* progressValue% = */ progressValue
    );

    display.display();
}

// ---------------------------------------------------
// drawArrowSprite(char direction, x, y, filledOrNot)
//   Uses the 8x8 XBM images from StratagemGameSprites.h
// ---------------------------------------------------
void StratagemGame::drawArrowSprite(char direction, int16_t x, int16_t y, bool filled)
{
    // Choose sprite: if “filled” is true, use the Fill variant. Otherwise the outline.
    const unsigned char* sprite = nullptr;

    switch (direction) {
        case 'U':
            sprite = filled ? Arrow_Up_Fill_8x8 : Arrow_Up_8x8;
            break;
        case 'D':
            sprite = filled ? Arrow_Down_Fill_8x8 : Arrow_Down_8x8;
            break;
        case 'L':
            sprite = filled ? Arrow_Left_Fill_8x8 : Arrow_Left_8x8;
            break;
        case 'R':
            sprite = filled ? Arrow_Right_Fill_8x8 : Arrow_Right_8x8;
            break;
        default:
            sprite = filled ? Arrow_Up_Fill_8x8 : Arrow_Up_8x8; // fallback
            break;
    }

    // The displayProxy supports drawXbm(x, y, w, h, data)
    display.drawXbm(x, y, 8, 8, sprite);
}

// ---------------------------------------------------
// drawGameOver()
// ---------------------------------------------------
void StratagemGame::drawGameOver()
{
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(64, 20, "GAME OVER");
    display.drawString(64, 38, String("Score: ") + score);
    display.drawString(64, 54, "Press ENTER");
    display.display();
}

// ---------------------------------------------------
// playDirectionSound() - beep or file-based
// ---------------------------------------------------
void StratagemGame::playDirectionSound(char direction)
{
    switch (direction) {
        case 'U': audioManager.playTone(523.25f, 150); break; // C5
        case 'D': audioManager.playTone(329.63f, 150); break; // E4
        case 'L': audioManager.playTone(392.00f, 150); break; // G4
        case 'R': audioManager.playTone(261.63f, 150); break; // C4
        default: break;
    }
}
