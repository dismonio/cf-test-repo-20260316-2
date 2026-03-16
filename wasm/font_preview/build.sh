#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (c) 2023-2026 Dismo Industries LLC
# Build the font preview WASM module
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
OUTPUT_DIR="${1:-$SCRIPT_DIR/../../../cyberfidget_website/wasm}"

echo "=== Building Font Preview WASM Module ==="
echo "Build dir: $BUILD_DIR"
echo "Output dir: $OUTPUT_DIR"

# Configure with Emscripten
emcmake cmake -B "$BUILD_DIR" -S "$SCRIPT_DIR"

# Build
emmake cmake --build "$BUILD_DIR"

# Copy outputs
mkdir -p "$OUTPUT_DIR"
cp "$BUILD_DIR/fontpreview.js" "$OUTPUT_DIR/"
cp "$BUILD_DIR/fontpreview.wasm" "$OUTPUT_DIR/"

echo "=== Done: fontpreview.js + fontpreview.wasm copied to $OUTPUT_DIR ==="
