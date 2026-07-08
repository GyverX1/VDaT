/******************************************************************************
 * VDaT
 *
 * File:
 *    serial_sync.h
 *
 * Purpose:
 *    Line-based command protocol over USB serial, used by VDaT Studio to
 *    list/download/delete rig-stored logs without WiFi. Every response is
 *    wrapped in @@BEGIN/@@END markers so the PC-side parser can safely
 *    ignore any stray Serial.println() debug output interleaved on the
 *    same stream, rather than requiring every print site in the codebase
 *    to be suppressed during a sync session.
 *
 *    Commands (each a single line, newline-terminated):
 *      SYNC:PING                 -> @@BEGIN:PONG ... @@END:PONG
 *      SYNC:LIST                 -> @@BEGIN:LIST <json> @@END:LIST
 *      SYNC:GET:<filename>       -> @@BEGIN:FILE:<filename>:<size> <raw bytes> @@END:FILE
 *      SYNC:DELETE:<filename>    -> @@BEGIN:DELETE:<ok|err> @@END:DELETE
 *
 ******************************************************************************/
#ifndef SERIAL_SYNC_H
#define SERIAL_SYNC_H

void checkSerialSync(); // call every loop() iteration — non-blocking

#endif
