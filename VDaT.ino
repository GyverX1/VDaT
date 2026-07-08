/******************************************************************************
 * VDaT
 *
 * File:
 *    VDaT.ino
 *
 * Purpose:
 *    Main sketch entry point
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "globals.h"

#include "imu.h"
#include "display.h"
#include "boot_anim.h"
#include "web.h"
#include "riglog.h"
#include "serial_sync.h"
#include "utils.h"

void setup()
{
    Serial.begin(115200);
    delay(200);

    Wire.begin(SDA_PIN, SCL_PIN);           // Bus 1: BNO085
    Wire1.begin(OLED_SDA_PIN, OLED_SCL_PIN); // Bus 2: OLED

    Serial.println();
    Serial.println("==============================");
    Serial.println(PROJECT_NAME);
    Serial.println(PROJECT_VERSION);
    Serial.println("==============================");

    oledConnected = initDisplay();
    if (oledConnected) showBootAnimation();   // flag animation before IMU/WiFi init
    imuConnected = initIMU();
    initRigLog();
    initWebServer();
}

void loop()
{
    server.handleClient();
    checkSerialSync();

    if (updateIMU())
    {
        if (oledConnected) updateDisplay();
        appendRigLogRow(); // no-op internally if no session is active
    }
}
