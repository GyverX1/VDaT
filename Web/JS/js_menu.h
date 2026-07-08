/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_menu.h
 *
 * Purpose:
 *    Settings menu navigation — open, close, showScreen
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef JS_MENU_H
#define JS_MENU_H

#include <Arduino.h>

const char JS_MENU[] PROGMEM = R"rawliteral(

    function openSettings() {
      document.getElementById('settingsOverlay').classList.add('open');
      showScreen('screenMenu');
    }

    function closeSettings() {
      document.getElementById('settingsOverlay').classList.remove('open');
    }

    function showScreen(screenId) {
      const screens = document.querySelectorAll('.settings-screen');
      screens.forEach(s => s.classList.add('hidden'));

      const target = document.getElementById(screenId);
      if (target) target.classList.remove('hidden');

      if (screenId === 'screenLogging' && typeof buildLogSummary === 'function') {
        buildLogSummary();
        const toggle = document.getElementById('autoSaveToggle');
        if (toggle) toggle.checked = autoSaveEnabled;
        if (typeof refreshRigLogState === 'function') refreshRigLogState();
      }

      if (screenId === 'screenCalibration') {
        const orientSelect = document.getElementById('orientationSelect');
        if (orientSelect) orientSelect.value = lastKnownOrientation;
      }

      if (screenId === 'screenRigName' && typeof initRigNameScreen === 'function') {
        initRigNameScreen();
      }
    }

)rawliteral";

#endif
