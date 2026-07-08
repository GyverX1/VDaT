/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_calibration.h
 *
 * Purpose:
 *    Calibration page — live readout, zero, reset
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef JS_CALIBRATION_H
#define JS_CALIBRATION_H

#include <Arduino.h>

const char JS_CALIBRATION[] PROGMEM = R"rawliteral(

    let lastKnownOrientation = 0;

    function setOrientation(value) {
      fetch('/orientation?value=' + value, { method: 'POST' }).catch(() => {
        alert('Could not reach the rig to save that setting.');
      });
    }

    // Called every fetch cycle from fetchData() to keep the
    // Calibration page's live readout and accuracy dot current,
    // even while the settings overlay is open.
    function updateCalibrationDisplay(pitch, roll, calStatus) {
      const pEl = document.getElementById('calLivePitch');
      const rEl = document.getElementById('calLiveRoll');
      if (pEl) pEl.textContent = (pitch >= 0 ? '+' : '') + pitch.toFixed(1) + '\u00B0';
      if (rEl) rEl.textContent = (roll  >= 0 ? '+' : '') + roll.toFixed(1)  + '\u00B0';

      const dot  = document.getElementById('calStatusDot');
      const text = document.getElementById('calStatusText');
      if (!dot || !text) return;

      dot.classList.remove('cal-unreliable', 'cal-low', 'cal-medium', 'cal-high');

      switch (calStatus) {
        case 3:
          dot.classList.add('cal-high');
          text.textContent = 'High';
          break;
        case 2:
          dot.classList.add('cal-medium');
          text.textContent = 'Medium';
          break;
        case 1:
          dot.classList.add('cal-low');
          text.textContent = 'Low - move the rig a bit';
          break;
        default:
          dot.classList.add('cal-unreliable');
          text.textContent = 'Unreliable - move the rig around';
          break;
      }
    }

    function doZeroCalibration() {
      fetch('/calibrate?action=zero')
        .then(res => res.json())
        .then(d => {
          const msg = document.getElementById('calActionStatus');
          if (d.status === 'calibrated') {
            msg.style.color = '#0f0';
            msg.textContent = 'Zeroed! Current position is now 0,0.';
          } else {
            msg.style.color = '#f00';
            msg.textContent = 'Zero failed.';
          }
          setTimeout(() => { msg.textContent = ''; }, 3000);
        })
        .catch(() => {
          const msg = document.getElementById('calActionStatus');
          msg.style.color = '#f00';
          msg.textContent = 'Zero failed.';
        });
    }

    function confirmResetCalibration() {
      if (confirm('Reset calibration to raw sensor output? This clears your saved zero offset.')) {
        doResetCalibration();
      }
    }

    function doResetCalibration() {
      fetch('/calibrate?action=reset')
        .then(res => res.json())
        .then(d => {
          const msg = document.getElementById('calActionStatus');
          if (d.status === 'reset') {
            msg.style.color = '#0f0';
            msg.textContent = 'Calibration reset to raw.';
          } else {
            msg.style.color = '#f00';
            msg.textContent = 'Reset failed.';
          }
          setTimeout(() => { msg.textContent = ''; }, 3000);
        })
        .catch(() => {
          const msg = document.getElementById('calActionStatus');
          msg.style.color = '#f00';
          msg.textContent = 'Reset failed.';
        });
    }

)rawliteral";

#endif
