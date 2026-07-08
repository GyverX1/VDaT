/******************************************************************************
 * VDaT
 *
 * File:
 *    boot_anim.cpp
 *
 * Purpose:
 *    Animated American flag boot sequence with fade-out
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
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "boot_anim.h"
#include "globals.h"
#include "config.h"
#include "vdat_logo_bitmap.h"

//--------------------------------------------------
// Timing
//--------------------------------------------------

#define FRAME_MS      80      // ms per animation frame (~12 fps)
#define FLAG_FRAMES   65      // frames before fade (~5.2 seconds)
#define WAVE_AMP      3       // max pixel vertical displacement
#define WAVE_PERIOD   40      // horizontal wavelength in pixels

//--------------------------------------------------
// Flag geometry
//--------------------------------------------------

#define FLAG_W        128
#define FLAG_H        52      // flag occupies top 52 rows; status bar below

#define STRIPE_H      4       // each stripe is 4px tall (13 stripes = 52px)
#define CANTON_W      32      // canton width
#define CANTON_H      28      // canton height (covers top 7 stripes)

//--------------------------------------------------
// Star positions (5 rows x 6 cols + 4 rows x 5 cols = 50 stars)
// Coordinates are relative to canton top-left (0,0)
// Arranged in alternating rows of 6 and 5 to approximate the US flag
//--------------------------------------------------

static const uint8_t STAR_X[] = {
  3, 8, 13, 18, 23, 28,   // row 0 - 6 stars
  5, 10, 15, 20, 25,       // row 1 - 5 stars
  3, 8, 13, 18, 23, 28,   // row 2 - 6 stars
  5, 10, 15, 20, 25,       // row 3 - 5 stars
  3, 8, 13, 18, 23, 28,   // row 4 - 6 stars
  5, 10, 15, 20, 25,       // row 5 - 5 stars
  3, 8, 13, 18, 23, 28,   // row 6 - 6 stars
  5, 10, 15, 20, 25,       // row 7 - 5 stars
  3, 8, 13, 18, 23, 28,   // row 8 - 6 stars
};

static const uint8_t STAR_Y[] = {
  2, 2, 2, 2, 2, 2,
  5, 5, 5, 5, 5,
  8, 8, 8, 8, 8, 8,
  11, 11, 11, 11, 11,
  14, 14, 14, 14, 14, 14,
  17, 17, 17, 17, 17,
  20, 20, 20, 20, 20, 20,
  23, 23, 23, 23, 23,
  26, 26, 26, 26, 26, 26,
};

#define NUM_STARS  (sizeof(STAR_X) / sizeof(STAR_X[0]))

//--------------------------------------------------
// drawFlag()
//
// Draws one frame of the flag animation.
// phase: advances each frame to scroll the wave.
//--------------------------------------------------

static void drawFlag(int phase)
{
    display.clearDisplay();

    //--------------------------------------------------
    // Draw stripes with sine wave warp
    // Odd stripes (0,2,4...) = white (filled)
    // Even stripes (1,3,5...) = black (empty) — natural gap
    //--------------------------------------------------

    for (int stripe = 0; stripe < 13; stripe++)
    {
        int stripeY = stripe * STRIPE_H;
        bool white = (stripe % 2 == 0);

        if (!white) continue; // black stripes are just the background

        for (int x = 0; x < FLAG_W; x++)
        {
            // Wave offset: varies by x and advances with phase
            float rad = ((float)(x + phase) / WAVE_PERIOD) * 2.0f * PI;
            int waveY = (int)(sin(rad) * WAVE_AMP);

            int y0 = stripeY + waveY;
            int y1 = y0 + STRIPE_H - 1;

            // Clamp to flag area
            if (y1 < 0 || y0 >= FLAG_H) continue;
            if (y0 < 0) y0 = 0;
            if (y1 >= FLAG_H) y1 = FLAG_H - 1;

            // Draw this column of the stripe
            display.drawFastVLine(x, y0, y1 - y0 + 1, SSD1306_WHITE);
        }
    }

    //--------------------------------------------------
    // Canton background (dithered to suggest blue)
    // Checkerboard pattern over the canton area
    //--------------------------------------------------

    for (int y = 0; y < CANTON_H; y++)
    {
        for (int x = 0; x < CANTON_W; x++)
        {
            // Checkerboard: dark (sparse) to differentiate from white stripes
            if ((x + y) % 2 == 0)
                display.drawPixel(x, y, SSD1306_BLACK);
            else
                display.drawPixel(x, y, SSD1306_WHITE);
        }
    }

    //--------------------------------------------------
    // Stars - static, drawn on top of canton
    //--------------------------------------------------

    for (uint8_t i = 0; i < NUM_STARS; i++)
    {
        int sx = STAR_X[i];
        int sy = STAR_Y[i];

        if (sx >= CANTON_W || sy >= CANTON_H) continue;

        // 3x3 plus-sign star shape
        display.drawPixel(sx,     sy,     SSD1306_WHITE);
        display.drawPixel(sx - 1, sy,     SSD1306_WHITE);
        display.drawPixel(sx + 1, sy,     SSD1306_WHITE);
        display.drawPixel(sx,     sy - 1, SSD1306_WHITE);
        display.drawPixel(sx,     sy + 1, SSD1306_WHITE);
    }

    //--------------------------------------------------
    // Rig name label below the flag
    //--------------------------------------------------

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(44, 55);
    display.print(rigName);

    display.display();
}

//--------------------------------------------------
// fadeOut()
//
// Wipes the display by blanking rows top-to-bottom.
//--------------------------------------------------

static void fadeOut()
{
    for (int y = 0; y < 64; y += 2)
    {
        display.fillRect(0, y, 128, 2, SSD1306_BLACK);
        display.display();
        delay(18);
    }
    display.clearDisplay();
    display.display();
    delay(200);
}

//--------------------------------------------------
// showLogo()
//
// Displays the real VDaT logo (converted from the brand PNG) plus
// version number. This is the boot sequence's final resting frame
// before normal telemetry display takes over.
//--------------------------------------------------

static void showLogo()
{
    display.clearDisplay();

    //--------------------------------------------------
    // Dual-color OLED layout note:
    // This panel is a physical two-zone display — rows 0-15 are
    // yellow, rows 16-63 are blue. Color is NOT software-selectable;
    // a pixel's color is determined purely by which row it lands on.
    //
    // So we work WITH the seam instead of straddling it:
    //   - version string sits in the yellow band (top)  -> renders yellow
    //   - wordmark sits entirely in the blue band       -> renders blue
    // A small safety margin below the seam keeps the wordmark fully
    // blue even if a given panel's boundary is off by a row or two.
    //--------------------------------------------------

    #define OLED_SEAM_Y     16      // yellow/blue boundary row
    #define SEAM_MARGIN     2       // keep wordmark this far below the seam

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // --- Version, centered in the yellow band ---
    char verStr[24];
    sprintf(verStr, "v%s", PROJECT_VERSION);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(verStr, 0, 0, &x1, &y1, &w, &h);
    int verY = (OLED_SEAM_Y - h) / 2;          // vertically center in yellow zone
    if (verY < 0) verY = 0;
    display.setCursor((128 - w) / 2, verY);
    display.print(verStr);

    // --- Wordmark, centered within the blue band ---
    int blueTop = OLED_SEAM_Y + SEAM_MARGIN;   // first safely-blue row
    int logoX = (128 - VDAT_LOGO_WIDTH) / 2;
    int logoY = blueTop + ((64 - blueTop) - VDAT_LOGO_HEIGHT) / 2;
    display.drawBitmap(logoX, logoY, VDAT_LOGO_BITMAP,
                       VDAT_LOGO_WIDTH, VDAT_LOGO_HEIGHT, SSD1306_WHITE);

    display.display();
    delay(1800);
}

//--------------------------------------------------
// showBootAnimation()
//--------------------------------------------------

void showBootAnimation()
{
    int phase = 0;

    for (int frame = 0; frame < FLAG_FRAMES; frame++)
    {
        drawFlag(phase);
        phase += 2;   // advance wave — lower = slower scroll
        if (phase >= WAVE_PERIOD * 10) phase = 0;
        delay(FRAME_MS);
    }

    fadeOut();
    showLogo();
}
