// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

// CyberFidget Font Preview Module
// Standalone WASM module exposing the ThingPulse SSD1306 rasterizer
// for pixel-accurate font previews in the browser.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// ---- Arduino stubs (minimal, no app framework) ----

#define PROGMEM
#define pgm_read_byte(addr) (*(const uint8_t*)(addr))
#define pgm_read_word(addr) (*(const uint16_t*)(addr))
#define pgm_read_dword(addr) (*(const uint32_t*)(addr))
#define F(str) (str)

#define INPUT         0x0
#define OUTPUT        0x1
#define INPUT_PULLUP  0x2
#define INPUT_PULLDOWN 0x3
#define LOW           0x0
#define HIGH          0x1
#define SDA           21
#define SCL           22

using std::min;
using std::max;

// Arduino String class (needed by SSD1306Wire)
class String {
public:
    String() : _str() {}
    String(const char* s) : _str(s ? s : "") {}
    String(const String& s) : _str(s._str) {}
    String(int val) : _str(std::to_string(val)) {}
    const char* c_str() const { return _str.c_str(); }
    unsigned int length() const { return _str.length(); }
    String& operator=(const char* rhs) { _str = rhs ? rhs : ""; return *this; }
    String operator+(const String& rhs) const { return String((_str + rhs._str).c_str()); }
    String operator+(const char* rhs) const { return String((_str + (rhs ? rhs : "")).c_str()); }
    String& operator+=(const char* rhs) { if (rhs) _str += rhs; return *this; }
    String& operator+=(char c) { _str += c; return *this; }
    bool operator==(const char* rhs) const { return _str == (rhs ? rhs : ""); }
private:
    std::string _str;
};

// Stub externs required by SSD1306Wire.h
uint8_t wasm_button_states[8] = {0};
int wasm_analog_values[8] = {0};
const int wasm_pin_map[8] = {0};
uint32_t millis() { return 0; }
void delay(uint32_t) {}
inline void pinMode(int, int) {}
inline int digitalRead(int) { return HIGH; }
inline void digitalWrite(int, int) {}

// Serial stubs
extern "C" void js_serial_write(const char*, int) {}
class HardwareSerial {
public:
    void begin(unsigned long) {}
    void print(const char*) {}
    void println(const char*) {}
    void println() {}
    operator bool() { return true; }
};
HardwareSerial Serial;

// Framebuffer push stub (not used in preview mode, we read it directly)
extern "C" void js_push_framebuffer(const uint8_t*, int) {}

// ---- Font data pointers (declared as extern in SSD1306Wire.h) ----
// We need to provide these symbols before including SSD1306Wire.h

// Include the real ThingPulse font data
#include "OLEDDisplayFonts_real.h"

// Point the extern pointers to the _data arrays
const uint8_t* ArialMT_Plain_10 = ArialMT_Plain_10_data;
const uint8_t* ArialMT_Plain_16 = ArialMT_Plain_16_data;
const uint8_t* ArialMT_Plain_24 = ArialMT_Plain_24_data;

// Custom firmware fonts
#include "fontNotoSansJPReg.h"

// ---- Include the rasterizer ----
#include "SSD1306Wire.h"

// ---- Display instance ----
static SSD1306Wire display(0x3C, SDA, SCL);
static bool inverted = false;

// ---- Font Registry ----

struct FontEntry {
    const char* id;
    const uint8_t* data;
};

static const FontEntry STATIC_FONT_TABLE[] = {
    {"ArialMT_Plain_10", ArialMT_Plain_10_data},
    {"ArialMT_Plain_16", ArialMT_Plain_16_data},
    {"ArialMT_Plain_24", ArialMT_Plain_24_data},
    {"notoSansRg_10", (const uint8_t*)notoSansRg_10},
};
static constexpr int STATIC_FONT_COUNT = sizeof(STATIC_FONT_TABLE) / sizeof(STATIC_FONT_TABLE[0]);

// Dynamic font slots for uploaded fonts
static constexpr int MAX_DYNAMIC_FONTS = 16;
struct DynamicFontEntry {
    char id[64];
    uint8_t* data;
    int dataLen;
    bool used;
};
static DynamicFontEntry dynamicFonts[MAX_DYNAMIC_FONTS] = {};

// Find a font by ID (checks static then dynamic)
static const uint8_t* findFont(const char* fontId) {
    for (int i = 0; i < STATIC_FONT_COUNT; i++) {
        if (strcmp(STATIC_FONT_TABLE[i].id, fontId) == 0) {
            return STATIC_FONT_TABLE[i].data;
        }
    }
    for (int i = 0; i < MAX_DYNAMIC_FONTS; i++) {
        if (dynamicFonts[i].used && strcmp(dynamicFonts[i].id, fontId) == 0) {
            return dynamicFonts[i].data;
        }
    }
    return nullptr;
}

