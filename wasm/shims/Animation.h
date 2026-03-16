// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_ANIMATION_H
#define WASM_ANIMATION_H

class Animation {
public:
    void start() {}
    void stop() {}
    bool isRunning() const { return false; }
};

#endif
