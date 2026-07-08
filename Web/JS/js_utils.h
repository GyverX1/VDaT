/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_utils.h
 *
 * Purpose:
 *    JS globals, session timer, resetPeaks, updateBar
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef JS_UTILS_H
#define JS_UTILS_H
#include <Arduino.h>

const char JS_UTILS[] PROGMEM = R"rawliteral(
	let peakPitchUp = 0;
    let peakPitchDown = 0;
    let peakRollRight = 0;
    let peakRollLeft = 0;

    let warnPitchUp = 35;
    let warnPitchDown = 35;
    let warnRollRight = 35;
    let warnRollLeft = 35;

    let pitchColorYellow = 15;
    let pitchColorRed = 30;
    let rollColorYellow = 15;
    let rollColorRed = 30;

    let settingsLoaded = false;

    const startTime = Date.now();

    setInterval(() => {
      const elapsed = Math.floor((Date.now() - startTime) / 1000);
      const h = String(Math.floor(elapsed / 3600)).padStart(2, '0');
      const m = String(Math.floor((elapsed % 3600) / 60)).padStart(2, '0');
      const s = String(elapsed % 60).padStart(2, '0');
      document.getElementById('timer').textContent = h + ':' + m + ':' + s;
    }, 1000);

    function resetPeaks() {
      peakPitchUp = 0;
      peakPitchDown = 0;
      peakRollRight = 0;
      peakRollLeft = 0;

      document.getElementById('peakPitchUp').textContent = '0.0\u00B0';
      document.getElementById('peakPitchDown').textContent = '0.0\u00B0';
      document.getElementById('peakRollRight').textContent = '0.0\u00B0';
      document.getElementById('peakRollLeft').textContent = '0.0\u00B0';
    }

    // axis: 'pitch' or 'roll' - selects which color threshold pair to use
    function updateBar(barId, value, warn, cardId, axis) {
      const bar = document.getElementById(barId);
      const maxDeg = 45;
      const clamped = Math.max(-maxDeg, Math.min(maxDeg, value));
      const pct = (Math.abs(clamped) / maxDeg) * 50;

      const absVal = Math.abs(clamped);

      const yellowAt = (axis === 'roll') ? rollColorYellow : pitchColorYellow;
      const redAt    = (axis === 'roll') ? rollColorRed    : pitchColorRed;

      let color = '#fff';
      if (absVal > redAt) color = '#f00';
      else if (absVal > yellowAt) color = '#ff0';

      bar.style.background = color;
      bar.style.width = pct + '%';
      bar.style.left = clamped >= 0 ? '50%' : (50 - pct) + '%';

      if (cardId) {
        const card = document.getElementById(cardId);
        if (card) {
          if (warn) card.classList.add('card-warn');
          else card.classList.remove('card-warn');
        }
      }
    }

)rawliteral";

#endif
