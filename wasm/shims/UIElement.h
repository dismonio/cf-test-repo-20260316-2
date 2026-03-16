// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_UIELEMENT_H
#define WASM_UIELEMENT_H

class UIElement {
public:
    float x = 0, y = 0, width = 0, height = 0;
    float targetX = 0, targetY = 0, targetWidth = 0, targetHeight = 0;
    void setPosition(float, float) {}
    void setSize(float, float) {}
};

#endif
