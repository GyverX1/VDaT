# VDaT
### Real-time tilt telemetry for RC vehicles, any scale

**Real-time telemetry for RC crawlers, served wirelessly from a postage-stamp-sized ESP32.**

No internet connection required. No app to install. No account needed. Connect your phone to the truck's WiFi access point, open a browser, and your full telemetry dashboard loads instantly — attitude indicator, warning system, session logging, and all.

---

## Why "VDaT"?
<table>
<tr>
<td width="280">
<img width="260" alt="VDaT dashboard1" src="https://github.com/user-attachments/assets/57f4b73a-b975-418f-916c-2eceb0edd2db" />
</td>
<td>

VDaT stands for Vehicle Data and Telemetry. Not just a crawler tool — any vehicle, any scale. The sensor collects real data as you drive, and VDaT turns it into something you can actually look at: charts, peaks, warning history, the kind of information that helps you understand how a rig actually behaves instead of guessing.


</td>
</tr>
</table>


---

// <img width="720" height="1600" alt="Screenshot_20260701-184642" src="https://github.com/user-attachments/assets/57f4b73a-b975-418f-916c-2eceb0edd2db" />
## Features

- **Live pitch and roll**, with accelerometer data coming soon — independent peak tracking per direction (up/down, left/right)
- **Attitude indicator** — artificial horizon showing pitch, roll, and heading simultaneously
- **Configurable warning system** — independent thresholds per axis, HUD header alerts, flashing card borders, and Web Audio API beeps at two distinct frequencies (pitch vs roll)
- **Per-axis bar color thresholds** — independent yellow and red zones for pitch and roll bars
- **IMU zero calibration** — set current orientation as 0,0 reference, saved to flash, survives reboot
- **Session logging, two ways at once** — Phone Logging saves to your browser; Rig Logging saves a second copy directly onto the ESP's own onboard flash, independent of your phone
- **VDaT Studio integration** — pull rig-stored sessions and flash firmware straight over USB, no phone required
- **Add to home screen support** — the dashboard can be installed like an app on your phone if you prefer that over a browser tab
- **Live I2C diagnostics** — a built-in bus scanner confirms your IMU is wired correctly before you go hunting for problems blind
- **Full settings menu** — Thresholds, Calibration, Logging, Rig Name, About, Diagnostics
- **Rig Name** — user-configurable, displayed in the dashboard header
- **AP-only WiFi** — no home network credentials stored in firmware
- **Game Rotation Vector mode** — IMU runs gyro + accelerometer fusion only, immune to motor/servo magnetic interference

---

## Hardware

| Component | Product |
|---|---|
| Microcontroller | ESP32-S3 SuperMini (HW-747), 4MB Flash |
| IMU | BNO085 9-DOF IMU breakout |
| Power | USB-C cable, data-capable |
| Wiring | 4x Dupont jumper wires (female-to-female) |

**Total assembled weight: ~12 grams.**

An onboard display is possible on a second I2C bus but isn't part of the standard build right now — **Soon**.

---

## Wiring

The IMU connects to pins clustered right next to the board's `3V3(OUT)` pin, keeping the whole hookup on one side of the board with short, clean runs. No splitters, no shared bus.

```
ESP32-S3 SuperMini    IMU (BNO085)
─────────────────     ────────────
GP13  (SDA)      ──── SDA
GP12  (SCL)      ──── SCL
3V3              ──── VCC
GND              ──── GND
```

**IMU I2C Address:** 0x4B

Power the ESP32 via USB-C. This board uses native USB — no separate USB-UART driver needed.

---

## Software Setup

### Arduino IDE

Version 2.x recommended.

### Board Package

Install via **Boards Manager:**
- Package: `esp32` by Espressif Systems
- Board selection: **ESP32S3 Dev Module**

### Partition Scheme

Set **Partition Scheme** to **Huge APP (3MB No OTA/1MB SPIFFS)**. VDaT has no OTA update path — it's an offline access point with nothing to push an update from — so this scheme trades unused OTA headroom for real code space and a dedicated 1MB partition for Rig Logging.

### Libraries

Install via **Library Manager:**
- `Adafruit GFX Library`
- `Adafruit SSD1306`
- `SparkFun BNO080 Arduino Library`

Included with the ESP32 board package (no separate install):
- `WiFi`
- `WebServer`
- `Preferences`
- `Wire`
- `LittleFS`

---

## Configuration