// ---- Helper: build JSON for font info ----
static char jsonBuf[4096];

static void appendFontInfo(char* buf, int& pos, int maxLen, const char* id, const uint8_t* data, bool dynamic, bool comma) {
    if (!data) return;
    uint8_t w = pgm_read_byte(data);
    uint8_t h = pgm_read_byte(data + 1);
    uint8_t fc = pgm_read_byte(data + 2);
    uint8_t nc = pgm_read_byte(data + 3);
    pos += snprintf(buf + pos, maxLen - pos,
        "%s{\"id\":\"%s\",\"width\":%d,\"height\":%d,\"firstChar\":%d,\"numChars\":%d,\"dynamic\":%s}",
        comma ? "," : "", id, w, h, fc, nc, dynamic ? "true" : "false");
}

// ---- Exported C API ----

extern "C" {

EMSCRIPTEN_KEEPALIVE
void fp_init() {
    display.init();
    display.clear();
}

EMSCRIPTEN_KEEPALIVE
void fp_clear() {
    display.clear();
}

EMSCRIPTEN_KEEPALIVE
int fp_set_font(const char* fontId) {
    const uint8_t* f = findFont(fontId);
    if (!f) return 0;
    display.setFont(f);
    return 1;
}

EMSCRIPTEN_KEEPALIVE
void fp_set_text_alignment(int align) {
    display.setTextAlignment((OLEDDISPLAY_TEXT_ALIGNMENT)align);
}

EMSCRIPTEN_KEEPALIVE
int fp_draw_string(int x, int y, const char* text) {
    display.setColor(inverted ? BLACK : WHITE);
    return display.drawString(x, y, String(text));
}

EMSCRIPTEN_KEEPALIVE
int fp_get_string_width(const char* text) {
    return display.getStringWidth(String(text));
}

EMSCRIPTEN_KEEPALIVE
uint8_t* fp_get_framebuffer() {
    return const_cast<uint8_t*>(display.getBuffer());
}

EMSCRIPTEN_KEEPALIVE
int fp_get_framebuffer_size() {
    return display.getBufferSize();
}

EMSCRIPTEN_KEEPALIVE
void fp_set_invert(int inv) {
    inverted = (inv != 0);
    // When inverted, we fill the display with white pixels as background
    if (inverted) {
        display.setColor(WHITE);
        display.fillRect(0, 0, 128, 64);
    }
}

EMSCRIPTEN_KEEPALIVE
void fp_set_color(int color) {
    display.setColor((OLEDDISPLAY_COLOR)color);
}

EMSCRIPTEN_KEEPALIVE
void fp_set_pixel(int x, int y) {
    display.setPixel(x, y);
}

EMSCRIPTEN_KEEPALIVE
void fp_fill_rect(int x, int y, int w, int h) {
    display.fillRect(x, y, w, h);
}

EMSCRIPTEN_KEEPALIVE
void fp_draw_rect(int x, int y, int w, int h) {
    display.drawRect(x, y, w, h);
}

// ---- Font enumeration ----

EMSCRIPTEN_KEEPALIVE
const char* fp_list_fonts() {
    int pos = 0;
    int maxLen = sizeof(jsonBuf);
    pos += snprintf(jsonBuf + pos, maxLen - pos, "[");

    bool first = true;
    for (int i = 0; i < STATIC_FONT_COUNT; i++) {
        appendFontInfo(jsonBuf, pos, maxLen, STATIC_FONT_TABLE[i].id, STATIC_FONT_TABLE[i].data, false, !first);
        first = false;
    }
    for (int i = 0; i < MAX_DYNAMIC_FONTS; i++) {
        if (dynamicFonts[i].used) {
            appendFontInfo(jsonBuf, pos, maxLen, dynamicFonts[i].id, dynamicFonts[i].data, true, !first);
            first = false;
        }
    }

    pos += snprintf(jsonBuf + pos, maxLen - pos, "]");
    return jsonBuf;
}

EMSCRIPTEN_KEEPALIVE
const char* fp_font_info(const char* fontId) {
    const uint8_t* f = findFont(fontId);
    if (!f) {
        snprintf(jsonBuf, sizeof(jsonBuf), "null");
        return jsonBuf;
    }
    int pos = 0;
    bool dynamic = false;
    for (int i = 0; i < MAX_DYNAMIC_FONTS; i++) {
        if (dynamicFonts[i].used && strcmp(dynamicFonts[i].id, fontId) == 0) {
            dynamic = true;
            break;
        }
    }
    snprintf(jsonBuf, sizeof(jsonBuf), "{\"id\":\"%s\",\"width\":%d,\"height\":%d,\"firstChar\":%d,\"numChars\":%d,\"dynamic\":%s}",
        fontId,
        pgm_read_byte(f),
        pgm_read_byte(f + 1),
        pgm_read_byte(f + 2),
        pgm_read_byte(f + 3),
        dynamic ? "true" : "false");
    return jsonBuf;
}

// ---- Dynamic font loading ----

EMSCRIPTEN_KEEPALIVE
uint8_t* fp_alloc_font(int size) {
    return (uint8_t*)malloc(size);
}

EMSCRIPTEN_KEEPALIVE
int fp_register_custom_font(const char* fontId, uint8_t* dataPtr, int dataLen) {
    // Check if ID already used, replace it
    for (int i = 0; i < MAX_DYNAMIC_FONTS; i++) {
        if (dynamicFonts[i].used && strcmp(dynamicFonts[i].id, fontId) == 0) {
            free(dynamicFonts[i].data);
            dynamicFonts[i].data = dataPtr;
            dynamicFonts[i].dataLen = dataLen;
            return 1;
        }
    }
    // Find empty slot
    for (int i = 0; i < MAX_DYNAMIC_FONTS; i++) {
        if (!dynamicFonts[i].used) {
            strncpy(dynamicFonts[i].id, fontId, sizeof(dynamicFonts[i].id) - 1);
            dynamicFonts[i].id[sizeof(dynamicFonts[i].id) - 1] = '\0';
            dynamicFonts[i].data = dataPtr;
            dynamicFonts[i].dataLen = dataLen;
            dynamicFonts[i].used = true;
            return 1;
        }
    }
    return 0; // No slots available
}

EMSCRIPTEN_KEEPALIVE
void fp_unregister_custom_font(const char* fontId) {
    for (int i = 0; i < MAX_DYNAMIC_FONTS; i++) {
        if (dynamicFonts[i].used && strcmp(dynamicFonts[i].id, fontId) == 0) {
            free(dynamicFonts[i].data);
            dynamicFonts[i].data = nullptr;
            dynamicFonts[i].dataLen = 0;
            dynamicFonts[i].used = false;
            return;
        }
    }
}

// ---- High-level render preview ----
// Accepts a simple format: "fontId\nsize1:y1:text1\nsize2:y2:text2\n..."
// This avoids needing a JSON parser in C++.

EMSCRIPTEN_KEEPALIVE
uint8_t* fp_render_preview(const char* payload) {
    display.clear();

    if (inverted) {
        display.setColor(WHITE);
        display.fillRect(0, 0, 128, 64);
    }

    // Parse: first line is fontId, subsequent lines are "y:text"
    // Font is already set via fp_set_font, so payload is just lines: "y:text\ny:text\n..."
    const char* p = payload;
    while (*p) {
        // Parse y coordinate
        int y = 0;
        while (*p >= '0' && *p <= '9') {
            y = y * 10 + (*p - '0');
            p++;
        }
        if (*p == ':') p++;

        // Extract text until newline or end
        const char* textStart = p;
        while (*p && *p != '\n') p++;
        int textLen = p - textStart;
        if (*p == '\n') p++;

        if (textLen > 0) {
            char textBuf[256];
            int copyLen = textLen < 255 ? textLen : 255;
            memcpy(textBuf, textStart, copyLen);
            textBuf[copyLen] = '\0';

            display.setColor(inverted ? BLACK : WHITE);
            display.drawString(0, y, String(textBuf));
        }
    }

    return const_cast<uint8_t*>(display.getBuffer());
}

// ---- Multi-font render: "fontId:y:text\nfontId:y:text\n..." ----

EMSCRIPTEN_KEEPALIVE
uint8_t* fp_render_multi(const char* payload) {
    display.clear();

    if (inverted) {
        display.setColor(WHITE);
        display.fillRect(0, 0, 128, 64);
    }

    const char* p = payload;
    while (*p) {
        // Parse fontId
        const char* fontStart = p;
        while (*p && *p != ':') p++;
        int fontLen = p - fontStart;
        if (*p == ':') p++;

        char fontIdBuf[64];
        int copyLen = fontLen < 63 ? fontLen : 63;
        memcpy(fontIdBuf, fontStart, copyLen);
        fontIdBuf[copyLen] = '\0';

        // Set font
        const uint8_t* f = findFont(fontIdBuf);
        if (f) display.setFont(f);

        // Parse y
        int y = 0;
        while (*p >= '0' && *p <= '9') {
            y = y * 10 + (*p - '0');
            p++;
        }
        if (*p == ':') p++;

        // Parse text until newline
        const char* textStart = p;
        while (*p && *p != '\n') p++;
        int textLen = p - textStart;
        if (*p == '\n') p++;

        if (textLen > 0 && f) {
            char textBuf[256];
            int tl = textLen < 255 ? textLen : 255;
            memcpy(textBuf, textStart, tl);
            textBuf[tl] = '\0';

            display.setColor(inverted ? BLACK : WHITE);
            display.drawString(0, y, String(textBuf));
        }
    }

    return const_cast<uint8_t*>(display.getBuffer());
}

} // extern "C"
