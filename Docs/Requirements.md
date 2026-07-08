# VDaT — Requirements

Hardware, software, and library requirements for building and flashing VDaT.

---

## Development Environment

| Tool | Version |
|---|---|
| Arduino IDE | 2.x (recommended) |
| Board Package | esp32 by Espressif Systems |
| Board Selection | ESP32S3 Dev Module |

Install the ESP32 board package via **File → Preferences → Additional Boards Manager URLs:**

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Then install via **Tools → Board → Boards Manager** — search for `esp32`.

---

## Required Libraries

### Install via Library Manager (Tools → Manage Libraries)

| Library | Author | Purpose |
|---|---|---|
| Adafruit GFX Library | Adafruit | Graphics primitives for OLED |
| Adafruit SSD1306 | Adafruit | SSD1306 OLED driver |
| SparkFun BNO080 Arduino Library | SparkFun Electronics | BNO085 IMU interface |

### Included with ESP32 Board Package (no separate install)

| Library | Purpose |
|---|---|
| WiFi | WiFi Access Point management |
| WebServer | HTTP server for dashboard |
| Preferences | Persistent flash storage for settings |
| Wire | I2C communication (two instances — see Wiring below) |
| LittleFS | Onboard filesystem — powers Rig Logging |

---

## Hardware Bill of Materials

| Qty | Component | Specification | Notes |
|---|---|---|---|
| 1 | ESP32-S3 SuperMini | HW-747, 4MB Flash | Budget-friendly generic board — no need to pay more for a name-brand variant |
| 1 | IMU Module | BNO085 breakout | — |
| 1 | Power Cable | USB-C | data-capable, not charge-only |
| 4 | Jumper Wires | Dupont female-to-female | SDA, SCL, 3V3, GND |

**Total assembled weight: ~12 grams.** Worth knowing before you commit panel space or worry about balance on a small chassis.

An onboard display is possible on a second I2C bus but isn't part of the standard build right now — **Soon**.

> **Note:** No splitters needed — every wire runs straight from a GPIO to the IMU.

---

## Wiring Reference

The IMU connects to pins clustered right next to the board's `3V3(OUT)` pin — chosen deliberately so the whole hookup lands on one side of the board with short, clean runs.

```
ESP32-S3 SuperMini      IMU (BNO085)
─────────────────       ────────────
GP13 (SDA)          ──── SDA
GP12 (SCL)          ──── SCL
3V3                 ──── VCC
GND                 ──── GND
USB-C               ──── (power input, native USB — no driver needed)
```

Four wires, straight from the board to the BNO085. Nothing to splice or share.

---

## I2C Addresses

| Device | Address |
|---|---|
| BNO085 IMU | 0x4B |

Use the built-in **Settings → Diagnostics** page on the dashboard to confirm the IMU is responding at its expected address before troubleshooting further — it's a live I2C scan, not a guess.

---

## Compile Settings

| Setting | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| Upload Speed | 921600 |
| CPU Frequency | 240MHz (WiFi) |
| Flash Size | 4MB (32Mb) |
| Partition Scheme | **Huge APP (3MB No OTA/1MB SPIFFS)** |
| USB CDC On Boot | Disabled |
| Port | COM port of your ESP32 |

> **Why "Huge APP" and not the default?** VDaT has no path for over-the-air updates — it's an offline access point with no internet connection, so there's nothing to push an OTA update *from*. The default partition scheme reserves space for OTA regardless, which left the app dangerously close to full. Switching to a No-OTA scheme costs nothing functionally and roughly triples available code space, while still leaving a full 1MB partition free for Rig Logging.

---

## Flash Procedure

1. Connect the ESP32-S3 SuperMini to your PC via USB-C
2. Select the correct COM port in Arduino IDE
3. Select board: **ESP32S3 Dev Module**
4. Confirm Partition Scheme is set to **Huge APP (3MB No OTA/1MB SPIFFS)**
5. Click Upload
6. This board uses native USB (no separate USB-UART chip) — if the upload doesn't start or the port disappears, hold the **BOOT** button while plugging in the cable (or hold BOOT and tap RESET) to force download mode

---

## Rig-Side Logging (LittleFS)

VDaT can record sessions two ways at once, independently of each other:

- **Phone Logging** — saved in the connected phone's browser (works today, no extra setup)
- **Rig Logging** — saved directly to the ESP's own 1MB LittleFS partition, toggled on from **Settings → Logging** on the dashboard

Rig-stored sessions can be listed, downloaded, and deleted straight from the dashboard, with no dependency on the phone's copy existing at all. First boot on a freshly-flashed board will auto-format the LittleFS partition — this is normal and only happens once.

---

## Mounting Orientation

If pitch and roll read backward or swapped after mounting the IMU, no code changes or reflashing are needed. **Settings → Calibration → Mounting Orientation** on the dashboard offers four presets covering every way a flat board can be rotated around the vertical axis — pick the one matching your mount and watch the Live Reading confirm it.

---

## Version

**VDaT 1.0.1**
Last Updated: July 2026
