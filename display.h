/******************************************************************************
 * VDaT
 *
 * File:
 *    display.h
 *
 * Purpose:
 *    OLED display interface declarations
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

bool initDisplay();

void updateDisplay();

void drawSegmentBar(float pitch);

#endif
