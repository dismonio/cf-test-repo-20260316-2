# Cyber Fidget — Permissions Summary

This is a plain-language summary of what you can and cannot do with Cyber Fidget. It is not a substitute for the [LICENSE](LICENSE) file, which is the legally binding document.

## You CAN

- **Build and sell apps** for the Cyber Fidget using the HAL API. Apps you create through the HAL API are entirely yours — you choose the license.
- **Modify the firmware** for personal use, research, or contribution back to the project.
- **Distribute modified firmware** under the same GPL-3.0 license (you must share your source code).
- **Use Cyber Fidget in education** — classrooms, workshops, tutorials, YouTube videos, etc.
- **3D print enclosures** from published STL files (CC BY-SA 4.0 — share your remixes under the same terms).
- **Use the firmware commercially** under GPL-3.0 terms (source code must remain open).
- **Fork and build on this project** under GPL-3.0 terms.

## You CANNOT

- Use the "Cyber Fidget" name or logo without permission from Dismo Industries LLC (see [TRADEMARK.md](TRADEMARK.md)).
- Sell cloned devices under the Cyber Fidget brand.
- Distribute modified firmware without sharing the source code (GPL-3.0 requires it).
- Sublicense the firmware under different terms (it stays GPL-3.0).

## App Ownership

If your app interacts with the firmware **only** through the published HAL API headers (`HAL.h`, `DisplayProxy.h`, `ButtonManager.h`, `AudioManager.h`, `RGBController.h`, `globals.h`), it is **not** a derivative work. You own it completely and can license it however you want — open source, proprietary, commercial, whatever.

If your app modifies or includes firmware internals beyond the HAL API, it becomes a derivative work under GPL-3.0.

## Documentation

Documentation in the [cyberfidget-docs](https://github.com/CyberFidget/cyberfidget-docs) repository is licensed under CC BY-SA 4.0. You can share and adapt it with attribution.

## Questions?

For commercial licensing inquiries, OEM partnerships, or trademark permissions: **support@dismoindustries.com**
