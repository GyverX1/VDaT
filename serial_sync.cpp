/******************************************************************************
 * VDaT
 *
 * File:
 *    serial_sync.cpp
 *
 ******************************************************************************/
#include <LittleFS.h>

#include "serial_sync.h"
#include "riglog.h"
#include "config.h"

static String inputBuffer = "";

static bool isSafeFilename(const String &name)
{
    // No path traversal — a filename is just a filename, never a path.
    return name.length() > 0 && name.indexOf('/') < 0 && name.indexOf("..") < 0;
}

static void handleSyncPing()
{
    Serial.println("@@BEGIN:PONG");
    Serial.println(PROJECT_NAME);
    Serial.println(PROJECT_VERSION);
    Serial.println("@@END:PONG");
}

static void handleSyncList()
{
    Serial.println("@@BEGIN:LIST");
    Serial.println(listRigLogsJSON());
    Serial.println("@@END:LIST");
}

static void handleSyncGet(const String &filename)
{
    if (!isSafeFilename(filename))
    {
        Serial.println("@@BEGIN:FILE:ERR:0");
        Serial.println("@@END:FILE");
        return;
    }

    String fullPath = String(RIG_LOG_DIR) + "/" + filename;
    if (!LittleFS.exists(fullPath))
    {
        Serial.println("@@BEGIN:FILE:ERR:0");
        Serial.println("@@END:FILE");
        return;
    }

    File f = LittleFS.open(fullPath, "r");
    size_t size = f.size();

    // Header line carries the exact byte count so Studio knows precisely
    // how much to read before expecting the @@END:FILE marker — this is
    // what lets the reader distinguish "end of file data" from "the
    // marker string happened to appear inside the CSV," even though in
    // practice VDaT's own CSV format would never contain it.
    Serial.println("@@BEGIN:FILE:" + filename + ":" + String(size));

    // Stream raw bytes directly — no line-based encoding here, since
    // this is exactly the same CSV bytes as the file on disk.
    uint8_t buf[256];
    while (f.available())
    {
        size_t n = f.read(buf, sizeof(buf));
        Serial.write(buf, n);
    }
    f.close();

    Serial.println();
    Serial.println("@@END:FILE");
}

static void handleSyncDelete(const String &filename)
{
    bool ok = isSafeFilename(filename) && deleteRigLog(filename);
    Serial.println("@@BEGIN:DELETE:" + String(ok ? "ok" : "err"));
    Serial.println("@@END:DELETE");
}

void checkSerialSync()
{
    while (Serial.available())
    {
        char c = Serial.read();

        if (c == '\n')
        {
            inputBuffer.trim();

            if (inputBuffer == "SYNC:PING")
            {
                handleSyncPing();
            }
            else if (inputBuffer == "SYNC:LIST")
            {
                handleSyncList();
            }
            else if (inputBuffer.startsWith("SYNC:GET:"))
            {
                handleSyncGet(inputBuffer.substring(9));
            }
            else if (inputBuffer.startsWith("SYNC:DELETE:"))
            {
                handleSyncDelete(inputBuffer.substring(12));
            }
            // Anything else on serial (or partial garbage) is silently
            // ignored — this is also what makes the protocol tolerant
            // of a human typing into Serial Monitor by mistake.

            inputBuffer = "";
        }
        else if (c != '\r')
        {
            inputBuffer += c;
        }
    }
}
