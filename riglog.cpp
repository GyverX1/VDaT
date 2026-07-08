/******************************************************************************
 * VDaT
 *
 * File:
 *    riglog.cpp
 *
 ******************************************************************************/
#include <LittleFS.h>
#include <Preferences.h>

#include "riglog.h"
#include "globals.h"
#include "config.h"

extern Preferences prefs;

bool rigLogEnabled       = RIG_LOG_ENABLED_DEFAULT;
bool rigLogSessionActive = false;

static File rigLogFile;
static unsigned long rigLogSessionStartMillis = 0;

bool initRigLog()
{
    if (!LittleFS.begin(true)) // true = format if mount fails (first boot on new partition table)
    {
        Serial.println("LittleFS mount failed.");
        return false;
    }

    if (!LittleFS.exists(RIG_LOG_DIR))
    {
        LittleFS.mkdir(RIG_LOG_DIR);
    }

    prefs.begin("vdat", false);
    rigLogEnabled = prefs.getBool("rigLogEn", RIG_LOG_ENABLED_DEFAULT);
    prefs.end();

    Serial.printf("LittleFS mounted. Rig logging: %s\n", rigLogEnabled ? "ON" : "OFF");
    return true;
}

void setRigLogEnabled(bool enabled)
{
    rigLogEnabled = enabled;
    prefs.begin("vdat", false);
    prefs.putBool("rigLogEn", enabled);
    prefs.end();
}

bool startRigLogSession()
{
    if (!rigLogEnabled) return false;
    if (rigLogSessionActive) stopRigLogSession(); // safety: close any stray open session first

    // Filename from millis() since the ESP has no RTC — not human-readable,
    // but unique and monotonically increasing, which is all that matters
    // for "list newest first."
    String filename = String(RIG_LOG_DIR) + "/log_" + String(millis()) + ".csv";

    rigLogFile = LittleFS.open(filename, "w");
    if (!rigLogFile)
    {
        Serial.println("Failed to open rig log file for writing.");
        return false;
    }

    rigLogFile.println("# rig_name," + String(rigName));
    rigLogFile.println("# source,rig");
    rigLogFile.println(String("# version,") + PROJECT_VERSION);
    rigLogFile.println("# warn_pitch_up," + String(warnPitchUp, 1));
    rigLogFile.println("# warn_pitch_down," + String(warnPitchDown, 1));
    rigLogFile.println("# warn_roll_right," + String(warnRollRight, 1));
    rigLogFile.println("# warn_roll_left," + String(warnRollLeft, 1));
    rigLogFile.println("time_s,pitch_deg,roll_deg,heading_deg,warn,warn_pitch_up,warn_pitch_down,warn_roll_right,warn_roll_left");

    rigLogSessionActive = true;
    rigLogSessionStartMillis = millis();

    Serial.println("Rig log session started: " + filename);
    return true;
}

void appendRigLogRow()
{
    if (!rigLogSessionActive || !rigLogFile) return;

    float t = (millis() - rigLogSessionStartMillis) / 1000.0f;

    bool wPitchUp   = g_pitch > 0 && g_pitch >= warnPitchUp;
    bool wPitchDown = g_pitch < 0 && fabs(g_pitch) >= warnPitchDown;
    bool wRollRight = g_roll  > 0 && g_roll  >= warnRollRight;
    bool wRollLeft  = g_roll  < 0 && fabs(g_roll)  >= warnRollLeft;
    bool warn = wPitchUp || wPitchDown || wRollRight || wRollLeft;

    rigLogFile.printf("%.2f,%.1f,%.1f,%.0f,%d,%d,%d,%d,%d\n",
        t, g_pitch, g_roll, g_yaw,
        warn ? 1 : 0, wPitchUp ? 1 : 0, wPitchDown ? 1 : 0, wRollRight ? 1 : 0, wRollLeft ? 1 : 0);
}

void stopRigLogSession()
{
    if (!rigLogSessionActive) return;

    if (rigLogFile) rigLogFile.close();
    rigLogSessionActive = false;

    Serial.println("Rig log session stopped.");
}

String listRigLogsJSON()
{
    String json = "[";
    bool first = true;

    File dir = LittleFS.open(RIG_LOG_DIR);
    File entry = dir.openNextFile();
    while (entry)
    {
        if (!entry.isDirectory())
        {
            if (!first) json += ",";
            first = false;

            String name = String(entry.name());
            // entry.name() may or may not include the directory prefix
            // depending on core version — strip it if present, for a
            // clean filename in the response.
            int slashIdx = name.lastIndexOf('/');
            if (slashIdx >= 0) name = name.substring(slashIdx + 1);

            json += "{\"name\":\"" + name + "\",\"size\":" + String(entry.size()) + "}";
        }
        entry = dir.openNextFile();
    }

    json += "]";
    return json;
}

bool deleteRigLog(const String &filename)
{
    // Reject anything that isn't a plain filename — no path traversal
    // via a crafted "../../something" in the delete request.
    if (filename.indexOf('/') >= 0 || filename.indexOf("..") >= 0) return false;

    String fullPath = String(RIG_LOG_DIR) + "/" + filename;
    if (!LittleFS.exists(fullPath)) return false;

    return LittleFS.remove(fullPath);
}
