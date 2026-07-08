/******************************************************************************
 * VDaT
 *
 * File:
 *    diagnostics.h
 *
 * Purpose:
 *    I2C bus health checks — the real-world equivalent of "is this pin
 *    connected." I2C lines are pulled high and float indistinguishably
 *    from idle when disconnected, so per-pin testing isn't meaningful.
 *    What IS meaningful: does anything respond on the bus at all, and
 *    at the expected address.
 *
 ******************************************************************************/
#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <Arduino.h>

// Returns a JSON string describing both I2C buses: pins, whether any
// device responded, which address(es) if so, and whether that matches
// the expected device for that bus.
String getDiagnosticsJSON();

#endif
