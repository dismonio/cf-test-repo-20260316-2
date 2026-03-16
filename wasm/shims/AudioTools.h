// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023-2026 Dismo Industries LLC

#ifndef WASM_AUDIOTOOLS_H
#define WASM_AUDIOTOOLS_H

#include <cstdint>

namespace audio_tools {

struct AudioInfo {
    int sample_rate = 44100;
    int channels = 1;
    int bits_per_sample = 16;
};

class I2SStream {
public:
    bool begin() { return true; }
    bool begin(const AudioInfo&) { return true; }
    void end() {}
    int availableForWrite() { return 1024; }
    size_t write(const uint8_t*, size_t len) { return len; }
};

struct I2SConfig : public AudioInfo {
    int pin_bck = -1;
    int pin_ws = -1;
    int pin_data = -1;
    int pin_data_rx = -1;
    bool is_master = true;
    int i2s_format = 0;
    int signal_type = 0;
    bool auto_clear = true;
};

template<typename T>
class SineWaveGenerator {
public:
    void begin(const AudioInfo& info, float freq = 0) { (void)info; (void)freq; }
    void setFrequency(float) {}
};

template<typename T>
class GeneratedSoundStream {
public:
    GeneratedSoundStream() {}
    GeneratedSoundStream(SineWaveGenerator<T>&) {}
    bool begin(const AudioInfo&) { return true; }
    void end() {}
    int available() { return 0; }
    size_t readBytes(uint8_t*, size_t) { return 0; }
};

class VolumeStream {
public:
    bool begin(const AudioInfo&) { return true; }
    void setVolume(float) {}
    void end() {}
    int availableForWrite() { return 1024; }
    size_t write(const uint8_t*, size_t len) { return len; }
};

class StreamCopy {
public:
    StreamCopy() {}
    template<typename A, typename B>
    StreamCopy(A&, B&, int bufSize = 1024) { (void)bufSize; }
    size_t copy() { return 0; }
    void begin() {}
};

class VolumeMeter {
public:
    bool begin(const AudioInfo&) { return true; }
    float volume() { return 0.0f; }
    void end() {}
    int availableForWrite() { return 1024; }
    size_t write(const uint8_t*, size_t len) { return len; }
};

} // namespace audio_tools

#endif
