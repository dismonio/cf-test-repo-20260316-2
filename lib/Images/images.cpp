// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "images.h"

// Define the arrays here
const int epd_bitmap_allArray_LEN = 3;
const unsigned char* epd_bitmap_allArray[3] = {
    epd_bitmap_cf_logo,
    epd_bitmap_cf_logo_w_icon,
    epd_bitmap_retro_car_sunset
};

const int boot_epd_bitmap_allArray_LEN = 4;
const unsigned char* boot_epd_bitmap_allArray[4] = {
    epd_bitmap_cf_logo_w_icon_boot,
    epd_bitmap_cf_logo_w_icon_boot_1,
    epd_bitmap_cf_logo_w_icon_boot_2,
    epd_bitmap_cf_logo_w_icon_boot_3
};