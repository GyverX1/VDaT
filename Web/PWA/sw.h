/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/PWA/sw.h
 *
 * Purpose:
 *    Service worker. Required for a page to be installable as a PWA —
 *    without one, "Add to Home Screen" either isn't offered or just
 *    creates a bookmark shortcut rather than a real standalone app.
 *
 *    Caches the app shell (the dashboard page, icons) on first load so
 *    that a brief WiFi hiccup or AP roam shows the cached shell instead
 *    of a browser error page. Live telemetry (/data) is intentionally
 *    NOT cached — that data is only meaningful live, from the device.
 *
 ******************************************************************************/
#ifndef SW_H
#define SW_H

#include <Arduino.h>

const char SERVICE_WORKER_JS[] PROGMEM = R"rawliteral(
const CACHE_NAME = 'vdat-shell-v1';
const SHELL_ASSETS = [
  '/',
  '/manifest.json',
  '/icon-192.png',
  '/icon-512.png'
];

self.addEventListener('install', (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME).then((cache) => cache.addAll(SHELL_ASSETS))
  );
  self.skipWaiting();
});

self.addEventListener('activate', (event) => {
  event.waitUntil(
    caches.keys().then((names) =>
      Promise.all(
        names
          .filter((name) => name !== CACHE_NAME)
          .map((name) => caches.delete(name))
      )
    )
  );
  self.clients.claim();
});

self.addEventListener('fetch', (event) => {
  const url = new URL(event.request.url);

  // Never cache live telemetry or diagnostics — those are only
  // meaningful fresh, from a currently-connected device.
  if (url.pathname === '/data' || url.pathname === '/diagnostics') {
    return;
  }

  event.respondWith(
    fetch(event.request)
      .then((response) => {
        // Update the cached shell copy whenever a live fetch succeeds
        const responseClone = response.clone();
        caches.open(CACHE_NAME).then((cache) => {
          cache.put(event.request, responseClone);
        });
        return response;
      })
      .catch(() => caches.match(event.request))
  );
});
)rawliteral";

#endif
