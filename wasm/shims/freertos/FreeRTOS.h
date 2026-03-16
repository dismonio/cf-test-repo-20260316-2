// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_FREERTOS_H
#define WASM_FREERTOS_H

typedef void* TaskHandle_t;
typedef void* SemaphoreHandle_t;
typedef unsigned int TickType_t;
typedef int BaseType_t;
typedef unsigned int UBaseType_t;

#define pdTRUE 1
#define pdFALSE 0
#define pdPASS pdTRUE
#define portMAX_DELAY 0xFFFFFFFF

#endif
