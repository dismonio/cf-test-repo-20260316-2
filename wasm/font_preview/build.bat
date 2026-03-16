@echo off
REM SPDX-License-Identifier: GPL-3.0-or-later
REM Copyright (c) 2023-2026 Dismo Industries LLC
REM Build the font preview WASM module (Windows)
setlocal

set SCRIPT_DIR=%~dp0
set BUILD_DIR=%SCRIPT_DIR%build
set OUTPUT_DIR=%SCRIPT_DIR%..\..\..\cyberfidget_website\wasm

echo === Building Font Preview WASM Module ===

call emcmake cmake -B "%BUILD_DIR%" -S "%SCRIPT_DIR%"
if errorlevel 1 goto :error

call emmake cmake --build "%BUILD_DIR%"
if errorlevel 1 goto :error

if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
copy /Y "%BUILD_DIR%\fontpreview.js" "%OUTPUT_DIR%\"
copy /Y "%BUILD_DIR%\fontpreview.wasm" "%OUTPUT_DIR%\"

echo === Done: fontpreview.js + fontpreview.wasm copied to %OUTPUT_DIR% ===
goto :eof

:error
echo Build failed!
exit /b 1
