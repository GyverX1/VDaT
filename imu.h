/******************************************************************************
 * VDaT
 *
 * File:
 *    imu.h
 *
 * Purpose:
 *    BNO085 IMU interface declarations
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef IMU_H
#define IMU_H

#include <Arduino.h>

bool initIMU();

bool updateIMU();

// Hardware reset of the BNO085 via its RST pin (v1.1.0) — resets just
// the sensor, not the whole ESP32. Called automatically as a recovery
// attempt in initIMU() if the sensor fails to respond on first try.
void resetIMU();

// Zero-out calibration - sets current orientation as the new 0,0 reference
void calibrateZero();

// Clears stored offset, returns to raw sensor readings
void resetCalibration();

#endif
