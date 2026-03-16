// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_ESP_SLEEP_H
#define WASM_ESP_SLEEP_H

typedef int gpio_num_t;
#define GPIO_NUM_15 15

typedef enum {
    ESP_SLEEP_WAKEUP_UNDEFINED,
    ESP_SLEEP_WAKEUP_ALL,
    ESP_SLEEP_WAKEUP_EXT0,
    ESP_SLEEP_WAKEUP_EXT1,
    ESP_SLEEP_WAKEUP_TIMER,
    ESP_SLEEP_WAKEUP_TOUCHPAD,
    ESP_SLEEP_WAKEUP_ULP
} esp_sleep_wakeup_cause_t;

inline void esp_sleep_enable_ext0_wakeup(gpio_num_t, int) {}
inline void esp_deep_sleep_start() {}
inline esp_sleep_wakeup_cause_t esp_sleep_get_wakeup_cause() { return ESP_SLEEP_WAKEUP_UNDEFINED; }

#endif
