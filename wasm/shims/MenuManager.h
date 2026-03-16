// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_MENU_MANAGER_H
#define WASM_MENU_MANAGER_H

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

class MenuManager {
public:
    static MenuManager& instance() {
        static MenuManager s;
        return s;
    }

    void returnToMenu() {
#ifdef __EMSCRIPTEN__
        EM_ASM({ if (Module.onAppExit) Module.onAppExit(); });
#endif
    }

    void begin() {}
    void update() {}
    void end() {}

    void registerApp(const void*, const void*, int) {}
    bool isMenuActive() const { return false; }

private:
    MenuManager() {}
};

inline void menuBegin() {}
inline void menuEnd() {}
inline void menuRun() {}

#endif
