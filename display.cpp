/******************************************************************************
 * VDaT
 *
 * File:
 *    display.cpp
 *
 * Purpose:
 *    SSD1306 OLED rendering — telemetry display
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "display.h"
#include "globals.h"
#include "config.h"

bool initDisplay()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
    {
        Serial.println("OLED init failed");
        return false;
    }

    display.clearDisplay();
    display.setRotation(0);
    display.setTextColor(SSD1306_WHITE);

    return true;
}

void drawSegmentBar(float pitch)
{
    const int segments = 9;

    int lit = constrain(map(abs((int)pitch), 0, 60, 0, segments), 0, segments);

    for (int i = 0; i < segments; i++)
    {
        int x = 30 + (i * 10);

        if (i < lit)
            display.fillRect(x, 3, 8, 8, SSD1306_WHITE);
        else
            display.drawRect(x, 3, 8, 8, SSD1306_WHITE);
    }
}

void updateDisplay()
{
    display.clearDisplay();

    //--------------------------------------------------
    // Title
    //--------------------------------------------------

    display.setTextSize(2);
    display.setCursor(0,0);
    display.print("VD");

    //--------------------------------------------------
    // Warning Bar
    //--------------------------------------------------

    drawSegmentBar(g_pitch);

    //--------------------------------------------------
    // Large Pitch Number
    //--------------------------------------------------

    char pitchStr[8];
    sprintf(pitchStr,"%d",abs((int)g_pitch));

    display.setTextSize(3);

    int16_t x1,y1;
    uint16_t w,h;

    display.getTextBounds(pitchStr,0,0,&x1,&y1,&w,&h);

    display.setCursor(128-w,18);
    display.print(pitchStr);

    //--------------------------------------------------
    // Direction
    //--------------------------------------------------

    display.setTextSize(1);

    if(g_pitch > 2)
    {
        display.setCursor(104,48);
        display.print("UP");
    }
    else if(g_pitch < -2)
    {
        display.setCursor(96,48);
        display.print("DOWN");
    }
    else
    {
        display.setCursor(96,48);
        display.print("LEVEL");
    }

    //--------------------------------------------------
    // Pitch
    //--------------------------------------------------

    display.setCursor(0,22);
    display.print("Pitch: ");

    if(g_pitch >= 0)
        display.print("+");

    display.print(g_pitch,1);

    //--------------------------------------------------
    // Roll
    //--------------------------------------------------

    display.setCursor(0,36);
    display.print("Roll : ");

    if(g_roll >= 0)
        display.print("+");

    display.print(g_roll,1);

    display.display();
}