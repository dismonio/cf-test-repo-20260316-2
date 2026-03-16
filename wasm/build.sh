#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (c) 2023-2026 Dismo Industries LLC
# Build the CyberFidget WASM emulator
# Prerequisites: Emscripten SDK installed and activated
#   emsdk install latest && emsdk activate latest
#   source emsdk_env.sh  (or emsdk_env.bat on Windows)
#
# Usage:
#   cd wasm/
#   ./build.sh
#
# Output: build/cyberfidget.js + build/cyberfidget.wasm

set -e

BUILD_DIR="build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "=== Configuring with Emscripten ==="
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release

echo "=== Building ==="
emmake make -j$(nproc 2>/dev/null || echo 4)

echo ""
echo "=== Build complete ==="
echo "Output files:"
ls -la cyberfidget.js cyberfidget.wasm 2>/dev/null || echo "(check build directory for output)"
echo ""
echo "To test: copy cyberfidget.js and cyberfidget.wasm to the website's /wasm/ directory"
