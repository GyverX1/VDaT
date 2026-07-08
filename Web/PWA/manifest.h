/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/PWA/manifest.h
 *
 * Purpose:
 *    Web App Manifest — tells the phone's browser this page can be
 *    installed to the home screen, and how it should look/behave
 *    when launched that way (name, icons, standalone display mode).
 *
 ******************************************************************************/
#ifndef MANIFEST_H
#define MANIFEST_H

#include <Arduino.h>

const char PWA_MANIFEST[] PROGMEM = R"rawliteral(
{
  "name": "VDaT",
  "short_name": "VDaT",
  "description": "Crawler Data Acquisition telemetry dashboard",
  "start_url": "/",
  "display": "standalone",
  "background_color": "#000000",
  "theme_color": "#000000",
  "orientation": "portrait",
  "icons": [
    {
      "src": "/icon-192.png",
      "sizes": "192x192",
      "type": "image/png",
      "purpose": "any maskable"
    },
    {
      "src": "/icon-512.png",
      "sizes": "512x512",
      "type": "image/png",
      "purpose": "any maskable"
    }
  ]
}
)rawliteral";

#endif
