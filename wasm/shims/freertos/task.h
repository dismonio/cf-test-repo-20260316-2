// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_FREERTOS_TASK_H
#define WASM_FREERTOS_TASK_H

#include "FreeRTOS.h"

inline BaseType_t xTaskCreatePinnedToCore(
    void (*)(void*), const char*, uint32_t, void*, UBaseType_t,
    TaskHandle_t*, int) { return pdPASS; }

inline BaseType_t xTaskCreate(
    void (*)(void*), const char*, uint32_t, void*, UBaseType_t,
    TaskHandle_t*) { return pdPASS; }

inline void vTaskDelay(TickType_t) {}
inline void vTaskDelete(TaskHandle_t) {}

#endif
