/******************************************************************************
 * VDaT
 *
 * File:
 *    globals.cpp
 *
 * Purpose:
 *    Global object and variable definitions
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#include "globals.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"

//--------------------------------------------------
// Global Objects
//--------------------------------------------------

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire1,
    OLED_RESET);

BNO080 myIMU;

WebServer server(80);

Preferences prefs;

//--------------------------------------------------
// Telemetry
//--------------------------------------------------

float g_roll  = 0.0f;
float g_pitch = 0.0f;
float g_yaw   = 0.0f;

const char *g_compass = "N";

//--------------------------------------------------
// Hardware Connection Status
//--------------------------------------------------

bool imuConnected  = false;
bool oledConnected = false;
bool imuResponding = false;
int  imuOrientation = 0; // A - Standard

int imuIntPulsesPerSec  = 0;
int imuDataReadsPerSec  = 0;

//--------------------------------------------------
// Warning Thresholds
//--------------------------------------------------

float warnPitchUp    = 35.0f;
float warnPitchDown  = 35.0f;
float warnRollRight  = 35.0f;
float warnRollLeft   = 35.0f;

//--------------------------------------------------
// Bar Color Thresholds
//--------------------------------------------------

float pitchColorYellow = 15.0f;
float pitchColorRed    = 30.0f;
float rollColorYellow  = 15.0f;
float rollColorRed     = 30.0f;

//--------------------------------------------------
// IMU Calibration
//--------------------------------------------------

float pitchOffset = 0.0f;
float rollOffset  = 0.0f;

uint8_t imuCalStatus = 0;

//--------------------------------------------------
// Rig Identity
//--------------------------------------------------

char rigName[11] = "VDaT";
