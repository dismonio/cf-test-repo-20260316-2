@echo off
REM SPDX-License-Identifier: GPL-3.0-or-later
REM Copyright (c) 2023-2026 Dismo Industries LLC
REM Build the CyberFidget WASM emulator (Windows)
REM Prerequisites: Emscripten SDK installed and activated
REM   emsdk install latest
REM   emsdk activate latest
REM   emsdk_env.bat
REM
REM Usage:
REM   cd wasm\
REM   build.bat

if not exist build mkdir build
cd build

echo === Configuring with Emscripten ===
call emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo.
    echo ERROR: emcmake failed. Make sure Emscripten SDK is installed and emsdk_env.bat has been run.
    exit /b 1
)

echo === Building ===
call emmake cmake --build . --config Release
if errorlevel 1 (
    echo.
    echo ERROR: Build failed. Check the output above for details.
    exit /b 1
)

echo.
echo === Build complete ===
echo Output: build\cyberfidget.js + build\cyberfidget.wasm
echo Copy these to the website's \wasm\ directory to test.
