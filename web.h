/******************************************************************************
 * VDaT
 *
 * File:
 *    web.h
 *
 * Purpose:
 *    Web server interface declarations
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef WEB_H
#define WEB_H

void initWebServer();
void handleRoot();
void handleData();
void handleSaveSettings();
void handleCalibrate();
void handleSetName();
void handleVersion();
void handleDiagnostics();
void handleRigLogStart();
void handleRigLogStop();
void handleRigLogToggle();
void handleRigLogList();
void handleRigLogDownload();
void handleRigLogDelete();
void handleSetOrientation();
void handleTestImuReset();
void handleManifest();
void handleServiceWorker();
void handleIcon192();
void handleIcon512();

#endif
