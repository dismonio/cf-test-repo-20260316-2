// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_ARDUINO_H
#define WASM_ARDUINO_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <string>

// ---- Pin modes and digital I/O ----
#define INPUT         0x0
#define OUTPUT        0x1
#define INPUT_PULLUP  0x2
#define INPUT_PULLDOWN 0x3
#define LOW           0x0
#define HIGH          0x1

// Button state array set by JS via exported WASM functions
extern uint8_t wasm_button_states[8];
extern int wasm_analog_values[8];

inline void pinMode(int, int) {}

inline int digitalRead(int pin) {
    for (int i = 0; i < 8; i++) {
        extern const int wasm_pin_map[8];
        if (wasm_pin_map[i] == pin) {
            return wasm_button_states[i] ? LOW : HIGH;
        }
    }
    return HIGH;
}

inline void digitalWrite(int, int) {}

inline int analogRead(int) {
    return wasm_analog_values[0];
}

inline int analogReadMilliVolts(int) {
    return (wasm_analog_values[0] * 3300) / 4095;
}

// ---- Timing ----
uint32_t millis();
void delay(uint32_t ms);
inline void delayMicroseconds(uint32_t) {}
inline uint32_t micros() { return millis() * 1000; }

// ---- Math helpers ----
inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
    if (in_max == in_min) return out_min;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline long constrain(long x, long a, long b) {
    return (x < a) ? a : ((x > b) ? b : x);
}

using std::min;
using std::max;
using std::abs;

// ---- Random ----
inline long random(long max_val) { return std::rand() % max_val; }
inline long random(long min_val, long max_val) {
    if (max_val <= min_val) return min_val;
    return min_val + (std::rand() % (max_val - min_val));
}
inline void randomSeed(unsigned long seed) { std::srand(seed); }

// ---- Memory / PROGMEM ----
#define PROGMEM
#define pgm_read_byte(addr) (*(const uint8_t*)(addr))
#define pgm_read_word(addr) (*(const uint16_t*)(addr))
#define pgm_read_dword(addr) (*(const uint32_t*)(addr))
#define F(str) (str)

// ---- String class (Arduino-compatible subset) ----
class String {
public:
    String() : _str() {}
    String(const char* s) : _str(s ? s : "") {}
    String(const String& s) : _str(s._str) {}
    String(int val) : _str(std::to_string(val)) {}
    String(unsigned int val) : _str(std::to_string(val)) {}
    String(long val) : _str(std::to_string(val)) {}
    String(unsigned long val) : _str(std::to_string(val)) {}
    String(float val, int dec = 2) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.*f", dec, val);
        _str = buf;
    }
    String(double val, int dec = 2) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.*f", dec, val);
        _str = buf;
    }

    const char* c_str() const { return _str.c_str(); }
    unsigned int length() const { return _str.length(); }
    bool isEmpty() const { return _str.empty(); }
    char charAt(unsigned int i) const { return (i < _str.size()) ? _str[i] : 0; }
    void toCharArray(char* buf, unsigned int len) const {
        strncpy(buf, _str.c_str(), len);
        if (len > 0) buf[len - 1] = '\0';
    }
    int toInt() const { return atoi(_str.c_str()); }
    float toFloat() const { return atof(_str.c_str()); }

    String& operator=(const String& rhs) { _str = rhs._str; return *this; }
    String& operator=(const char* rhs) { _str = rhs ? rhs : ""; return *this; }

    String operator+(const String& rhs) const { return String((_str + rhs._str).c_str()); }
    String operator+(const char* rhs) const { return String((_str + (rhs ? rhs : "")).c_str()); }
    String operator+(int val) const { return *this + String(val); }
    String operator+(unsigned int val) const { return *this + String(val); }
    String operator+(float val) const { return *this + String(val); }

    friend String operator+(const char* lhs, const String& rhs) {
        return String((std::string(lhs ? lhs : "") + rhs._str).c_str());
    }

    String& operator+=(const String& rhs) { _str += rhs._str; return *this; }
    String& operator+=(const char* rhs) { if (rhs) _str += rhs; return *this; }
    String& operator+=(char c) { _str += c; return *this; }

    bool operator==(const String& rhs) const { return _str == rhs._str; }
    bool operator==(const char* rhs) const { return _str == (rhs ? rhs : ""); }
    bool operator!=(const String& rhs) const { return _str != rhs._str; }
    char operator[](unsigned int i) const { return charAt(i); }

    int indexOf(char c) const { auto p = _str.find(c); return p == std::string::npos ? -1 : (int)p; }
    String substring(unsigned int from) const { return String(_str.substr(from).c_str()); }
    String substring(unsigned int from, unsigned int to) const {
        return String(_str.substr(from, to - from).c_str());
    }
    void replace(const String& f, const String& r) {
        size_t pos = 0;
        while ((pos = _str.find(f._str, pos)) != std::string::npos) {
            _str.replace(pos, f._str.length(), r._str);
            pos += r._str.length();
        }
    }
    void trim() {
        auto s = _str.find_first_not_of(" \t\r\n");
        auto e = _str.find_last_not_of(" \t\r\n");
        _str = (s == std::string::npos) ? "" : _str.substr(s, e - s + 1);
    }

private:
    std::string _str;
};

// ---- Serial ----
extern "C" void js_serial_write(const char* str, int len);

class HardwareSerial {
public:
    void begin(unsigned long) {}
    void end() {}

    void print(const char* s)          { _write(s); }
    void print(const String& s)        { _write(s.c_str()); }
    void print(int v)                  { char b[16]; snprintf(b,sizeof(b),"%d",v);   _write(b); }
    void print(unsigned int v)         { char b[16]; snprintf(b,sizeof(b),"%u",v);   _write(b); }
    void print(long v)                 { char b[16]; snprintf(b,sizeof(b),"%ld",v);  _write(b); }
    void print(unsigned long v)        { char b[16]; snprintf(b,sizeof(b),"%lu",v);  _write(b); }
    void print(float v)                { char b[24]; snprintf(b,sizeof(b),"%f",v);   _write(b); }
    void print(double v)               { char b[24]; snprintf(b,sizeof(b),"%f",v);   _write(b); }
    void println()                     { _write("\n"); }
    void println(const char* s)        { _write(s); _write("\n"); }
    void println(const String& s)      { _write(s.c_str()); _write("\n"); }
    void println(int v)                { print(v);  _write("\n"); }
    void println(unsigned int v)       { print(v);  _write("\n"); }
    void println(long v)               { print(v);  _write("\n"); }
    void println(unsigned long v)      { print(v);  _write("\n"); }
    void println(float v)              { print(v);  _write("\n"); }
    void println(double v)             { print(v);  _write("\n"); }

    int available() { return 0; }
    int read() { return -1; }
    void flush() {}
    operator bool() { return true; }

private:
    void _write(const char* s) {
        int len = (int)strlen(s);
        js_serial_write(s, len);
    }
};

extern HardwareSerial Serial;

// ---- Misc Arduino defines ----
#define SDA 21
#define SCL 22
#define LED_BUILTIN 13
#define A0 0

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1 << (bit)))
#define bitClear(value, bit) ((value) &= ~(1 << (bit)))
#define bit(b) (1 << (b))

#ifndef PI
#define PI 3.14159265358979323846
#endif

inline float radians(float deg) { return deg * PI / 180.0f; }
inline float degrees(float rad) { return rad * 180.0f / PI; }

// ---- time.h compatibility ----
#include <ctime>

#endif // WASM_ARDUINO_H
