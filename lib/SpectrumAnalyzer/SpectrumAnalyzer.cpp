// SPDX-License-Identifier: GPL-3.0-or-later WITH Cyberfidget-HAL-exception
// Copyright (c) 2023-2026 Dismo Industries LLC

#include "SpectrumAnalyzer.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// ---------------------------------------------------------------------------
// Construction / setup
// ---------------------------------------------------------------------------

SpectrumAnalyzer::SpectrumAnalyzer(Print& out) : p_out(&out) {}

bool SpectrumAnalyzer::begin(int sampleRate, int channels, int bitsPerSample) {
    _channels = channels;
    _bitsPerSample = bitsPerSample;

    // Precompute Hann window coefficients
    for (int i = 0; i < FFT_SIZE; i++) {
        hannWindow[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (FFT_SIZE - 1)));
    }

    initBands(sampleRate);
    sampleIdx = 0;
    skipCount = 0;
    peakVolume = 0.0f;
    memset(bandMagnitudes, 0, sizeof(bandMagnitudes));
    for (int i = 0; i < BAND_COUNT; i++) bandMax[i] = 0.001f;
    return true;
}

// ---------------------------------------------------------------------------
// Logarithmic band mapping: 16 bands from ~60 Hz to ~16 kHz
// ---------------------------------------------------------------------------

void SpectrumAnalyzer::initBands(int sampleRate) {
    const float freqLow  = 60.0f;
    const float freqHigh = 16000.0f;
    const float binWidth = (float)sampleRate / FFT_SIZE; // ~86 Hz per bin

    for (int i = 0; i < BAND_COUNT; i++) {
        float edgeLow  = freqLow * powf(freqHigh / freqLow, (float)i / BAND_COUNT);
        float edgeHigh = freqLow * powf(freqHigh / freqLow, (float)(i + 1) / BAND_COUNT);

        bandBinStart[i] = (int)(edgeLow / binWidth);
        bandBinEnd[i]   = (int)(edgeHigh / binWidth);

        // Clamp to valid range (bins 1 .. FFT_SIZE/2 - 1, skip DC)
        if (bandBinStart[i] < 1) bandBinStart[i] = 1;
        if (bandBinEnd[i] >= FFT_SIZE / 2) bandBinEnd[i] = FFT_SIZE / 2 - 1;
        // Ensure at least one bin per band
        if (bandBinEnd[i] < bandBinStart[i]) bandBinEnd[i] = bandBinStart[i];
    }
}

// ---------------------------------------------------------------------------
// ModifyingStream interface
// ---------------------------------------------------------------------------

void SpectrumAnalyzer::setStream(Stream& in) { p_stream = &in; }
void SpectrumAnalyzer::setOutput(Print& out) { p_out = &out; }

size_t SpectrumAnalyzer::readBytes(uint8_t* data, size_t len) {
    if (!p_stream) return 0;
    return p_stream->readBytes(data, len);
}

// ---------------------------------------------------------------------------
// write() — the main audio path
// ---------------------------------------------------------------------------

