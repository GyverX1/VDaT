/******************************************************************************
 * VDaT
 *
 * File:
 *    web.cpp
 *
 * Purpose:
 *    WiFi AP, HTTP server, all endpoint handlers
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#include "web.h"
#include "globals.h"
#include "config.h"
#include "dashboard.h"
#include "imu.h"
#include "diagnostics.h"
#include "riglog.h"
#include <LittleFS.h>
#include "Web/PWA/manifest.h"
#include "Web/PWA/sw.h"
#include "Web/PWA/icon_192.h"
#include "Web/PWA/icon_512.h"

extern WebServer server;
extern Preferences prefs;

//--------------------------------------------------
// Root Page - chunked send to avoid heap fragmentation
//--------------------------------------------------

void handleRoot()
{
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent_P(HTML_HEADER);
    server.sendContent_P(HTML_CARDS);

    server.sendContent(
        "<div class=\"main-actions\">\r\n"
        "<button class=\"btn\" onclick=\"resetPeaks()\">RESET PEAKS</button>\r\n"
        "<button class=\"btn\" id=\"logBtn\" onclick=\"toggleLogging()\">&#9210; START LOG</button>\r\n"
        "<button class=\"btn\" id=\"muteBtn\" onclick=\"toggleMute()\">&#128266; SOUND ON</button>\r\n"
        "</div>\r\n"
        "<div class=\"log-rec-indicator\" id=\"logRecIndicator\"></div>\r\n"
        "<button class=\"settings-btn\" onclick=\"openSettings()\">&#9881; SETTINGS</button>\r\n"
    );

    server.sendContent_P(HTML_MENU);

    server.sendContent(
        "<div class=\"about-banner\" id=\"aboutBanner\">\r\n"
        "<span class=\"about-banner-text\">&#9888; DID YOU READ THE ABOUT PAGE?</span>\r\n"
        "<button class=\"about-banner-btn\" onclick=\"dismissAboutBanner()\">YES, I'VE READ IT</button>\r\n"
        "</div>\r\n"
        "<div class=\"status\" id=\"status\">Connecting...</div>\r\n"
        "<script>\r\n"
    );

    server.sendContent_P(JS_UTILS);
    server.sendContent_P(JS_AUDIO);
    server.sendContent_P(JS_ATTITUDE);
    server.sendContent_P(JS_SETTINGS);
    server.sendContent_P(JS_CALIBRATION);
    server.sendContent_P(JS_STORAGE);
    server.sendContent_P(JS_LOGGING);
    server.sendContent_P(JS_SAVED_SESSIONS);
    server.sendContent_P(JS_RIGNAME);
    server.sendContent_P(JS_MENU);
    server.sendContent_P(JS_FETCH);

    server.sendContent("fetchData();\r\nsetInterval(fetchData, 200);\r\n</script>\r\n");

    server.sendContent(
        "<script>\r\n"
        "if ('serviceWorker' in navigator) {\r\n"
        "  navigator.serviceWorker.register('/sw.js').catch(function(e) {\r\n"
        "    console.log('Service worker registration failed:', e);\r\n"
        "  });\r\n"
        "}\r\n"
        "</script>\r\n"
    );

    server.sendContent_P(HTML_FOOTER);

    server.sendContent("");  // signal end of chunked response
}

//--------------------------------------------------
// JSON Telemetry
//--------------------------------------------------

void handleData()
{
    static char json[512];

    snprintf(json, sizeof(json),
        "{"
        "\"roll\":%.1f,"
        "\"pitch\":%.1f,"
        "\"heading\":%.0f,"
        "\"compass\":\"%s\","
        "\"warnPitchUp\":%.1f,"
        "\"warnPitchDown\":%.1f,"
        "\"warnRollRight\":%.1f,"
        "\"warnRollLeft\":%.1f,"
        "\"pitchColorYellow\":%.1f,"
        "\"pitchColorRed\":%.1f,"
        "\"rollColorYellow\":%.1f,"
        "\"rollColorRed\":%.1f,"
        "\"imuCalStatus\":%u,"
        "\"imuOrientation\":%d,"
        "\"imuConnected\":%s,"
        "\"imuResponding\":%s,"
        "\"oledConnected\":%s,"
        "\"rigName\":\"%s\","
        "\"version\":\"%s\","
        "\"build\":\"%s\""
        "}",
        g_roll, g_pitch, g_yaw, g_compass,
        warnPitchUp, warnPitchDown, warnRollRight, warnRollLeft,
        pitchColorYellow, pitchColorRed, rollColorYellow, rollColorRed,
        imuCalStatus,
        imuOrientation,
        imuConnected  ? "true" : "false",
        imuResponding ? "true" : "false",
        oledConnected ? "true" : "false",
        rigName, PROJECT_VERSION, PROJECT_BUILD);

    server.send(200, "application/json", json);
}

//--------------------------------------------------
// Save Tilt + Color Threshold Settings
//--------------------------------------------------

void handleSaveSettings()
{
    if (server.hasArg("warnPitchUp")      &&
        server.hasArg("warnPitchDown")    &&
        server.hasArg("warnRollRight")    &&
        server.hasArg("warnRollLeft")     &&
        server.hasArg("pitchColorYellow") &&
        server.hasArg("pitchColorRed")    &&
        server.hasArg("rollColorYellow")  &&
        server.hasArg("rollColorRed"))
    {
        warnPitchUp      = server.arg("warnPitchUp").toFloat();
        warnPitchDown    = server.arg("warnPitchDown").toFloat();
        warnRollRight    = server.arg("warnRollRight").toFloat();
        warnRollLeft     = server.arg("warnRollLeft").toFloat();
        pitchColorYellow = server.arg("pitchColorYellow").toFloat();
        pitchColorRed    = server.arg("pitchColorRed").toFloat();
        rollColorYellow  = server.arg("rollColorYellow").toFloat();
        rollColorRed     = server.arg("rollColorRed").toFloat();

        prefs.begin("vdat", false);
        prefs.putFloat("warnPitchUp",   warnPitchUp);
        prefs.putFloat("warnPitchDown", warnPitchDown);
        prefs.putFloat("warnRollRight", warnRollRight);
        prefs.putFloat("warnRollLeft",  warnRollLeft);
        prefs.putFloat("pitchColorYel", pitchColorYellow);
        prefs.putFloat("pitchColorRed", pitchColorRed);
        prefs.putFloat("rollColorYel",  rollColorYellow);
        prefs.putFloat("rollColorRed",  rollColorRed);
        prefs.end();

        server.send(200, "application/json", "{\"status\":\"saved\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"status\":\"error\"}");
    }
}

//--------------------------------------------------
// IMU Calibration
//--------------------------------------------------

void handleCalibrate()
{
    if (server.hasArg("action"))
    {
        String action = server.arg("action");

        if (action == "zero")
        {
            calibrateZero();
            server.send(200, "application/json", "{\"status\":\"calibrated\"}");
        }
        else if (action == "reset")
        {
            resetCalibration();
            server.send(200, "application/json", "{\"status\":\"reset\"}");
        }
        else
        {
            server.send(400, "application/json", "{\"status\":\"error\"}");
        }
    }
    else
    {
        server.send(400, "application/json", "{\"status\":\"error\"}");
    }
}

//--------------------------------------------------
// Set Rig Name
//--------------------------------------------------

void handleSetName()
{
    if (server.hasArg("name"))
    {
        String name = server.arg("name");
        name.trim();

        // Strip spaces - rig name must be single token
        name.replace(" ", "");

        if (name.length() == 0 || name.length() > 10)
        {
            server.send(400, "application/json", "{\"status\":\"error\",\"msg\":\"Name must be 1-10 characters, no spaces\"}");
            return;
        }

        name.toCharArray(rigName, sizeof(rigName));

        prefs.begin("vdat", false);
        prefs.putString("rigName", rigName);
        prefs.end();

        Serial.print("Rig name set to: "); Serial.println(rigName);
        server.send(200, "application/json", "{\"status\":\"saved\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"status\":\"error\"}");
    }
}

//--------------------------------------------------
// Version
//--------------------------------------------------

void handleVersion()
{
    String json = "{";
    json += "\"name\":\"" + String(PROJECT_NAME) + "\",";
    json += "\"version\":\"" + String(PROJECT_VERSION) + "\",";
    json += "\"build\":\"" + String(PROJECT_BUILD) + "\"";
    json += "}";
    server.send(200, "application/json", json);
}

//--------------------------------------------------
// Diagnostics — I2C bus scan
//--------------------------------------------------

void handleDiagnostics()
{
    server.send(200, "application/json", getDiagnosticsJSON());
}

//--------------------------------------------------
// Rig-Side Logging
//--------------------------------------------------

void handleRigLogStart()
{
    bool ok = startRigLogSession();
    server.send(ok ? 200 : 409, "text/plain", ok ? "Started" : "Not enabled or already running");
}

void handleRigLogStop()
{
    stopRigLogSession();
    server.send(200, "text/plain", "Stopped");
}

void handleRigLogToggle()
{
    if (!server.hasArg("enabled"))
    {
        server.send(400, "text/plain", "Missing 'enabled' arg");
        return;
    }
    setRigLogEnabled(server.arg("enabled") == "true");
    server.send(200, "text/plain", rigLogEnabled ? "Rig logging ON" : "Rig logging OFF");
}

void handleRigLogList()
{
    String json = "{\"enabled\":" + String(rigLogEnabled ? "true" : "false") +
                  ",\"active\":" + String(rigLogSessionActive ? "true" : "false") +
                  ",\"files\":" + listRigLogsJSON() + "}";
    server.send(200, "application/json", json);
}

void handleRigLogDownload()
{
    if (!server.hasArg("file"))
    {
        server.send(400, "text/plain", "Missing 'file' arg");
        return;
    }

    String filename = server.arg("file");
    if (filename.indexOf('/') >= 0 || filename.indexOf("..") >= 0)
    {
        server.send(400, "text/plain", "Invalid filename");
        return;
    }

    String fullPath = String(RIG_LOG_DIR) + "/" + filename;
    if (!LittleFS.exists(fullPath))
    {
        server.send(404, "text/plain", "Not found");
        return;
    }

    File f = LittleFS.open(fullPath, "r");
    server.streamFile(f, "text/csv");
    f.close();
}

void handleRigLogDelete()
{
    if (!server.hasArg("file"))
    {
        server.send(400, "text/plain", "Missing 'file' arg");
        return;
    }
    bool ok = deleteRigLog(server.arg("file"));
    server.send(ok ? 200 : 404, "text/plain", ok ? "Deleted" : "Not found");
}

//--------------------------------------------------
// IMU Orientation
//--------------------------------------------------

void handleSetOrientation()
{
    if (!server.hasArg("value"))
    {
        server.send(400, "text/plain", "Missing 'value' arg");
        return;
    }

    int val = server.arg("value").toInt();
    if (val < 0 || val > 3)
    {
        server.send(400, "text/plain", "Value must be 0-3");
        return;
    }

    imuOrientation = val;
    prefs.begin("vdat", false);
    prefs.putInt("imuOrient", imuOrientation);
    prefs.end();

    server.send(200, "text/plain", "OK");
}

//--------------------------------------------------
// IMU Reset Test (v1.1.0)
//
// Deliberately disruptive — this genuinely resets the sensor mid-
// session. Only reachable via an explicit button click with its own
// confirm prompt on the dashboard, not part of the passive scan.
//--------------------------------------------------

void handleTestImuReset()
{
    resetIMU();

    bool recovered = myIMU.begin(IMU_ADDRESS, Wire);

    if (recovered)
    {
        Wire.setClock(400000);
        myIMU.enableGameRotationVector(50);
    }

    String json = "{\"recovered\":" + String(recovered ? "true" : "false") + "}";
    server.send(200, "application/json", json);
}

//--------------------------------------------------
// PWA — manifest, service worker, icons
//--------------------------------------------------

void handleManifest()
{
    server.send_P(200, "application/json", PWA_MANIFEST);
}

void handleServiceWorker()
{
    // Service workers must be served with this MIME type and, ideally,
    // from the root scope — both satisfied here since it's registered
    // at /sw.js directly.
    server.send_P(200, "application/javascript", SERVICE_WORKER_JS);
}

void handleIcon192()
{
    server.send_P(200, "image/png", (const char *)ICON_192_PNG, ICON_192_PNG_LEN);
}

void handleIcon512()
{
    server.send_P(200, "image/png", (const char *)ICON_512_PNG, ICON_512_PNG_LEN);
}

//--------------------------------------------------
// Initialize Web Server
//--------------------------------------------------

void initWebServer()
{
    prefs.begin("vdat", true);
    imuOrientation   = prefs.getInt("imuOrient", 0);
    warnPitchUp      = prefs.getFloat("warnPitchUp",   35.0f);
    warnPitchDown    = prefs.getFloat("warnPitchDown",  35.0f);
    warnRollRight    = prefs.getFloat("warnRollRight",  35.0f);
    warnRollLeft     = prefs.getFloat("warnRollLeft",   35.0f);
    pitchColorYellow = prefs.getFloat("pitchColorYel", 15.0f);
    pitchColorRed    = prefs.getFloat("pitchColorRed", 30.0f);
    rollColorYellow  = prefs.getFloat("rollColorYel",  15.0f);
    rollColorRed     = prefs.getFloat("rollColorRed",  30.0f);
    {
        String savedName = prefs.getString("rigName", "VDaT");
        savedName.replace(" ", "");   // sanitize any legacy names with spaces
        if (savedName.length() == 0) savedName = "VDaT";
        savedName.toCharArray(rigName, sizeof(rigName));
    }
    prefs.end();

    Serial.print("Loaded RigName: "); Serial.println(rigName);

    WiFi.persistent(false);
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_AP);
    WiFi.setHostname(AP_SSID);
    delay(200);

    bool ok = WiFi.softAP(AP_SSID, AP_PASSWORD, 1, false, 4);
    delay(500);

    // Disable modem sleep. Default WiFi power-save periodically sleeps the
    // radio between beacon intervals — fine for idle IoT devices, but it
    // shows up as intermittent stalls/freezes for a phone actively polling
    // the dashboard every 100ms. This is a low-latency AP, not a low-power one.
    WiFi.setSleep(false);

    if (ok)
        Serial.println("Access Point Started");
    else
        Serial.println("Access Point FAILED");

    Serial.print("SSID: "); Serial.println(WiFi.softAPSSID());
    Serial.print("IP:   "); Serial.println(WiFi.softAPIP());

    server.on("/",          handleRoot);
    server.on("/data",      handleData);
    server.on("/save",      handleSaveSettings);
    server.on("/calibrate", handleCalibrate);
    server.on("/setname",   handleSetName);
    server.on("/version",   handleVersion);
    server.on("/diagnostics", handleDiagnostics);
    server.on("/riglog/start", HTTP_POST, handleRigLogStart);
    server.on("/riglog/stop", HTTP_POST, handleRigLogStop);
    server.on("/riglog/toggle", HTTP_POST, handleRigLogToggle);
    server.on("/riglogs", handleRigLogList);
    server.on("/riglogs/download", handleRigLogDownload);
    server.on("/riglogs/delete", HTTP_POST, handleRigLogDelete);
    server.on("/orientation", HTTP_POST, handleSetOrientation);
    server.on("/diagnostics/resetimu", HTTP_POST, handleTestImuReset);
    server.on("/manifest.json", handleManifest);
    server.on("/sw.js", handleServiceWorker);
    server.on("/icon-192.png", handleIcon192);
    server.on("/icon-512.png", handleIcon512);

    server.begin();
    Serial.println("Web Server Started");
}
