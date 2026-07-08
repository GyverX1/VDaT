/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_saved_sessions.h
 *
 * Purpose:
 *    Renders the Saved Sessions list (from IndexedDB) in both the
 *    Logging settings screen and the offline overlay. Also owns
 *    connectivity tracking — after a run of consecutive failed
 *    fetches, shows the offline screen automatically instead of
 *    leaving the browser's own connection-error page to take over.
 *
 ******************************************************************************/
#ifndef JS_SAVED_SESSIONS_H
#define JS_SAVED_SESSIONS_H

#include <Arduino.h>

const char JS_SAVED_SESSIONS[] PROGMEM = R"rawliteral(

    //--------------------------------------------------
    // List rendering — shared by the Logging screen and the offline overlay
    //--------------------------------------------------

    function formatSessionRow(s) {
      const mins = Math.floor(s.duration / 60);
      const secs = Math.floor(s.duration % 60);
      const durStr = mins > 0 ? (mins + 'm ' + secs + 's') : (secs + 's');

      return '<div class="saved-session-row">' +
        '<div class="saved-session-info">' +
          '<div class="saved-session-name">' + s.rigName + '</div>' +
          '<div class="saved-session-meta">' + s.date + ' ' + s.time + ' &bull; ' + durStr +
            ' &bull; ' + s.warnCount + ' warning' + (s.warnCount === 1 ? '' : 's') + '</div>' +
        '</div>' +
        '<div class="saved-session-actions">' +
          '<button class="btn-small" onclick="viewSavedSession(' + s.id + ')">VIEW</button>' +
          '<button class="btn-small" onclick="shareSavedSession(' + s.id + ')">SHARE</button>' +
          '<button class="btn-small btn-danger" onclick="deleteSavedSession(' + s.id + ')">DELETE</button>' +
        '</div>' +
      '</div>';
    }

    function renderSessionListInto(containerId, sessions) {
      const container = document.getElementById(containerId);
      if (!container) return;

      if (!sessions || sessions.length === 0) {
        container.innerHTML = '<p class="settings-help">No saved sessions yet.</p>';
        return;
      }

      container.innerHTML = sessions.map(formatSessionRow).join('');
    }

    function refreshSavedSessionsList() {
      getAllSessions().then((sessions) => {
        renderSessionListInto('savedSessionsList', sessions);
        renderSessionListInto('offlineSessionsList', sessions);
      }).catch((e) => console.log('Could not load saved sessions:', e));
    }

    //--------------------------------------------------
    // Parse a stored CSV back into the same {t, pitch, roll, heading,
    // warn, ...} shape buildLogSummary()/drawLogChart() already expect
    // — reuses the existing summary/chart code rather than duplicating it.
    //--------------------------------------------------

    function parseVDaTCsv(text) {
      if (!isValidVDaTCsv(text)) return null;

      const lines = text.split('\n').filter(l => l.length > 0 && !l.startsWith('#'));
      if (lines.length < 2) return null;

      const rows = lines.slice(1); // skip column header row
      return rows.map(line => {
        const c = line.split(',');
        return {
          t:          parseFloat(c[0]),
          pitch:      parseFloat(c[1]),
          roll:       parseFloat(c[2]),
          heading:    parseFloat(c[3]),
          warn:       c[4] === '1',
          wPitchUp:   c[5] === '1',
          wPitchDown: c[6] === '1',
          wRollRight: c[7] === '1',
          wRollLeft:  c[8] === '1'
        };
      });
    }

    function viewSavedSession(id) {
      getSessionFromDB(id).then((s) => {
        if (!s) return;
        const parsed = parseVDaTCsv(s.csv);
        if (!parsed) {
          alert('This session\'s data could not be read.');
          return;
        }
        lastLogData = parsed;
        lastLogMeta = { rigName: s.rigName, date: s.date, time: s.time };
        buildLogSummary();
        showScreen('screenLogging');
      });
    }

    function shareSavedSession(id) {
      getSessionFromDB(id).then((s) => {
        if (!s) return;

        const filename = 'vdat-log-' + s.date.replace(/\//g, '-') + '-' + s.time.replace(/[: ]/g, '-') + '.csv';
        const file = new File([s.csv], filename, { type: 'text/csv' });

        if (navigator.share && navigator.canShare && navigator.canShare({ files: [file] })) {
          navigator.share({ files: [file], title: 'VDaT Session Log' }).catch(() => {});
        } else {
          // Fallback for browsers without native file sharing
          const blob = new Blob([s.csv], { type: 'text/csv' });
          const url  = URL.createObjectURL(blob);
          const a    = document.createElement('a');
          a.href = url;
          a.download = filename;
          document.body.appendChild(a);
          a.click();
          document.body.removeChild(a);
          URL.revokeObjectURL(url);
        }
      });
    }

    function deleteSavedSession(id) {
      if (!confirm('Delete this session? This cannot be undone.')) return;
      deleteSessionFromDB(id).then(refreshSavedSessionsList);
    }

    //--------------------------------------------------
    // Offline detection — shows the offline screen after a run of
    // consecutive failed fetches, rather than leaving the browser's
    // own connection-error page to take over mid-session.
    //--------------------------------------------------

    let consecutiveFetchFailures = 0;
    const OFFLINE_THRESHOLD = 3; // ~3 failed polls before declaring "offline"
    let isShowingOfflineScreen = false;

    function reportFetchSuccess() {
      consecutiveFetchFailures = 0;
      if (isShowingOfflineScreen) {
        isShowingOfflineScreen = false;
        document.getElementById('offlineOverlay').classList.add('hidden');
      }
    }

    function reportFetchFailure() {
      consecutiveFetchFailures++;
      if (consecutiveFetchFailures >= OFFLINE_THRESHOLD && !isShowingOfflineScreen) {
        isShowingOfflineScreen = true;
        refreshSavedSessionsList();
        document.getElementById('offlineOverlay').classList.remove('hidden');
      }
    }

    // Populate the list once on initial load too
    refreshSavedSessionsList();

)rawliteral";

#endif
