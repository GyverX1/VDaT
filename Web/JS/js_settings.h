/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_settings.h
 *
 * Purpose:
 *    Threshold settings save logic
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef JS_SETTINGS_H
#define JS_SETTINGS_H

#include <Arduino.h>

const char JS_SETTINGS[] PROGMEM = R"rawliteral(

    function saveSettings() {
      const pu  = parseFloat(document.getElementById('inputPitchUp').value);
      const pd  = parseFloat(document.getElementById('inputPitchDown').value);
      const rr  = parseFloat(document.getElementById('inputRollRight').value);
      const rl  = parseFloat(document.getElementById('inputRollLeft').value);
      const pcy = parseFloat(document.getElementById('inputPitchColorYellow').value);
      const pcr = parseFloat(document.getElementById('inputPitchColorRed').value);
      const rcy = parseFloat(document.getElementById('inputRollColorYellow').value);
      const rcr = parseFloat(document.getElementById('inputRollColorRed').value);

      if ([pu, pd, rr, rl, pcy, pcr, rcy, rcr].some(v => isNaN(v) || v <= 0)) {
        document.getElementById('saveStatus').textContent = 'Invalid values';
        return;
      }

      if (pcy >= pcr) {
        document.getElementById('saveStatus').textContent = 'Pitch: Yellow must be less than Red';
        return;
      }

      if (rcy >= rcr) {
        document.getElementById('saveStatus').textContent = 'Roll: Yellow must be less than Red';
        return;
      }

      fetch('/save?warnPitchUp=' + pu +
                 '&warnPitchDown=' + pd +
                 '&warnRollRight=' + rr +
                 '&warnRollLeft='  + rl +
                 '&pitchColorYellow=' + pcy +
                 '&pitchColorRed='    + pcr +
                 '&rollColorYellow='  + rcy +
                 '&rollColorRed='     + rcr)
        .then(res => res.json())
        .then(d => {
          if (d.status === 'saved') {
            warnPitchUp   = pu;
            warnPitchDown = pd;
            warnRollRight = rr;
            warnRollLeft  = rl;
            pitchColorYellow = pcy;
            pitchColorRed    = pcr;
            rollColorYellow  = rcy;
            rollColorRed     = rcr;
            document.getElementById('saveStatus').textContent = 'Saved!';
            setTimeout(() => {
              document.getElementById('saveStatus').textContent = '';
            }, 2000);
          }
        })
        .catch(() => {
          document.getElementById('saveStatus').textContent = 'Save failed';
        });
    }

)rawliteral";

#endif
