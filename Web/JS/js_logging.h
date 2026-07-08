/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_logging.h
 *
 * Purpose:
 *    Session logging — record, summary, chart, CSV export
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef JS_LOGGING_H
#define JS_LOGGING_H

#include <Arduino.h>

const char JS_LOGGING[] PROGMEM = R"rawliteral(

    let isLogging = false;
    let logData = [];
    let logStartTime = 0;
    let logStartDate = null;
    let lastLogData = [];
    let lastLogMeta = {};

    function toggleLogging() {
      if (isLogging) stopLogging();
      else startLogging();
    }

    let rigLogEnabledClient = false; // mirrors the ESP's own setting, refreshed on Logging screen open

    function startLogging() {
      isLogging = true;
      logData = [];
      logStartTime = Date.now();
      logStartDate = new Date();

      const btn = document.getElementById('logBtn');
      btn.textContent = '\u23F9 STOP LOG';
      btn.classList.add('btn-recording');

      const ind = document.getElementById('logRecIndicator');
      ind.classList.add('recording');

      if (rigLogEnabledClient) {
        ind.textContent = '\u25CF RECORDING (Phone + Rig)';
        fetch('/riglog/start', { method: 'POST' }).catch(() => {
          // Rig-side start failed silently from the user's perspective —
          // phone logging still proceeds either way, so this isn't fatal,
          // but worth surfacing rather than pretending it succeeded.
          ind.textContent = '\u25CF RECORDING (Phone only \u2014 rig log failed to start)';
        });
      } else {
        ind.textContent = '\u25CF RECORDING (Phone)';
      }
    }

    function stopLogging() {
      isLogging = false;

      const btn = document.getElementById('logBtn');
      btn.textContent = '\u23FA START LOG';
      btn.classList.remove('btn-recording');

      const ind = document.getElementById('logRecIndicator');
      ind.textContent = '';
      ind.classList.remove('recording');

      if (rigLogEnabledClient) {
        fetch('/riglog/stop', { method: 'POST' }).catch(() => {});
      }

      lastLogData = logData;

      // Capture session metadata at stop time
      const h1 = document.querySelector('.hud-header h1');
      lastLogMeta = {
        rigName:       h1 ? h1.textContent : 'VDaT',
        date:          logStartDate ? logStartDate.toLocaleDateString() : '',
        time:          logStartDate ? logStartDate.toLocaleTimeString() : '',
        warnPitchUp:   warnPitchUp,
        warnPitchDown: warnPitchDown,
        warnRollRight: warnRollRight,
        warnRollLeft:  warnRollLeft,
        version:       'VDaT Beta'
      };

      buildLogSummary();

      // Persist to IndexedDB every time — this is separate from the
      // optional Downloads-folder auto-save below. This is what makes
      // the session survive a refresh or network switch, and what
      // populates the Saved Sessions list / offline screen.
      if (lastLogData.length > 0) {
        const csv = buildCSVString(lastLogMeta, lastLogData);
        const duration = lastLogData[lastLogData.length - 1].t;
        let warnCount = 0, wasWarning = false;
        lastLogData.forEach(d => {
          if (d.warn && !wasWarning) warnCount++;
          wasWarning = d.warn;
        });

        saveSessionToDB({
          rigName:   lastLogMeta.rigName,
          date:      lastLogMeta.date,
          time:      lastLogMeta.time,
          duration:  duration,
          warnCount: warnCount,
          csv:       csv,
          timestamp: Date.now()
        }).then(() => {
          if (typeof refreshSavedSessionsList === 'function') refreshSavedSessionsList();
        }).catch((e) => {
          console.log('Session save failed:', e);
        });
      }

      if (autoSaveEnabled) {
        setTimeout(downloadLogCSV, 300);
      }
    }

    // Called every fetch cycle from fetchData()
    // Receives per-axis warning flags for richer data capture
    function recordLogPoint(pitch, roll, heading, pitchUpW, pitchDownW, rollRightW, rollLeftW) {
      if (!isLogging) return;
      const anyWarn = pitchUpW || pitchDownW || rollRightW || rollLeftW;
      logData.push({
        t:          (Date.now() - logStartTime) / 1000,
        pitch:      pitch,
        roll:       roll,
        heading:    heading,
        warn:       anyWarn,
        wPitchUp:   pitchUpW,
        wPitchDown: pitchDownW,
        wRollRight: rollRightW,
        wRollLeft:  rollLeftW
      });
    }

    //--------------------------------------------------
    // Summary + Chart
    //--------------------------------------------------

    function formatDuration(totalSeconds) {
      const m = Math.floor(totalSeconds / 60);
      const s = (totalSeconds % 60).toFixed(1);
      return m > 0 ? m + 'm ' + s.padStart(4, '0') + 's' : s + 's';
    }

    function buildLogSummary() {
      const emptyState     = document.getElementById('logEmptyState');
      const resultsSection = document.getElementById('logResultsSection');

      if (!lastLogData || lastLogData.length === 0) {
        if (emptyState) emptyState.classList.remove('hidden');
        if (resultsSection) resultsSection.classList.add('hidden');
        return;
      }

      if (emptyState) emptyState.classList.add('hidden');
      if (resultsSection) resultsSection.classList.remove('hidden');

      const duration = lastLogData[lastLogData.length - 1].t;
      const points   = lastLogData.length;

      let maxPitchUp = 0, maxPitchDown = 0, maxRollRight = 0, maxRollLeft = 0;
      let warnCount  = 0;
      let wasWarning = false;

      lastLogData.forEach(d => {
        if (d.pitch > 0 && d.pitch > maxPitchUp)          maxPitchUp   = d.pitch;
        if (d.pitch < 0 && Math.abs(d.pitch) > maxPitchDown) maxPitchDown = Math.abs(d.pitch);
        if (d.roll  > 0 && d.roll  > maxRollRight)         maxRollRight = d.roll;
        if (d.roll  < 0 && Math.abs(d.roll)  > maxRollLeft)  maxRollLeft  = Math.abs(d.roll);
        if (d.warn && !wasWarning) warnCount++;
        wasWarning = d.warn;
      });

      document.getElementById('logDuration').textContent      = formatDuration(duration);
      document.getElementById('logPoints').textContent        = points;
      document.getElementById('logMaxPitchUp').textContent    = '+' + maxPitchUp.toFixed(1) + '\u00B0';
      document.getElementById('logMaxPitchDown').textContent  = '-' + maxPitchDown.toFixed(1) + '\u00B0';
      document.getElementById('logMaxRollRight').textContent  = '+' + maxRollRight.toFixed(1) + '\u00B0';
      document.getElementById('logMaxRollLeft').textContent   = '-' + maxRollLeft.toFixed(1) + '\u00B0';
      document.getElementById('logWarnCount').textContent     = warnCount;

      drawLogChart();
    }

    function drawLogChart() {
      const canvas = document.getElementById('logChart');
      if (!canvas || !lastLogData || lastLogData.length === 0) return;

      const ctx = canvas.getContext('2d');
      const w = canvas.width, h = canvas.height;
      const padL = 30, padR = 10, padT = 10, padB = 20;
      const plotW = w - padL - padR;
      const plotH = h - padT - padB;
      const maxT  = lastLogData[lastLogData.length - 1].t || 1;
      const maxDeg = 90;
      const zeroY  = padT + plotH / 2;

      ctx.clearRect(0, 0, w, h);

      ctx.strokeStyle = '#333';
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(padL, zeroY);
      ctx.lineTo(w - padR, zeroY);
      ctx.stroke();

      ctx.fillStyle = '#666';
      ctx.font = '9px monospace';
      ctx.textAlign = 'right';
      ctx.fillText('+90', padL - 4, padT + 8);
      ctx.fillText('0',   padL - 4, zeroY + 3);
      ctx.fillText('-90', padL - 4, h - padB);

      function plotLine(key, color) {
        for (let i = 1; i < lastLogData.length; i++) {
          const prev = lastLogData[i - 1];
          const curr = lastLogData[i];
          const x1 = padL + (prev.t / maxT) * plotW;
          const y1 = padT + plotH / 2 - (prev[key] / maxDeg) * (plotH / 2);
          const x2 = padL + (curr.t / maxT) * plotW;
          const y2 = padT + plotH / 2 - (curr[key] / maxDeg) * (plotH / 2);
          ctx.strokeStyle = (prev.warn || curr.warn) ? '#f00' : color;
          ctx.lineWidth   = (prev.warn || curr.warn) ? 2 : 1.5;
          ctx.beginPath();
          ctx.moveTo(x1, y1);
          ctx.lineTo(x2, y2);
          ctx.stroke();
        }
      }

      plotLine('pitch', '#0ff');
      plotLine('roll',  '#ff0');

      ctx.font = '10px monospace';
      ctx.textAlign = 'left';
      ctx.fillStyle = '#0ff'; ctx.fillText('\u2014 Pitch',   padL, h - 4);
      ctx.fillStyle = '#ff0'; ctx.fillText('\u2014 Roll',    padL + 60, h - 4);
      ctx.fillStyle = '#f00'; ctx.fillText('\u2014 Warning', padL + 120, h - 4);
    }

    //--------------------------------------------------
    // CSV Export - rich format with metadata header
    //--------------------------------------------------

    function buildCSVString(m, data) {
      // Metadata header lines (prefixed with # so reviewers can skip them).
      // The leading "# rig_name," line doubles as this app's CSV
      // signature — used elsewhere to verify a file was really
      // generated by VDaT before ever loading/rendering it.
      let csv = '';
      csv += '# rig_name,' + (m.rigName || 'VDaT') + '\n';
      csv += '# date,' + (m.date || '') + '\n';
      csv += '# time,' + (m.time || '') + '\n';
      csv += '# version,' + (m.version || 'VDaT Beta') + '\n';
      csv += '# warn_pitch_up,' + (m.warnPitchUp || '') + '\n';
      csv += '# warn_pitch_down,' + (m.warnPitchDown || '') + '\n';
      csv += '# warn_roll_right,' + (m.warnRollRight || '') + '\n';
      csv += '# warn_roll_left,' + (m.warnRollLeft || '') + '\n';

      // Column headers
      csv += 'time_s,pitch_deg,roll_deg,heading_deg,warn,warn_pitch_up,warn_pitch_down,warn_roll_right,warn_roll_left\n';

      // Data rows
      data.forEach(d => {
        csv += d.t.toFixed(2) + ',' +
               d.pitch.toFixed(1) + ',' +
               d.roll.toFixed(1) + ',' +
               d.heading.toFixed(0) + ',' +
               (d.warn       ? '1' : '0') + ',' +
               (d.wPitchUp   ? '1' : '0') + ',' +
               (d.wPitchDown ? '1' : '0') + ',' +
               (d.wRollRight ? '1' : '0') + ',' +
               (d.wRollLeft  ? '1' : '0') + '\n';
      });

      return csv;
    }

    function downloadLogCSV() {
      if (!lastLogData || lastLogData.length === 0) return;

      const csv  = buildCSVString(lastLogMeta, lastLogData);
      const blob = new Blob([csv], { type: 'text/csv' });
      const url  = URL.createObjectURL(blob);
      const a    = document.createElement('a');
      const ts   = new Date().toISOString().replace(/[:.]/g, '-');
      a.href     = url;
      a.download = 'vdat-log-' + ts + '.csv';
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
    }

    //--------------------------------------------------
    // Rig Logging
    //--------------------------------------------------

    function setRigLogEnabled(enabled) {
      rigLogEnabledClient = enabled;
      fetch('/riglog/toggle?enabled=' + enabled, { method: 'POST' })
        .then(() => refreshRigLogState())
        .catch(() => {
          document.getElementById('rigLogStatus').textContent = 'Could not reach the rig — try again.';
        });
    }

    function refreshRigLogState() {
      fetch('/riglogs')
        .then(r => r.json())
        .then(d => {
          rigLogEnabledClient = d.enabled;
          const toggle = document.getElementById('rigLogToggle');
          if (toggle) toggle.checked = d.enabled;

          const status = document.getElementById('rigLogStatus');
          if (status) {
            status.textContent = d.active
              ? 'A rig-side session is currently recording.'
              : (d.enabled ? 'Ready \u2014 will record alongside your next session.' : '');
          }

          renderRigLogFiles(d.files || []);
        })
        .catch(() => {
          const status = document.getElementById('rigLogStatus');
          if (status) status.textContent = 'Could not reach the rig to check logging status.';
        });
    }

    function renderRigLogFiles(files) {
      const wrap = document.getElementById('riglogFilesList');
      if (!wrap) return;

      if (files.length === 0) {
        wrap.innerHTML = '<p class="settings-help">No sessions stored on the rig yet.</p>';
        return;
      }

      wrap.innerHTML = '<div class="settings-help" style="margin-top:10px;">Stored on rig:</div>' +
        files.map(f =>
          '<div class="riglog-file-row">' +
            '<span class="riglog-file-name">' + f.name + '</span>' +
            '<span class="riglog-file-size">' + (f.size / 1024).toFixed(1) + ' KB</span>' +
            '<a class="btn-small" href="/riglogs/download?file=' + f.name + '" download>DOWNLOAD</a>' +
            '<button class="btn-small btn-danger" onclick="deleteRigLogFile(\'' + f.name + '\')">DELETE</button>' +
          '</div>'
        ).join('');
    }

    function deleteRigLogFile(filename) {
      if (!confirm('Delete this session from the rig? This cannot be undone.')) return;
      fetch('/riglogs/delete?file=' + filename, { method: 'POST' })
        .then(() => refreshRigLogState())
        .catch(() => {});
    }

    //--------------------------------------------------
    // Auto-Save
    //--------------------------------------------------

    let autoSaveEnabled = false;

    (function initAutoSave() {
      const saved = localStorage.getItem('vdat_autosave');
      autoSaveEnabled = (saved === 'true');
      const toggle = document.getElementById('autoSaveToggle');
      if (toggle) toggle.checked = autoSaveEnabled;
    })();

    function setAutoSave(enabled) {
      autoSaveEnabled = enabled;
      localStorage.setItem('vdat_autosave', enabled ? 'true' : 'false');
      const status = document.getElementById('autoSaveStatus');
      if (status) {
        status.textContent = enabled
          ? 'On \u2014 sessions will be saved automatically.'
          : 'Off \u2014 tap Download CSV to save manually.';
        setTimeout(() => { status.textContent = ''; }, 3000);
      }
    }

    (function initRigLogClientState() {
      fetch('/riglogs')
        .then(r => r.json())
        .then(d => { rigLogEnabledClient = d.enabled; })
        .catch(() => {});
    })();

    //--------------------------------------------------
    // About Banner
    //--------------------------------------------------

    (function initAboutBanner() {
      const dismissed = localStorage.getItem('vdat_about_read');
      const banner    = document.getElementById('aboutBanner');
      if (dismissed === 'true' && banner) banner.style.display = 'none';
    })();

    function dismissAboutBanner() {
      localStorage.setItem('vdat_about_read', 'true');
      const banner = document.getElementById('aboutBanner');
      if (banner) banner.style.display = 'none';
    }

)rawliteral";

#endif
