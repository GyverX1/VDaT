/******************************************************************************
 * VDaT
 *
 * File:
 *    riglog.h
 *
 * Purpose:
 *    Rig-side (on-device) session logging via LittleFS. Independent of
 *    phone-side logging — the ESP has no built-in concept of "a session
 *    is happening" otherwise, so the phone must explicitly tell it to
 *    start/stop via the endpoints in web.cpp.
 *
 ******************************************************************************/
#ifndef RIGLOG_H
#define RIGLOG_H

#include <Arduino.h>

bool initRigLog();                 // mounts LittleFS, creates /logs if missing
void setRigLogEnabled(bool enabled);
bool startRigLogSession();         // opens a new file, writes CSV header
void appendRigLogRow();            // called from the main loop when a session is active
void stopRigLogSession();          // closes the current file

String listRigLogsJSON();          // JSON array of {name, size, modified-ish}
bool deleteRigLog(const String &filename);

extern bool rigLogEnabled;         // persisted preference — feature on/off
extern bool rigLogSessionActive;   // is a session currently being written

#endif
