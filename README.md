# ESPHome firmware for TRMNL e-paper devices

Multi-device ESPHome firmware for [TRMNL](https://usetrmnl.com) BYOD e-paper displays. Supports the official TRMNL hardware (ESP32-C3 + Waveshare 7.50inv2) and the Lilygo T5-ePaper-S3 (ESP32-S3 + ED047TC1 parallel e-paper).

## Supported Devices

| Device | MCU | Display | Resolution | Colors | Framework |
|--------|-----|---------|------------|--------|-----------|
| **TRMNL** | ESP32-C3 | Waveshare 7.50inv2 (SPI) | 800x480 | B&W (1-bit) | esp-idf |
| **Lilygo T5-ePaper-S3** | ESP32-S3 | ED047TC1 (parallel) | 960x540 | 16 grayscale (4-bit) | esp-idf |

Both devices use the **same ESP-IDF framework** — no Arduino dependency.

## Architecture

The firmware is split into three layers using ESPHome's `packages` feature:

```
devices/{device}/          ← Hardware config (MCU, pins, display, button, battery)
packages/trmnl-cloud.yaml  ← Shared cloud logic (TRMNL API, image download, deep sleep)
{device}-cloud.yaml        ← Thin wrapper combining device + cloud + display pages
```

### File Map

```
├── devices/
│   ├── trmnl/                        TRMNL hardware
│   │   ├── trmnl.yaml                ESP32-C3, Waveshare, GPIO9 button, GPIO3 battery
│   │   └── trmnl.factory.yaml        + improv_serial, dashboard_import, factory_reset
│   └── lilygo-t5-s3/                 Lilygo hardware
│       ├── lilygo.yaml               ESP32-S3, ED047TC1, GPIO21 button, GPIO14 battery
│       └── lilygo.factory.yaml       + improv_serial, dashboard_import, factory_reset
├── packages/
│   └── trmnl-cloud.yaml              TRMNL API, image fetch, shared scripts
├── components/
│   └── ed047tc1/                     Custom ESP-IDF display driver for Lilygo
│       ├── display.py                ESPHome config schema & codegen
│       ├── ed047tc1.h                C++ header
│       └── ed047tc1.cpp              C++ implementation
├── trmnl.yaml                        → devices/trmnl/trmnl.yaml
├── trmnl.factory.yaml                → devices/trmnl/trmnl.factory.yaml
├── trmnl-cloud.yaml                  TRMNL device + cloud + display pages
├── trmnl-cloud.factory.yaml
├── lilygo.yaml                       → devices/lilygo-t5-s3/lilygo.yaml
├── lilygo-cloud.yaml                 Lilygo device + cloud + display pages
└── lilygo-cloud.factory.yaml
```

## Building & Flashing

### TRMNL (Official Hardware)

```bash
# Basic firmware (no cloud, hello-world display)
esphome run trmnl.yaml

# Cloud firmware (full TRMNL BYOD experience)
esphome run trmnl-cloud.yaml
```

Or flash via the web at: `https://NiltonVolpato.github.io/esphome-trmnl/`

### Lilygo T5-ePaper-S3

```bash
# Basic firmware (hello-world display)
esphome run lilygo.yaml

# Cloud firmware (full TRMNL BYOD experience)
esphome run lilygo-cloud.yaml
```

#### Lilygo Pin Reference

| GPIO | Function |
|------|----------|
| 0,1,2,3,4,5,6,7,8 | E-paper data bus D0-D7 |
| 12,13 | E-paper config (CLK, DATA) |
| 38 | E-paper CKV |
| 40 | E-paper STH |
| 41 | E-paper CKH |
| **21** | **Button** (active low) |
| **14** | **Battery ADC** |
| 18 | I2C SDA |
| 17 | I2C SCL |
| 47 | Touch IRQ (optional) |
| 10,39,45,48 | Free GPIOs |

#### Battery Voltage

The battery ADC on GPIO14 uses a voltage divider. The `multiply: 2.0` filter in the config is a starting point — calibrate this against a known voltage for accurate readings.

## How It Works

1. **First boot** — device creates a Wi-Fi access point. Connect and provide credentials.
2. **Setup** — firmware calls `trmnl.app/api/setup` to register. The server returns an API key and friendly ID. Display shows a QR code for captive portal login.
3. **Normal operation** — firmware calls `trmnl.app/api/display` with the API key and battery voltage. The server returns an image URL and refresh interval.
4. **Deep sleep** — after rendering, the device enters deep sleep for the server-specified interval. Wake on button press (or GPIO2 on TRMNL).
5. **Repeat** from step 3.

The TRMNL server handles image format conversion (B&W or grayscale, dithering) based on the registered device type. The firmware renders whatever BMP the server sends at native resolution.

## Web Flasher

The `static/` directory contains a [ESP Web Tools](https://esphome.github.io/esp-web-tools/) manifest for browser-based flashing. After pushing to GitHub, enable GitHub Pages on the `main` branch to serve the flasher at:

```
https://NiltonVolpato.github.io/esphome-trmnl/
```

## Sources & Credits

- Original firmware: [jesserockz/esphome-trmnl](https://github.com/jesserockz/esphome-trmnl)
- Display driver component based on [nickolay/esphome-lilygo-t547plus](https://github.com/nickolay/esphome-lilygo-t547plus)
- Lilygo hardware library: [Xinyuan-LilyGO/LilyGo-EPD47](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47)
- EPD driver: [vroland/epdiy](https://github.com/vroland/epdiy)
- ESPHome feature discussion: [#1960](https://github.com/esphome/feature-requests/issues/1960)
