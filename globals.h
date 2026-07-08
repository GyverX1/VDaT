/******************************************************************************
 * VDaT
 *
 * File:
 *    globals.h
 *
 * Purpose:
 *    Global object and variable declarations
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef GLOBALS_H
#define GLOBALS_H

#include <Adafruit_SSD1306.h>
#include <WebServer.h>
#include <Preferences.h>
#include "SparkFun_BNO080_Arduino_Library.h"

extern Adafruit_SSD1306 display;
extern BNO080 myIMU;
extern WebServer server;
extern Preferences prefs;

extern float g_roll;
extern float g_pitch;
extern float g_yaw;
extern const char *g_compass;

//--------------------------------------------------
// Hardware Connection Status
//--------------------------------------------------

extern bool imuConnected;      // set once at boot from initIMU()
extern bool oledConnected;     // set once at boot from initDisplay()
extern bool imuResponding;     // updated every loop from dataAvailable()
extern int  imuOrientation;    // 0=A Standard, 1=B 180°, 2=C 90°CW, 3=D 90°CCW

// v1.1.0 — INT/data-read activity, sampled over a rolling 1-second
// window, updated continuously in the background by updateIMU().
// Diagnostics reads these snapshots rather than actively sampling —
// keeps the HTTP handler non-blocking.
extern int imuIntPulsesPerSec;   // how often INT signaled data-ready
extern int imuDataReadsPerSec;   // how often that actually produced a successful read

extern float warnPitchUp;
extern float warnPitchDown;
extern float warnRollRight;
extern float warnRollLeft;

//--------------------------------------------------
// Bar Color Thresholds (user configurable, per axis)
//--------------------------------------------------

extern float pitchColorYellow;   // degrees - pitch bar turns yellow past this
extern float pitchColorRed;      // degrees - pitch bar turns red past this
extern float rollColorYellow;    // degrees - roll bar turns yellow past this
extern float rollColorRed;       // degrees - roll bar turns red past this

//--------------------------------------------------
// IMU Calibration
//--------------------------------------------------

extern float pitchOffset;
extern float rollOffset;

extern uint8_t imuCalStatus;

//--------------------------------------------------
// Rig Identity
//--------------------------------------------------

extern char rigName[11];   // max 10 chars + null terminator

#endif
