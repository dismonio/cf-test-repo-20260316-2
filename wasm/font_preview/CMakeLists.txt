# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (c) 2023-2026 Dismo Industries LLC
cmake_minimum_required(VERSION 3.13)
project(fontpreview)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fontpreview font_preview.cpp)

# fontNotoSansJPReg.h uses const char[] with values > 127, suppress narrowing
target_compile_options(fontpreview PRIVATE -Wno-c++11-narrowing)

# Include paths: shims for SSD1306Wire.h, font headers
target_include_directories(fontpreview PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../shims
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Fonts
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/MatrixScreensaver
)

# Emscripten-specific settings
if(EMSCRIPTEN)
    set_target_properties(fontpreview PROPERTIES
        SUFFIX ".js"
    )

    target_link_options(fontpreview PRIVATE
        "SHELL:-s EXPORTED_FUNCTIONS=[\
'_fp_init',\
'_fp_clear',\
'_fp_set_font',\
'_fp_set_text_alignment',\
'_fp_draw_string',\
'_fp_get_string_width',\
'_fp_get_framebuffer',\
'_fp_get_framebuffer_size',\
'_fp_set_invert',\
'_fp_set_color',\
'_fp_set_pixel',\
'_fp_fill_rect',\
'_fp_draw_rect',\
'_fp_list_fonts',\
'_fp_font_info',\
'_fp_alloc_font',\
'_fp_register_custom_font',\
'_fp_unregister_custom_font',\
'_fp_render_preview',\
'_fp_render_multi',\
'_malloc',\
'_free'\
]"
        "SHELL:-s EXPORTED_RUNTIME_METHODS=['cwrap','ccall','stringToNewUTF8','UTF8ToString','HEAPU8']"
        "SHELL:-s INITIAL_MEMORY=2097152"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s MODULARIZE=1"
        "SHELL:-s EXPORT_NAME='FontPreviewModule'"
        "SHELL:-s NO_EXIT_RUNTIME=1"
        "SHELL:-O2"
    )
endif()
