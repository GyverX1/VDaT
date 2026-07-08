/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_storage.h
 *
 * Purpose:
 *    IndexedDB session storage. Solves the original problem this whole
 *    PWA effort was for: a plain browser tab has no access to the
 *    phone's file system and loses everything in JS memory on refresh
 *    or network switch. IndexedDB persists locally in the browser,
 *    survives both, and has no practical size ceiling like localStorage.
 *
 *    Each stored session carries the same metadata already used in the
 *    CSV header (rig name, date/time, thresholds) plus the full CSV
 *    text itself, so a session can be re-downloaded or re-summarized
 *    later without needing the original live data array.
 *
 ******************************************************************************/
#ifndef JS_STORAGE_H
#define JS_STORAGE_H

#include <Arduino.h>

const char JS_STORAGE[] PROGMEM = R"rawliteral(

    const VDAT_DB_NAME    = 'vdat_sessions';
    const VDaT_DB_VERSION = 1;
    const VDAT_STORE_NAME = 'sessions';

    // A signature written into every session this app creates. Used to
    // verify ownership before ever loading/rendering a CSV — anything
    // without this marker is refused rather than parsed.
    const VDAT_CSV_SIGNATURE = '# rig_name,';

    let _vdatDB = null;

    function openVDaTDB() {
      return new Promise((resolve, reject) => {
        if (_vdatDB) { resolve(_vdatDB); return; }

        const req = indexedDB.open(VDAT_DB_NAME, VDaT_DB_VERSION);

        req.onupgradeneeded = (event) => {
          const db = event.target.result;
          if (!db.objectStoreNames.contains(VDAT_STORE_NAME)) {
            const store = db.createObjectStore(VDAT_STORE_NAME, {
              keyPath: 'id',
              autoIncrement: true
            });
            store.createIndex('timestamp', 'timestamp', { unique: false });
          }
        };

        req.onsuccess = (event) => {
          _vdatDB = event.target.result;
          resolve(_vdatDB);
        };

        req.onerror = () => reject(req.error);
      });
    }

    // session = { rigName, date, time, duration, warnCount, csv, timestamp }
    function saveSessionToDB(session) {
      return openVDaTDB().then((db) => {
        return new Promise((resolve, reject) => {
          const tx    = db.transaction(VDAT_STORE_NAME, 'readwrite');
          const store = tx.objectStore(VDAT_STORE_NAME);
          const req   = store.add(session);
          req.onsuccess = () => resolve(req.result);
          req.onerror   = () => reject(req.error);
        });
      });
    }

    function getAllSessions() {
      return openVDaTDB().then((db) => {
        return new Promise((resolve, reject) => {
          const tx    = db.transaction(VDAT_STORE_NAME, 'readonly');
          const store = tx.objectStore(VDAT_STORE_NAME);
          const req   = store.getAll();
          req.onsuccess = () => {
            // Newest first
            const sessions = req.result.sort((a, b) => b.timestamp - a.timestamp);
            resolve(sessions);
          };
          req.onerror = () => reject(req.error);
        });
      });
    }

    function getSessionFromDB(id) {
      return openVDaTDB().then((db) => {
        return new Promise((resolve, reject) => {
          const tx    = db.transaction(VDAT_STORE_NAME, 'readonly');
          const store = tx.objectStore(VDAT_STORE_NAME);
          const req   = store.get(id);
          req.onsuccess = () => resolve(req.result);
          req.onerror   = () => reject(req.error);
        });
      });
    }

    function deleteSessionFromDB(id) {
      return openVDaTDB().then((db) => {
        return new Promise((resolve, reject) => {
          const tx    = db.transaction(VDAT_STORE_NAME, 'readwrite');
          const store = tx.objectStore(VDAT_STORE_NAME);
          const req   = store.delete(id);
          req.onsuccess = () => resolve();
          req.onerror   = () => reject(req.error);
        });
      });
    }

    // Ownership check — refuses anything that isn't a VDaT-generated CSV,
    // rather than attempting to parse and display whatever a mystery
    // file might contain.
    function isValidVDaTCsv(text) {
      return typeof text === 'string' && text.trimStart().startsWith(VDAT_CSV_SIGNATURE);
    }

)rawliteral";

#endif
