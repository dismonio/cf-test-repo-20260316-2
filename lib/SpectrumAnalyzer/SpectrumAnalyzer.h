// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#pragma once
#include "AudioTools/CoreAudio/AudioStreams.h"

/// Audio passthrough stream that provides both volume metering and
/// 16-band FFT spectrum analysis.  Drop-in replacement for VolumeMeter.
class SpectrumAnalyzer : public audio_tools::ModifyingStream {
public:
    SpectrumAnalyzer(Print& out);

    /// Call after construction to precompute Hann window + band bin ranges.
    bool begin(int sampleRate = 44100, int channels = 2, int bitsPerSample = 16);

    // --- AudioStream interface ---
    size_t write(const uint8_t* data, size_t len) override;
    size_t readBytes(uint8_t* data, size_t len) override;

    // --- ModifyingStream interface ---
    void setStream(Stream& in) override;
    void setOutput(Print& out) override;

    /// Peak volume ratio 0.0–1.0 (replaces VolumeMeter::volumeRatio())
    float volumeRatio();

    /// 16 logarithmically-spaced frequency band magnitudes, 0.0–1.0
    static const int BAND_COUNT = 16;
    const float* bands();

    /// Delayed versions — returns data from `delayMs` ago to compensate for BT latency
    float delayedVolumeRatio();
    const float* delayedBands();
    void setDelayMs(int ms);
    int getDelayMs() const { return delayMs; }

    /// Enable/disable FFT processing (saves CPU when spectrum viz is off)
    bool spectrumEnabled = false;

    /// Call from main loop to run pending FFT (keeps FFT out of audio write path)
    bool processFFT();

    /// Store current amplitude in delay history (call from main loop when FFT is not running)
    void storeDelayFrame();

private:
    Print* p_out = nullptr;
    Stream* p_stream = nullptr;

    int _channels = 2;
    int _bitsPerSample = 16;

    // PCM sample buffer (mono, left channel only)
    static const int FFT_SIZE = 512;
    int16_t sampleBuf[FFT_SIZE];
    int sampleIdx = 0;
    bool fftPending = false;

    // FFT workspace
    float fftRe[FFT_SIZE];
    float fftIm[FFT_SIZE];
    float hannWindow[FFT_SIZE];

    // Output: 16 frequency bands (raw, unsmoothed)
    float bandMagnitudes[BAND_COUNT] = {0};
    float peakVolume = 0.0f;

    // Precomputed bin ranges for each band
    int bandBinStart[BAND_COUNT];
    int bandBinEnd[BAND_COUNT];

    // Per-band adaptive normalization (each band scales independently)
    float bandMax[BAND_COUNT];

    // Frame skipping: only buffer every Nth window to reduce write() overhead
    int skipCount = 0;
    static const int SKIP_FRAMES = 1;  // process 1 out of every 2 windows (~43Hz)

    void initBands(int sampleRate);
    void runFFT();

    /// In-place radix-2 Cooley-Tukey FFT
    static void fftRadix2(float* re, float* im, int n);

    // Delay compensation ring buffer — stores recent analysis frames
    // so LEDs can be delayed to match BT audio latency
    struct AnalysisFrame {
        float bands[BAND_COUNT];
        float amplitude;
        uint32_t timestamp;
    };
    static const int DELAY_HISTORY_SIZE = 16;  // ~350ms at ~43Hz FFT rate
    AnalysisFrame delayHistory[DELAY_HISTORY_SIZE];
    int delayWriteIdx = 0;
    int delayMs = 150;  // default BT latency compensation (user-tunable)

    // Fallback frame when delay buffer is empty
    AnalysisFrame zeroBands = {};

    const AnalysisFrame& findDelayedFrame() const;
};
