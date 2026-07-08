/******************************************************************************
 * VDaT
 *
 * File:
 *    config.h
 *
 * Purpose:
 *    User configuration — WiFi, pins, I2C addresses, version
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

//====================================================
// Project
//====================================================

#define PROJECT_NAME        "VDaT"
#define PROJECT_VERSION     "1.0.1"
#define PROJECT_BUILD       "20260706"

//====================================================
// Debug
//====================================================
// Prints raw IMU timing/values straight to Serial Monitor, completely
// bypassing WiFi/web server. Use this to isolate whether stutter is
// coming from the sensor/I2C side or the WiFi/browser side.
#define DEBUG_IMU_SERIAL    false

//====================================================
// Display
//====================================================

#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define OLED_RESET          -1
#define OLED_ADDRESS        0x3C

//====================================================
// I2C — dual bus (SuperMini ESP32-S3, HW-747)
//====================================================
// Bus 1: BNO085 IMU — GP12/GP13, closest pair to the 3V3(OUT) pin
#define SDA_PIN             13  // GP13
#define SCL_PIN             12  // GP12

// Bus 2: SSD1306 OLED — GP8/GP9 (moved here in v1.1.0 to free GP10/GP11 for IMU RST/INT)
#define OLED_SDA_PIN        9   // GP9
#define OLED_SCL_PIN        8   // GP8

//====================================================
// IMU Hardware Control (v1.1.0)
//====================================================
// RST: hardware reset for the BNO085 chip itself — resets just the
// sensor, not the whole ESP32. Used as a recovery attempt if the sensor
// fails to respond at boot.
// INT: BNO085 pulls this low when new sensor data is ready. Used as a
// hint to poll more responsively, NOT as a hard gate — if this pin is
// ever miswired, the existing timed poll still runs as a fallback, so
// behavior degrades gracefully rather than breaking outright.
#define IMU_RST_PIN         10  // GP10
#define IMU_INT_PIN         11  // GP11

//====================================================
// IMU
//====================================================

#define IMU_ADDRESS         0x4B

//====================================================
// Rig-Side Logging (LittleFS)
//====================================================
// Session CSVs written directly to the ESP's flash, independent of
// phone-side (browser/IndexedDB) logging. Off by default — this is an
// opt-in second copy, not a replacement for phone logging.

#define RIG_LOG_DIR          "/logs"
#define RIG_LOG_ENABLED_DEFAULT false

// Current heading correction
#define HDG_OFFSET          -90.0f

//====================================================
// WiFi
//====================================================

#define AP_SSID             "VDATTELE"
#define AP_PASSWORD         "vdat1234"

#endif