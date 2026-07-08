/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_fetch.h
 *
 * Purpose:
 *    Main fetch loop — polls /data, drives all dashboard updates
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef JS_FETCH_H
#define JS_FETCH_H

#include <Arduino.h>

const char JS_FETCH[] PROGMEM = R"rawliteral(

    function setHudWarn(id, active, text) {
      const el = document.getElementById(id);
      if (!el) return;
      if (active) {
        el.textContent = text;
        el.classList.add('active');
      } else {
        el.classList.remove('active');
      }
    }

    function setDiagCard(prefix, bus) {
      const pinsEl   = document.getElementById('diag' + prefix + 'Pins');
      const statusEl = document.getElementById('diag' + prefix + 'Status');
      const cardEl   = document.getElementById('diag' + prefix + 'Card');

      pinsEl.textContent = 'SDA: GP' + bus.sda + '  /  SCL: GP' + bus.scl;

      if (bus.ok) {
        statusEl.textContent = '\u2705 Connected — found at ' + bus.expectedAddr;
        cardEl.classList.remove('diag-fail');
        cardEl.classList.add('diag-ok');
      } else if (bus.found.length > 0) {
        statusEl.textContent = '\u26A0 Wrong device — found ' + bus.found +
          ', expected ' + bus.expectedAddr + '. Check for address conflicts.';
        cardEl.classList.remove('diag-ok');
        cardEl.classList.add('diag-fail');
      } else {
        statusEl.textContent = '\u274C No device found — check wiring on GP' +
          bus.sda + ' / GP' + bus.scl + ' (SDA/SCL), plus power and ground.';
        cardEl.classList.remove('diag-ok');
        cardEl.classList.add('diag-fail');
      }
    }

    function setIntActivityCard(intActivity) {
      const pinsEl   = document.getElementById('diagIntPins');
      const statusEl = document.getElementById('diagIntStatus');
      const cardEl   = document.getElementById('diagIntCard');

      pinsEl.textContent = 'Pin: GP' + intActivity.pin;

      const pulses = intActivity.pulsesPerSec;
      const reads  = intActivity.dataReadsPerSec;

      if (pulses === 0) {
        statusEl.textContent = '\u274C Not toggling — check wiring on GP' + intActivity.pin + '.';
        cardEl.classList.remove('diag-ok');
        cardEl.classList.add('diag-fail');
      } else if (reads === 0) {
        // INT is active but no successful reads followed — this points
        // at the I2C data path (SDA/SCL), not the INT pin itself.
        statusEl.textContent = '\u26A0 Signaling (' + pulses + '/sec), but no data reads succeeded — check SDA/SCL wiring instead.';
        cardEl.classList.remove('diag-ok');
        cardEl.classList.add('diag-fail');
      } else {
        statusEl.textContent = '\u2705 Healthy — ' + pulses + ' signals/sec, ' + reads + ' reads/sec.';
        cardEl.classList.remove('diag-fail');
        cardEl.classList.add('diag-ok');
      }
    }

    function confirmTestImuReset() {
      if (!confirm('This will briefly reset the IMU. Live readings will interrupt for a moment. Continue?')) return;

      const statusEl = document.getElementById('diagResetStatus');
      statusEl.textContent = 'Resetting...';

      fetch('/diagnostics/resetimu', { method: 'POST' })
        .then(r => r.json())
        .then(d => {
          statusEl.textContent = d.recovered
            ? '\u2705 Sensor responded after reset.'
            : '\u274C Sensor did not respond after reset — check wiring.';
        })
        .catch(() => {
          statusEl.textContent = 'Could not reach the rig to run the test.';
        });
    }

    function runDiagnostics() {
      document.getElementById('diagImuStatus').textContent = 'Scanning...';
      document.getElementById('diagOledStatus').textContent = 'Scanning...';
      document.getElementById('diagIntStatus').textContent = 'Scanning...';

      fetch('/diagnostics')
        .then(r => r.json())
        .then(d => {
          setDiagCard('Imu', d.imuBus);
          setDiagCard('Oled', d.oledBus);
          setIntActivityCard(d.intActivity);
        })
        .catch(() => {
          document.getElementById('diagImuStatus').textContent = 'Scan failed — connection lost';
          document.getElementById('diagOledStatus').textContent = 'Scan failed — connection lost';
          document.getElementById('diagIntStatus').textContent = 'Scan failed — connection lost';
        });
    }

    let fetchInFlight = false;

    function fetchData() {
      // Guard against overlapping requests. If the previous fetch hasn't
      // resolved yet, skip this cycle rather than starting a second
      // request — two in-flight requests can resolve out of order,
      // which is what caused stale/glitched values to render.
      if (fetchInFlight) return;
      fetchInFlight = true;

      fetch('/data')
        .then(r => r.json())
        .then(d => {

          // --- Hardware connection check — runs before anything else touches ---
          // --- pitch/roll, so a disconnected sensor never gets silently ------
          // --- rendered as a valid "0.0°" reading. -----------------------------
          if (!d.imuConnected) {
            document.getElementById('pitch').textContent = '--';
            document.getElementById('roll').textContent  = '--';
            document.getElementById('status').textContent =
              '\u26A0 IMU DISCONNECTED — check BNO085 wiring (GP12/GP13)';
            document.getElementById('status').classList.add('status-error');
            reportFetchSuccess(); // network is fine — this is a sensor problem, not connectivity
            return;
          }

          if (!d.imuResponding) {
            document.getElementById('status').textContent =
              '\u26A0 IMU NOT RESPONDING — check connection';
            document.getElementById('status').classList.add('status-error');
            reportFetchSuccess(); // same — network reached the device fine
            return;
          }

          document.getElementById('status').classList.remove('status-error');

          const pitch   = d.pitch;
          const roll    = d.roll;
          const heading = d.heading;
          lastKnownOrientation = d.imuOrientation;

          if (!settingsLoaded) {
            warnPitchUp   = d.warnPitchUp;
            warnPitchDown = d.warnPitchDown;
            warnRollRight = d.warnRollRight;
            warnRollLeft  = d.warnRollLeft;
            pitchColorYellow = d.pitchColorYellow;
            pitchColorRed    = d.pitchColorRed;
            rollColorYellow  = d.rollColorYellow;
            rollColorRed     = d.rollColorRed;

            const setPitch    = document.getElementById('inputPitchUp');
            const setPitchD   = document.getElementById('inputPitchDown');
            const setRollR    = document.getElementById('inputRollRight');
            const setRollL    = document.getElementById('inputRollLeft');
            const setPCY      = document.getElementById('inputPitchColorYellow');
            const setPCR      = document.getElementById('inputPitchColorRed');
            const setRCY      = document.getElementById('inputRollColorYellow');
            const setRCR      = document.getElementById('inputRollColorRed');

            if (setPitch)  setPitch.value  = warnPitchUp;
            if (setPitchD) setPitchD.value = warnPitchDown;
            if (setRollR)  setRollR.value  = warnRollRight;
            if (setRollL)  setRollL.value  = warnRollLeft;
            if (setPCY)    setPCY.value    = pitchColorYellow;
            if (setPCR)    setPCR.value    = pitchColorRed;
            if (setRCY)    setRCY.value    = rollColorYellow;
            if (setRCR)    setRCR.value    = rollColorRed;

            settingsLoaded = true;

            // Update header with stored rig name
            if (d.rigName) {
              const h1 = document.querySelector('.hud-header h1');
              if (h1) h1.textContent = d.rigName;
            }
          }

          const pitchUpWarn   = pitch > 0 && pitch          > warnPitchUp;
          const pitchDownWarn = pitch < 0 && Math.abs(pitch) > warnPitchDown;
          const rollRightWarn = roll  > 0 && roll            > warnRollRight;
          const rollLeftWarn  = roll  < 0 && Math.abs(roll)  > warnRollLeft;

          const pitchWarn = pitchUpWarn || pitchDownWarn;
          const rollWarn  = rollRightWarn || rollLeftWarn;

          // Display values
          document.getElementById('pitch').textContent =
            (pitch >= 0 ? '+' : '') + pitch + '\u00B0';
          document.getElementById('roll').textContent =
            (roll >= 0 ? '+' : '') + roll + '\u00B0';

          // Attitude indicator
          drawAttitudeIndicator(pitch, roll, heading);

          // Calibration page live readout (cheap no-op if page isn't open)
          updateCalibrationDisplay(pitch, roll, d.imuCalStatus);

          // Session logging (no-op if not currently logging)
          recordLogPoint(pitch, roll, heading, pitchUpWarn, pitchDownWarn, rollRightWarn, rollLeftWarn);

          // Bars
          updateBar('pitchBar', pitch, pitchWarn, 'pitchCard', 'pitch');
          updateBar('rollBar',  roll,  rollWarn,  'rollCard',  'roll');

          // HUD header warnings
          setHudWarn('warnPitchUp',   pitchUpWarn,   '\u26A0 PITCH ' + pitch.toFixed(1) + '\u00B0 \u25B2');
          setHudWarn('warnPitchDown', pitchDownWarn, '\u26A0 PITCH ' + pitch.toFixed(1) + '\u00B0 \u25BC');
          setHudWarn('warnRollLeft',  rollLeftWarn,  '\u25C4 ROLL '  + Math.abs(roll).toFixed(1) + '\u00B0 \u26A0');
          setHudWarn('warnRollRight', rollRightWarn, '\u26A0 ROLL '  + roll.toFixed(1) + '\u00B0 \u25BA');

          // Audio beeps
          handleBeep('pitch', pitchWarn, PITCH_HZ);
          handleBeep('roll',  rollWarn,  ROLL_HZ);

          // Peak tracking
          if (pitch > 0 && pitch > peakPitchUp) {
            peakPitchUp = pitch;
            document.getElementById('peakPitchUp').textContent = '+' + pitch.toFixed(1) + '\u00B0';
          }
          if (pitch < 0 && Math.abs(pitch) > peakPitchDown) {
            peakPitchDown = Math.abs(pitch);
            document.getElementById('peakPitchDown').textContent = '-' + peakPitchDown.toFixed(1) + '\u00B0';
          }
          if (roll > 0 && roll > peakRollRight) {
            peakRollRight = roll;
            document.getElementById('peakRollRight').textContent = '+' + roll.toFixed(1) + '\u00B0';
          }
          if (roll < 0 && Math.abs(roll) > peakRollLeft) {
            peakRollLeft = Math.abs(roll);
            document.getElementById('peakRollLeft').textContent = '-' + peakRollLeft.toFixed(1) + '\u00B0';
          }

          document.getElementById('status').textContent =
            'Updated: ' + new Date().toLocaleTimeString();

          reportFetchSuccess();
        })
        .catch(() => {
          document.getElementById('status').textContent = 'No data';
          reportFetchFailure();
        })
        .finally(() => {
          fetchInFlight = false;
        });
    }

)rawliteral";

#endif
