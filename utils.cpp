/******************************************************************************
 * VDaT
 *
 * File:
 *    utils.cpp
 *
 * Purpose:
 *    Utility functions — compass direction string
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#include "utils.h"

const char* getCompassDir(float heading)
{
    if (heading >= 337.5 || heading < 22.5)  return "N";
    if (heading < 67.5)                      return "NE";
    if (heading < 112.5)                     return "E";
    if (heading < 157.5)                     return "SE";
    if (heading < 202.5)                     return "S";
    if (heading < 247.5)                     return "SW";
    if (heading < 292.5)                     return "W";
    return "NW";
}