size_t SpectrumAnalyzer::write(const uint8_t* data, size_t len) {
    // --- Forward audio FIRST (minimize latency to BT output) ---
    size_t result = len;
    if (p_out) result = p_out->write(data, len);

    // --- Peak volume detection (clean loop, no branching) ---
    const int16_t* samples = (const int16_t*)data;
    int totalSamples = len / sizeof(int16_t);
    int16_t peak = 0;
    for (int i = 0; i < totalSamples; i += _channels) {
        int16_t absS = samples[i] < 0 ? -samples[i] : samples[i];
        if (absS > peak) peak = absS;
    }
    peakVolume = (float)peak / 32768.0f;

    // --- Buffer samples for FFT (separate pass, only when needed) ---
    // Skip SKIP_FRAMES windows between each FFT to reduce write() overhead.
    // At 44.1kHz/512 samples, raw rate is ~86Hz. With SKIP_FRAMES=3, we
    // process every 4th window → ~21Hz, still plenty for visualization.
    if (spectrumEnabled && !fftPending) {
        int monoAvail = totalSamples / _channels;
        int need = FFT_SIZE - sampleIdx;
        int toCopy = monoAvail < need ? monoAvail : need;
        for (int i = 0; i < toCopy; i++) {
            sampleBuf[sampleIdx + i] = samples[i * _channels];
        }
        sampleIdx += toCopy;
        if (sampleIdx >= FFT_SIZE) {
            if (skipCount >= SKIP_FRAMES) {
                fftPending = true;
                skipCount = 0;
            } else {
                // Discard this window, start filling the next
                sampleIdx = 0;
                skipCount++;
            }
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// processFFT() — called from main loop, NOT from audio write path
// ---------------------------------------------------------------------------

bool SpectrumAnalyzer::processFFT() {
    if (!fftPending) return false;
    runFFT();

    // Store current analysis in delay history ring buffer
    AnalysisFrame& frame = delayHistory[delayWriteIdx];
    memcpy(frame.bands, bandMagnitudes, sizeof(bandMagnitudes));
    frame.amplitude = peakVolume;
    frame.timestamp = millis();
    delayWriteIdx = (delayWriteIdx + 1) % DELAY_HISTORY_SIZE;

    sampleIdx = 0;
    fftPending = false;
    return true;
}

// ---------------------------------------------------------------------------
// FFT execution + band magnitude extraction
// ---------------------------------------------------------------------------

void SpectrumAnalyzer::runFFT() {
    // Copy samples to float and apply Hann window
    for (int i = 0; i < FFT_SIZE; i++) {
        fftRe[i] = ((float)sampleBuf[i] / 32768.0f) * hannWindow[i];
        fftIm[i] = 0.0f;
    }

    fftRadix2(fftRe, fftIm, FFT_SIZE);

    // Compute magnitude per band with per-band adaptive normalization.
    // Each band tracks its own maximum independently, so bass energy
    // can't flatten out the treble bands. This is how classic VFD EQ
    // displays worked — each bar had its own AGC (automatic gain control).
    for (int b = 0; b < BAND_COUNT; b++) {
        float bandPeak = 0.0f;
        for (int bin = bandBinStart[b]; bin <= bandBinEnd[b]; bin++) {
            float mag = fftRe[bin] * fftRe[bin] + fftIm[bin] * fftIm[bin];
            if (mag > bandPeak) bandPeak = mag;
        }
        bandPeak = sqrtf(bandPeak);

        // Per-band adaptive scaling: each band normalizes to its own peak
        if (bandPeak > bandMax[b]) bandMax[b] = bandPeak;
        bandMagnitudes[b] = bandPeak / bandMax[b];

        // Slow decay so bands adapt to changing content (~3s to halve)
        bandMax[b] *= 0.995f;
        if (bandMax[b] < 0.001f) bandMax[b] = 0.001f;
    }
}

// ---------------------------------------------------------------------------
// Public accessors
// ---------------------------------------------------------------------------

float SpectrumAnalyzer::volumeRatio() { return peakVolume; }
const float* SpectrumAnalyzer::bands() { return bandMagnitudes; }

void SpectrumAnalyzer::setDelayMs(int ms) { delayMs = ms; }

void SpectrumAnalyzer::storeDelayFrame() {
    AnalysisFrame& frame = delayHistory[delayWriteIdx];
    memcpy(frame.bands, bandMagnitudes, sizeof(bandMagnitudes));
    frame.amplitude = peakVolume;
    frame.timestamp = millis();
    delayWriteIdx = (delayWriteIdx + 1) % DELAY_HISTORY_SIZE;
}

// Find the frame closest to (now - delayMs) in the ring buffer
const SpectrumAnalyzer::AnalysisFrame& SpectrumAnalyzer::findDelayedFrame() const {
    if (delayMs <= 0) return delayHistory[(delayWriteIdx - 1 + DELAY_HISTORY_SIZE) % DELAY_HISTORY_SIZE];
    uint32_t target = millis() - delayMs;
    int bestIdx = -1;
    uint32_t bestDiff = UINT32_MAX;
    for (int i = 0; i < DELAY_HISTORY_SIZE; i++) {
        if (delayHistory[i].timestamp == 0) continue;  // unused slot
        uint32_t diff = (target >= delayHistory[i].timestamp)
            ? (target - delayHistory[i].timestamp)
            : (delayHistory[i].timestamp - target);
        if (diff < bestDiff) {
            bestDiff = diff;
            bestIdx = i;
        }
    }
    return (bestIdx >= 0) ? delayHistory[bestIdx] : zeroBands;
}

float SpectrumAnalyzer::delayedVolumeRatio() {
    return findDelayedFrame().amplitude;
}

const float* SpectrumAnalyzer::delayedBands() {
    return findDelayedFrame().bands;
}

// ---------------------------------------------------------------------------
// Radix-2 Cooley-Tukey FFT (in-place, iterative)
// n must be a power of 2.
// ---------------------------------------------------------------------------

void SpectrumAnalyzer::fftRadix2(float* re, float* im, int n) {
    // Bit-reversal permutation
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) {
            float tmp;
            tmp = re[i]; re[i] = re[j]; re[j] = tmp;
            tmp = im[i]; im[i] = im[j]; im[j] = tmp;
        }
    }

    // Butterfly stages
    for (int len = 2; len <= n; len <<= 1) {
        float ang = -2.0f * M_PI / len;
        float wRe = cosf(ang);
        float wIm = sinf(ang);

        for (int i = 0; i < n; i += len) {
            float curRe = 1.0f, curIm = 0.0f;
            int half = len >> 1;
            for (int j = 0; j < half; j++) {
                int u = i + j;
                int v = u + half;
                float tRe = re[v] * curRe - im[v] * curIm;
                float tIm = re[v] * curIm + im[v] * curRe;

                re[v] = re[u] - tRe;
                im[v] = im[u] - tIm;
                re[u] += tRe;
                im[u] += tIm;

                float newRe = curRe * wRe - curIm * wIm;
                curIm = curRe * wIm + curIm * wRe;
                curRe = newRe;
            }
        }
    }
}
