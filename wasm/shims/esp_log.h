// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_ESP_LOG_H
#define WASM_ESP_LOG_H

#include <cstdio>

#define ESP_LOG_NONE    0
#define ESP_LOG_ERROR   1
#define ESP_LOG_WARN    2
#define ESP_LOG_INFO    3
#define ESP_LOG_DEBUG   4
#define ESP_LOG_VERBOSE 5

inline void esp_log_level_set(const char*, int) {}

#define ESP_LOGE(tag, fmt, ...) printf("[E][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) printf("[W][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGI(tag, fmt, ...) printf("[I][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) printf("[D][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGV(tag, fmt, ...) ((void)0)

#endif