All user-configurable constants live in `config.h`:

```cpp
// WiFi Access Point
#define AP_SSID      "VDATTELE"   // Change this to your preferred network name
#define AP_PASSWORD  "vdat1234"   // Change this — do not leave default in the field

// I2C Pins (IMU)
#define SDA_PIN      13
#define SCL_PIN      12

// IMU
#define IMU_ADDRESS  0x4B
```

After flashing, the Rig Name can be changed from the dashboard without reflashing — go to **Settings → Rig Name**.

---

## Connecting

1. Flash the firmware to your ESP32-S3 (directly via Arduino IDE, or through VDaT Studio's Install Firmware page)
2. Power on the ESP
3. On your phone, connect to the WiFi network configured in `config.h` (default: `VDATTELE`, password `vdat1234`)
4. Open your browser and navigate to `192.168.4.1`
5. The dashboard loads automatically

---

## IMU Mounting Notes

The BNO085's readings depend on how it's physically mounted. If pitch and roll read backward, swapped, or inverted, **no code changes or reflashing are needed** — go to **Settings → Calibration → Mounting Orientation** on the dashboard and pick the option that matches your mount. Four presets cover every way a flat board can be rotated around the vertical axis; the Live Reading on that same screen updates immediately so you can confirm you picked correctly.

After mounting, use **Settings → Calibration → Zero Now** on a level surface to set your 0,0 reference.

If you're not sure the IMU is wired correctly at all, check **Settings → Diagnostics** first — it runs a live I2C bus scan and tells you plainly whether the IMU is responding at its expected address.

---

## Session Logging, Two Ways

Every session can be recorded independently in two places at once:

- **Phone Logging** — saves to your connected device's browser (IndexedDB), survives page refreshes and network switches, works fully offline
- **Rig Logging** — saves a second copy directly onto the ESP's own onboard LittleFS storage, toggled from **Settings → Logging**

Rig-stored sessions can be listed, downloaded, and deleted straight from the dashboard — or pulled in bulk over USB using VDaT Studio's Grab Data action, with no phone required at all.

---

## Log Review Tool

VDaT includes a standalone offline log reviewer — `Docs/log-review.html`. VDaT Studio's Session Analyzer covers the same ground with a fuller feature set, but this standalone tool works anywhere, no install needed.

Open it in any browser on any device (phone, PC, tablet). No internet connection required. Load any CSV exported from the dashboard to get:

- Session summary with all peak values
- Warning breakdown per axis (pitch up/down, roll right/left independently)
- Time spent in warning and longest continuous warning event
- Most aggressive moment in the session
- Warning timeline bar showing exactly when warnings occurred
- Full pitch and roll chart with warning segments highlighted in red
- Scrollable data table with per-row warning flags
- Export warnings-only CSV for sharing just the sketchy moments

The CSV file format includes a metadata header with rig name, date, time, firmware version, and the warning thresholds that were active during the session — so the reviewer always knows exactly what rig produced the data and what it was tuned to.

---

## Project Structure

```
VDaT-S3/
├── VDaT-S3.ino          Main sketch
├── config.h              User configuration
├── globals.h / .cpp      Shared variables and objects
├── imu.h / .cpp          BNO085 sensor interface
├── display.h / .cpp      OLED display driver (considered feature, not part of standard setup)
├── boot_anim.h / .cpp    Animated boot sequence
├── web.h / .cpp          WiFi AP and web server
├── riglog.h / .cpp       Rig-side session logging (LittleFS)
├── serial_sync.h / .cpp  USB serial protocol for VDaT Studio
├── diagnostics.h / .cpp  Live I2C bus scanner
├── dashboard.h           Dashboard HTML assembler
├── utils.h / .cpp        Utility functions
├── Web/
│   ├── HTML/             HTML sub-headers (header, cards, menu, footer)
│   ├── JS/               JavaScript modules (utils, audio, attitude, storage, etc.)
│   └── PWA/              Installable dashboard — manifest, service worker, icons
└── Docs/
    ├── README.md         This file
    ├── Requirements.md   Hardware and software requirements
    └── log-review.html   Standalone offline log reviewer
```

---

## Version

**VDaT 1.0.1**
© 2026 GyverX

---

## Find Us

- GitHub: https://github.com/GyverX1
- Instagram: https://www.instagram.com/gyverx/
- YouTube: https://www.youtube.com/@GyverX

---

*Built by a maker, for makers.*
