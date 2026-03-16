// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "AppDefs.h"
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include "esp_log.h"

#include "AppManifest_Includes.h"

// Print APP_COUNT value when the program starts
void printAppCount() {
    std::cout << "APP_COUNT is " << APP_COUNT << std::endl;
}

// This definition should only exist in ONE .cpp file
PrintAppCount<APP_COUNT> appCountPrinter;

extern void menuBegin();
extern void menuEnd();
extern void menuRun();

// 1) Build the array with the lines from "AppManifest.h"
#define APP_ENTRY(ID, LABEL, CATPATH, BEGINF, ENDF, RUNF) \
    { LABEL, CATPATH, BEGINF, ENDF, RUNF },

AppDefinition appDefs[APP_COUNT] = {
    #include "AppManifest.h"  // each line expands into { LABEL, PATH, begin..., end..., run... }
};

#undef APP_ENTRY

// Debugging
static_assert(sizeof(appDefs)/sizeof(appDefs[0]) == APP_COUNT, 
              "Mismatch between appDefs and APP_COUNT?");

//__attribute__((constructor)) // Ensures it runs at startup
void debugAppDefs() {
   for (int i = 0; i < (int)APP_COUNT; i++) {
       ESP_LOGI(TAG_MAIN, "Index=%d name=%s path=%s beginFunc=%p",
                i,
                (appDefs[i].name ? appDefs[i].name : "(null)"),
                (appDefs[i].categoryPath ? appDefs[i].categoryPath : "(null)"),
                (void*)(appDefs[i].beginFunc));
   }
}

// 2) (Optional) A function to parse each categoryPath
static void addAppToMenu(const char* label, const char* path, int appIndex)
{
    // For example, pass it to your MenuManager or something. We'll do a minimal example:
    // We might store in some global data structure “menuRoot”
    // Then parse path by splitting on '/'
    // E.g. "Tools/WiFi" => [ "Tools", "WiFi" ]
    // Then go create subcategories if needed, etc.

    // Pseudocode for path splitting:
    // Tools => Subcategory "WiFi" => add item "label" => appIndex
    // We'll keep it simple:
    // MenuManager::instance().addItem(path, label, (AppIndex)appIndex);

    // We'll do a simple placeholder:
    MenuManager::instance().registerApp(path, label, (AppIndex)appIndex);
}

void buildNestedMenu() {
   for (int i=0; i<(int)APP_COUNT; i++){
       ESP_LOGI("AppDefs","i=%d name=%s path=%s beginFunc? %s",
                i, appDefs[i].name, appDefs[i].categoryPath,
                (appDefs[i].beginFunc ? "YES":"NO"));
       addAppToMenu(appDefs[i].name, appDefs[i].categoryPath, i);
   }
}