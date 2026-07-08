/******************************************************************************
 * VDaT
 *
 * File:
 *    diagnostics.cpp
 *
 * Purpose:
 *    I2C bus scanning for build-time wiring verification.
 *
 ******************************************************************************/
#include <Wire.h>

#include "diagnostics.h"
#include "config.h"
#include "globals.h"

//--------------------------------------------------
// scanBus()
//
// Sweeps every address 0x01-0x7E on the given bus, returning a
// comma-separated hex list of addresses that ACK'd. Empty string
// means nothing responded — the bus is effectively silent, which is
// the wiring-fault signal we care about.
//--------------------------------------------------

static String scanBus(TwoWire &bus)
{
    String found = "";

    for (uint8_t addr = 1; addr < 0x7F; addr++)
    {
        bus.beginTransmission(addr);
        uint8_t result = bus.endTransmission();

        if (result == 0) // 0 = ACK received, device present
        {
            if (found.length() > 0) found += ",";
            found += "0x" + String(addr, HEX);
        }
    }

    return found;
}

//--------------------------------------------------
// getDiagnosticsJSON()
//
// Reports both buses independently, plus a plain-language "expected
// vs found" verdict so the dashboard doesn't have to know the address
// numbers to explain the result.
//--------------------------------------------------

String getDiagnosticsJSON()
{
    String imuFound  = scanBus(Wire);
    String oledFound  = scanBus(Wire1);

    char imuAddrHex[8];
    sprintf(imuAddrHex, "0x%x", IMU_ADDRESS);
    char oledAddrHex[8];
    sprintf(oledAddrHex, "0x%x", OLED_ADDRESS);

    bool imuOk  = imuFound.indexOf(imuAddrHex) != -1;
    bool oledOk = oledFound.indexOf(oledAddrHex) != -1;

    String json = "{";

    json += "\"imuBus\":{";
    json += "\"sda\":" + String(SDA_PIN) + ",";
    json += "\"scl\":" + String(SCL_PIN) + ",";
    json += "\"expectedAddr\":\"" + String(imuAddrHex) + "\",";
    json += "\"found\":\"" + imuFound + "\",";
    json += "\"ok\":" + String(imuOk ? "true" : "false");
    json += "},";

    json += "\"oledBus\":{";
    json += "\"sda\":" + String(OLED_SDA_PIN) + ",";
    json += "\"scl\":" + String(OLED_SCL_PIN) + ",";
    json += "\"expectedAddr\":\"" + String(oledAddrHex) + "\",";
    json += "\"found\":\"" + oledFound + "\",";
    json += "\"ok\":" + String(oledOk ? "true" : "false");
    json += "},";

    json += "\"intActivity\":{";
    json += "\"pin\":" + String(IMU_INT_PIN) + ",";
    json += "\"pulsesPerSec\":" + String(imuIntPulsesPerSec) + ",";
    json += "\"dataReadsPerSec\":" + String(imuDataReadsPerSec);
    json += "}";

    json += "}";

    return json;
}
