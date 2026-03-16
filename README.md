# Cyber Fidget Firmware

Open-source firmware for the [Cyber Fidget](https://cyberfidget.com) — a handheld ESP32-based gadget with a 128x64 OLED display, clicky  buttons, slider, addressable LEDs, accelerometer, microphone, speaker, uSD card reader, and WiFi/Bluetooth.

## Features

- **20+ built-in apps** — games (Dino, Breakout, Simon Says, Stratagem Hero, Spaceship), screensavers (Matrix, Graveyard, Eye, Ghosts), tools (Clock, Flashlight, Spectrum Analyzer), and more
- **Music player** — Bluetooth A2DP streaming with AVRCP controls and MP3 playback
- **Web portal** — WiFi-based configuration and control interface
- **App SDK** — Build your own apps using the HAL API (see below)
- **WASM emulator** — Run firmware apps in the browser for development and testing

## Building

### Prerequisites

- [PlatformIO](https://platformio.org/) (CLI or VS Code extension)
- ESP32 board (Adafruit Feather ESP32 V2 or compatible)

### Flash

```bash
# Build and flash over USB
pio run -e local -t upload

# Serial monitor
pio run -e local -t monitor
```

### WASM Emulator

Build the browser-based emulator using [Emscripten](https://emscripten.org/):

```bash
cd wasm
./build_wasm.sh                          # Default demo
./build_wasm.sh MyApp.h MyApp.cpp        # Custom app
```

Output: `wasm/build/cyberfidget.js` + `cyberfidget.wasm`

## Writing Apps

Apps interact with the hardware through the **HAL API** — a set of abstraction headers that decouple app logic from the underlying ESP32 drivers:

| Header | Purpose |
|---|---|
| `HAL.h` | Hardware initialization, accelerometer globals |
| `DisplayProxy.h` | 128x64 OLED drawing (lines, rects, text, bitmaps) |
| `ButtonManager.h` | Button event callbacks |
| `AudioManager.h` | Audio playback control |
| `RGBController.h` | NeoPixel LED control |
| `globals.h` | Shared state (slider position, battery, etc.) |

Apps follow the `begin()` / `update()` / `end()` lifecycle and register via the `APP_ENTRY` macro in `AppManifest.h`. See any app in `lib/` for examples.

**Apps you create through the HAL API are yours** — the linking exception in the license means they are not considered derivative works of the firmware, regardless of how they are compiled or linked.

## Project Structure

```
src/              Main entry point
lib/              App and library modules
  AppDefs/        App manifest and registration
  HAL/            Hardware abstraction layer
  DisplayProxy/   OLED display interface
  ButtonManager/  Button input handling
  DinoGame/       Example app (and 20+ others)
include/          Board configuration, credentials
wasm/             WASM emulator build infrastructure
  hal/            WASM HAL implementation
  shims/          ESP32/Arduino API shims for browser
scripts/          Build utilities
```

## License

GPL-3.0-or-later with a **HAL Linking Exception** — apps built through the published HAL API may be licensed under terms of your choice.

See [LICENSE](LICENSE) for the full text, [PERMISSIONS.md](PERMISSIONS.md) for a plain-language summary, and [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md) for dependency attribution.

## Links

- [cyberfidget.com](https://cyberfidget.com) — product site and online emulator
- [Documentation](https://docs.cyberfidget.com) — hardware specs, guides, API reference
- [Cyber Fidget Docs repo](https://github.com/CyberFidget/cyberfidget-docs) — documentation source

---

Copyright (c) 2023-2026 Dismo Industries LLC