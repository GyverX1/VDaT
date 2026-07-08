/******************************************************************************
 * VDaT
 *
 * File:
 *    imu.cpp
 *
 * Purpose:
 *    BNO085 IMU — Game Rotation Vector mode, zero calibration
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#include <Wire.h>
#include <math.h>
#include <Preferences.h>

#include "SparkFun_BNO080_Arduino_Library.h"

#include "config.h"
#include "globals.h"
#include "imu.h"
#include "utils.h"

// Last raw (pre-offset) reading - needed so calibrateZero() can compute
// a fresh offset without waiting for the next sensor update.
static float rawPitch = 0.0f;
static float rawRoll  = 0.0f;

// Tracks the last time the sensor actually delivered data. If this goes
// stale for more than IMU_TIMEOUT_MS, we flag the sensor as no longer
// responding — catches a wire working loose mid-session, not just a
// dead sensor at boot.
static unsigned long lastResponseMillis = 0;
static const unsigned long IMU_TIMEOUT_MS = 1000;

// v1.1.0 — INT is a hint to poll more responsively, not a hard gate.
// If INT is ever miswired, this fallback timer means the sensor still
// gets checked at least every POLL_FALLBACK_MS regardless — behavior
// degrades gracefully back to the original always-poll approach rather
// than silently breaking.
static unsigned long lastIntCheckMillis = 0;
static const unsigned long POLL_FALLBACK_MS = 100;

//--------------------------------------------------
// resetIMU() — v1.1.0
//
// Hardware reset of the BNO085 chip itself via its RST pin — resets
// just the sensor, not the whole ESP32. Pulled low briefly, then
// released. The sensor needs a short settling delay afterward before
// it will respond on I2C again.
//--------------------------------------------------

void resetIMU()
{
    pinMode(IMU_RST_PIN, OUTPUT);
    digitalWrite(IMU_RST_PIN, LOW);
    delay(10);
    digitalWrite(IMU_RST_PIN, HIGH);
    delay(200); // sensor boot/settling time after reset
}

bool initIMU()
{
    pinMode(IMU_RST_PIN, OUTPUT);
    digitalWrite(IMU_RST_PIN, HIGH); // idle high — reset line is active-low

    pinMode(IMU_INT_PIN, INPUT_PULLUP); // BNO085 pulls this low when data is ready

    if (!myIMU.begin(IMU_ADDRESS, Wire))
    {
        // First attempt failed — try one hardware reset and retry before
        // giving up entirely. A stuck sensor from a previous session is
        // exactly the case this recovers from; a genuinely disconnected
        // or wrong-address sensor will still fail here too, same as before.
        Serial.println("IMU init failed — attempting hardware reset and retry...");
        resetIMU();

        if (!myIMU.begin(IMU_ADDRESS, Wire))
        {
            Serial.println("IMU init failed after reset retry");
            return false;
        }

        Serial.println("IMU recovered after hardware reset");
    }

    Wire.setClock(400000);
    myIMU.enableGameRotationVector(50);

    // Load any saved calibration offset from flash
    prefs.begin("vdat", true);
    pitchOffset = prefs.getFloat("pitchOffset", 0.0f);
    rollOffset  = prefs.getFloat("rollOffset",  0.0f);
    prefs.end();

    Serial.print("Loaded pitchOffset: "); Serial.println(pitchOffset);
    Serial.print("Loaded rollOffset: ");  Serial.println(rollOffset);

    Serial.println("IMU initialized");
    return true;
}

bool updateIMU()
{
    // v1.1.0 — rolling 1-second activity window for diagnostics. These
    // counters run continuously regardless of the INT hint logic below,
    // so Diagnostics always has a fresh, real picture of sensor
    // behavior without needing to actively block and sample.
    static unsigned long windowStart = millis();
    static int windowIntCount  = 0;
    static int windowDataCount = 0;

    // v1.1.0 — INT hint: if the sensor hasn't signaled data-ready AND
    // we're still within the fallback window, skip this cycle's check.
    // This only makes polling MORE responsive when INT is wired
    // correctly — it never blocks a check for longer than
    // POLL_FALLBACK_MS, so a miswired or unconnected INT pin just
    // means every check happens on the timer, same as before this
    // feature existed.
    bool intAsserted = (digitalRead(IMU_INT_PIN) == LOW);
    if (intAsserted) windowIntCount++;

    bool fallbackDue = (millis() - lastIntCheckMillis >= POLL_FALLBACK_MS);

    bool skippedThisCycle = (!intAsserted && !fallbackDue);
    if (!skippedThisCycle)
    {
        lastIntCheckMillis = millis();
    }

    if (millis() - windowStart >= 1000)
    {
        imuIntPulsesPerSec  = windowIntCount;
        imuDataReadsPerSec  = windowDataCount;
        windowIntCount  = 0;
        windowDataCount = 0;
        windowStart = millis();
    }

    if (skippedThisCycle)
    {
        return false;
    }

    if (myIMU.dataAvailable())
    {
        windowDataCount++;
        imuResponding = true;
        lastResponseMillis = millis();

        // Swap axes to match physical mounting orientation
        float sensorPitch = myIMU.getRoll()  * 180.0f / PI;
        float sensorRoll  = myIMU.getPitch() * 180.0f / PI;

        // User-selectable rig-relative orientation (Settings > Calibration).
        // A = standard, B = rotated 180°, C = rotated 90° CW, D = rotated 90° CCW.
        // Covers all four ways a flat board can be mounted rotated around
        // the vertical (yaw) axis — the realistic mounting cases for a
        // small IMU screwed or glued flat onto a chassis plate.
        switch (imuOrientation)
        {
            case 1: // B - 180°
                rawPitch = -sensorPitch;
                rawRoll  = -sensorRoll;
                break;
            case 2: // C - 90° CW
                rawPitch = sensorRoll;
                rawRoll  = -sensorPitch;
                break;
            case 3: // D - 90° CCW
                rawPitch = -sensorRoll;
                rawRoll  = sensorPitch;
                break;
            default: // A - Standard
                rawPitch = sensorPitch;
                rawRoll  = sensorRoll;
                break;
        }

        g_pitch = rawPitch - pitchOffset;
        g_roll  = rawRoll  - rollOffset;

        g_yaw = (myIMU.getYaw() * 180.0f / PI) + HDG_OFFSET;

        while (g_yaw < 0.0f)    g_yaw += 360.0f;
        while (g_yaw >= 360.0f) g_yaw -= 360.0f;

        g_compass = getCompassDir(g_yaw);

        // BNO085 reports per-sensor accuracy (0-3) via the quaternion
        // accuracy field exposed by the SparkFun library as getQuatAccuracy().
        // 0 = Unreliable, 1 = Low, 2 = Medium, 3 = High
        // Note: under Game Rotation Vector mode this reflects gyro/accel
        // fusion confidence only - no magnetometer is used, so this
        // value will NOT be affected by nearby motors, servos, or metal.
        imuCalStatus = myIMU.getQuatAccuracy();

#if DEBUG_IMU_SERIAL
        // Raw sensor timing/values, bypassing WiFi entirely. A steady,
        // small gap (roughly 20ms at 50Hz reporting) is healthy. A large
        // or irregular gap here means the stutter is in the sensor/I2C
        // path, not the network — if this stays smooth while the phone
        // dashboard still stutters, the problem is proven to be WiFi/browser.
        static unsigned long lastDebugMillis = 0;
        unsigned long now = millis();
        unsigned long gap = now - lastDebugMillis;
        lastDebugMillis = now;

        Serial.print("t=");
        Serial.print(now);
        Serial.print("  gap=");
        Serial.print(gap);
        Serial.print("ms  pitch=");
        Serial.print(g_pitch, 2);
        Serial.print("  roll=");
        Serial.println(g_roll, 2);
#endif

        return true;
    }

    // No new data this cycle — only flip to "not responding" after a
    // sustained gap. The BNO085 won't have fresh data every single loop
    // iteration even when healthy, so a single miss isn't a fault.
    if (imuResponding && (millis() - lastResponseMillis > IMU_TIMEOUT_MS))
    {
        imuResponding = false;
        Serial.println("WARNING: IMU stopped responding.");
    }

    return false;
}

//--------------------------------------------------
// calibrateZero()
//
// Captures the current raw orientation as the new zero reference.
// Whatever angle the truck is sitting at right now becomes 0,0.
// Saves the offset to flash so it survives a reboot.
//--------------------------------------------------

void calibrateZero()
{
    pitchOffset = rawPitch;
    rollOffset  = rawRoll;

    prefs.begin("vdat", false);
    prefs.putFloat("pitchOffset", pitchOffset);
    prefs.putFloat("rollOffset",  rollOffset);
    prefs.end();

    Serial.println("IMU zero calibration set:");
    Serial.print("  pitchOffset: "); Serial.println(pitchOffset);
    Serial.print("  rollOffset: ");  Serial.println(rollOffset);
}

//--------------------------------------------------
// resetCalibration()
//
// Clears the stored offset, returning to raw sensor readings.
//--------------------------------------------------

void resetCalibration()
{
    pitchOffset = 0.0f;
    rollOffset  = 0.0f;

    prefs.begin("vdat", false);
    prefs.putFloat("pitchOffset", 0.0f);
    prefs.putFloat("rollOffset",  0.0f);
    prefs.end();

    Serial.println("IMU calibration reset to 0,0");
}
