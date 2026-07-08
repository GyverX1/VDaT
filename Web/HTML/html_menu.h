/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/HTML/html_menu.h
 *
 * Purpose:
 *    Settings menu overlay — all pages and content
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef HTML_MENU_H
#define HTML_MENU_H

#include <Arduino.h>

const char HTML_MENU[] PROGMEM = R"rawliteral(

  <div class="settings-overlay" id="settingsOverlay">

    <div class="settings-screen" id="screenMenu">
      <div class="settings-topbar">
        <span class="settings-title">SETTINGS</span>
        <button class="settings-close" onclick="closeSettings()">&times;</button>
      </div>
      <div class="settings-menu-list">
        <button class="menu-item" onclick="showScreen('screenThresholds')">
          <span>&#127919; Thresholds</span>
          <span class="menu-arrow">&#8250;</span>
        </button>
        <button class="menu-item" onclick="showScreen('screenCalibration')">
          <span>&#128205; Calibration</span>
          <span class="menu-arrow">&#8250;</span>
        </button>
        <button class="menu-item" onclick="showScreen('screenLogging')">
          <span>&#128190; Logging</span>
          <span class="menu-arrow">&#8250;</span>
        </button>
        <button class="menu-item" onclick="showScreen('screenRigName')">
          <span>&#127991; Rig Name</span>
          <span class="menu-arrow">&#8250;</span>
        </button>
        <button class="menu-item" onclick="showScreen('screenAbout')">
          <span>&#9432; About VDaT</span>
          <span class="menu-arrow">&#8250;</span>
        </button>
        <button class="menu-item" onclick="showScreen('screenDiagnostics')">
          <span>&#128295; Diagnostics</span>
          <span class="menu-arrow">&#8250;</span>
        </button>
      </div>
    </div>

    <div class="settings-screen hidden" id="screenThresholds">
      <div class="settings-topbar">
        <button class="settings-back" onclick="showScreen('screenMenu')"><span class="back-arrow">&#8249;</span> Back</button>
        <span class="settings-title">THRESHOLDS</span>
        <button class="settings-close" onclick="closeSettings()">&times;</button>
      </div>
      <div class="settings-body">

        <div class="settings-section">
          <h3>Tilt Warnings</h3>
          <p class="settings-help">Angle at which a warning triggers in the HUD header, the flashing card border, and the audio beep.</p>

          <div class="setting-row">
            <label>&#9650; Pitch Up Warning (&deg;)</label>
            <input type="number" id="inputPitchUp" min="1" max="89" step="0.5">
          </div>
          <div class="setting-row">
            <label>&#9660; Pitch Down Warning (&deg;)</label>
            <input type="number" id="inputPitchDown" min="1" max="89" step="0.5">
          </div>
          <div class="setting-row">
            <label>&#9654; Roll Right Warning (&deg;)</label>
            <input type="number" id="inputRollRight" min="1" max="89" step="0.5">
          </div>
          <div class="setting-row">
            <label>&#9664; Roll Left Warning (&deg;)</label>
            <input type="number" id="inputRollLeft" min="1" max="89" step="0.5">
          </div>
        </div>

        <div class="settings-section">
          <h3>Pitch Bar Color</h3>
          <p class="settings-help">Angle at which the pitch bar changes color, independent of the warning threshold above.</p>

          <div class="setting-row">
            <label>&#128993; Yellow Past (&deg;)</label>
            <input type="number" id="inputPitchColorYellow" min="1" max="89" step="0.5">
          </div>
          <div class="setting-row">
            <label>&#128308; Red Past (&deg;)</label>
            <input type="number" id="inputPitchColorRed" min="1" max="89" step="0.5">
          </div>
        </div>

        <div class="settings-section">
          <h3>Roll Bar Color</h3>
          <p class="settings-help">Angle at which the roll bar changes color, independent of the warning threshold above.</p>

          <div class="setting-row">
            <label>&#128993; Yellow Past (&deg;)</label>
            <input type="number" id="inputRollColorYellow" min="1" max="89" step="0.5">
          </div>
          <div class="setting-row">
            <label>&#128308; Red Past (&deg;)</label>
            <input type="number" id="inputRollColorRed" min="1" max="89" step="0.5">
          </div>
        </div>

        <div style="text-align:center; margin-top:8px;">
          <button class="btn" onclick="saveSettings()">SAVE TO MEMORY</button>
        </div>
        <div class="save-status" id="saveStatus"></div>
      </div>
    </div>

    <div class="settings-screen hidden" id="screenCalibration">
      <div class="settings-topbar">
        <button class="settings-back" onclick="showScreen('screenMenu')"><span class="back-arrow">&#8249;</span> Back</button>
        <span class="settings-title">CALIBRATION</span>
        <button class="settings-close" onclick="closeSettings()">&times;</button>
      </div>
      <div class="settings-body">

        <div class="settings-section">
          <h3>Live Reading</h3>
          <p class="settings-help">Current sensor output, updates live. Use this to confirm the rig is level before zeroing.</p>

          <div class="cal-live-row">
            <div class="cal-live-item">
              <div class="cal-live-label">PITCH</div>
              <div class="cal-live-value" id="calLivePitch">--&deg;</div>
            </div>
            <div class="cal-live-item">
              <div class="cal-live-label">ROLL</div>
              <div class="cal-live-value" id="calLiveRoll">--&deg;</div>
            </div>
          </div>
        </div>

        <div class="settings-section">
          <h3>Sensor Accuracy</h3>
          <p class="settings-help">How confident the IMU is in its current reading. If Low or Unreliable, gently move the rig through a few tilts and turns to let it settle.</p>

          <div class="cal-status-row">
            <span class="cal-status-dot" id="calStatusDot"></span>
            <span class="cal-status-text" id="calStatusText">Unknown</span>
          </div>
        </div>

        <div class="settings-section">
          <h3>Zero Calibration</h3>
          <p class="settings-help">Set the truck on a flat, level surface and tap Zero. Whatever angle it's sitting at right now becomes the new 0&deg; reference for both pitch and roll. Saved permanently, survives a reboot.</p>

          <div style="text-align:center;">
            <button class="btn" onclick="doZeroCalibration()">ZERO NOW</button>
          </div>
        </div>

        <div class="settings-section">
          <h3>Mounting Orientation</h3>
          <p class="settings-help">
            If pitch and roll look wrong or swapped, your IMU is likely mounted
            rotated relative to how it was designed. Pick the option that
            matches your mount, watch the Live Reading above respond, and
            confirm it moves the way you'd expect.
          </p>

          <select id="orientationSelect" class="settings-select" onchange="setOrientation(this.value)">
            <option value="0">A — Standard</option>
            <option value="1">B — Rotated 180&deg;</option>
            <option value="2">C — Rotated 90&deg; Clockwise</option>
            <option value="3">D — Rotated 90&deg; Counter-Clockwise</option>
          </select>
        </div>

        <div class="settings-section">
          <h3>Reset Calibration</h3>
          <p class="settings-help">Clears the saved zero offset and returns to raw, unadjusted sensor readings. This cannot be undone unless you zero again.</p>

          <div style="text-align:center;">
            <button class="btn btn-danger" onclick="confirmResetCalibration()">RESET TO RAW</button>
          </div>
        </div>

        <div class="cal-status-msg" id="calActionStatus"></div>
      </div>
    </div>

    <div class="settings-screen hidden" id="screenLogging">
      <div class="settings-topbar">
        <button class="settings-back" onclick="showScreen('screenMenu')"><span class="back-arrow">&#8249;</span> Back</button>
        <span class="settings-title">LOGGING</span>
        <button class="settings-close" onclick="closeSettings()">&times;</button>
      </div>
      <div class="settings-body">

        <div class="settings-section">
          <h3>Phone Logging</h3>
          <p class="settings-help">When enabled, your session CSV is saved to your Downloads folder automatically when you stop recording. When disabled, use the Download CSV button manually — but be aware that refreshing the page will erase the session. This is stored in this phone's browser, separate from Rig Logging below.</p>
          <div class="autosave-row">
            <span class="autosave-label">Auto-Save Sessions</span>
            <label class="toggle-switch">
              <input type="checkbox" id="autoSaveToggle" onchange="setAutoSave(this.checked)">
              <span class="toggle-slider"></span>
            </label>
          </div>
          <div class="autosave-status" id="autoSaveStatus"></div>
        </div>

        <div class="settings-section">
          <h3>Rig Logging</h3>
          <p class="settings-help">When enabled, the truck itself also writes a copy of each session to its own onboard storage — separate from Phone Logging above. Both can run at once, or either alone. Rig-stored logs can be downloaded straight from this dashboard even without your phone's copy.</p>
          <div class="autosave-row">
            <span class="autosave-label">Enable Rig Logging</span>
            <label class="toggle-switch">
              <input type="checkbox" id="rigLogToggle" onchange="setRigLogEnabled(this.checked)">
              <span class="toggle-slider"></span>
            </label>
          </div>
          <div class="autosave-status" id="rigLogStatus"></div>

          <div class="riglog-files" id="riglogFilesList"></div>
        </div>

        <div class="settings-section">
          <h3>How It Works</h3>
          <p class="settings-help">Use the Start Log / Stop Log button on the main dashboard to record a run. Logging happens entirely on your phone, nothing is stored on the truck. When you stop, the results for that session appear below.</p>
        </div>

        <div class="settings-section" id="logEmptyState">
          <h3>Last Session</h3>
          <p class="settings-help">No session recorded yet. Start a log from the main dashboard to see results here.</p>
        </div>

        <div class="settings-section hidden" id="logResultsSection">
          <h3>Last Session</h3>

          <div class="log-stat-row">
            <div class="log-stat-item">
              <div class="log-stat-label">Duration</div>
              <div class="log-stat-value" id="logDuration">--</div>
            </div>
            <div class="log-stat-item">
              <div class="log-stat-label">Data Points</div>
              <div class="log-stat-value" id="logPoints">--</div>
            </div>
          </div>

          <div class="log-stat-row">
            <div class="log-stat-item">
              <div class="log-stat-label">&#9650; Max Pitch Up</div>
              <div class="log-stat-value" id="logMaxPitchUp">--</div>
            </div>
            <div class="log-stat-item">
              <div class="log-stat-label">&#9660; Max Pitch Down</div>
              <div class="log-stat-value" id="logMaxPitchDown">--</div>
            </div>
          </div>

          <div class="log-stat-row">
            <div class="log-stat-item">
              <div class="log-stat-label">&#9654; Max Roll Right</div>
              <div class="log-stat-value" id="logMaxRollRight">--</div>
            </div>
            <div class="log-stat-item">
              <div class="log-stat-label">&#9664; Max Roll Left</div>
              <div class="log-stat-value" id="logMaxRollLeft">--</div>
            </div>
          </div>

          <div class="log-stat-row">
            <div class="log-stat-item">
              <div class="log-stat-label">Warning Crossings</div>
              <div class="log-stat-value" id="logWarnCount">--</div>
            </div>
          </div>

          <canvas id="logChart" width="360" height="200" class="log-chart"></canvas></canvas>

          <div style="text-align:center; margin-top:10px;">
            <button class="btn" onclick="downloadLogCSV()">DOWNLOAD CSV</button>
          </div>
        </div>

        <div class="settings-section">
          <h3>Saved Sessions</h3>
          <p class="settings-help">
            Every session is saved automatically to this phone, separate from the
            Auto-Save Downloads option above. These survive a page refresh or
            switching networks — this is also what you'll see if the truck
            connection drops.
          </p>
          <div id="savedSessionsList"></div>
        </div>

      </div>
    </div>

    <div class="settings-screen hidden" id="screenRigName">
      <div class="settings-topbar">
        <button class="settings-back" onclick="showScreen('screenMenu')"><span class="back-arrow">&#8249;</span> Back</button>
        <span class="settings-title">RIG NAME</span>
        <button class="settings-close" onclick="closeSettings()">&times;</button>
      </div>
      <div class="settings-body">

        <div class="settings-section">
          <h3>Display Name</h3>
          <p class="settings-help">This name appears in the dashboard header and on the OLED display. Maximum 10 characters. Keep it short — GLDR, VIPER, BEAST, etc. No spaces or special characters to ensure it fits cleanly.</p>

          <div class="setting-row" style="margin-top:8px;">
            <label>Rig Name</label>
            <input type="text"
                   id="inputRigName"
                   maxlength="10"
                   autocomplete="off"
                   autocorrect="off"
                   autocapitalize="characters"
                   spellcheck="false"
                   style="background:#222; border:1px solid #0ff; border-radius:4px; color:#0ff; font-family:monospace; font-size:1.1em; width:110px; text-align:center; padding:6px; text-transform:uppercase;">
          </div>
          <div style="text-align:center; margin-top:12px;">
            <button class="btn" onclick="saveRigName()">SAVE NAME</button>
          </div>
          <div class="save-status" id="rigNameStatus"></div>
        </div>

      </div>
    </div>

    <div class="settings-screen hidden" id="screenAbout">
      <div class="settings-topbar">
        <button class="settings-back" onclick="showScreen('screenMenu')"><span class="back-arrow">&#8249;</span> Back</button>
        <span class="settings-title">ABOUT</span>
        <button class="settings-close" onclick="closeSettings()">&times;</button>
      </div>
      <div class="settings-body">

        <div class="about-warning">
          <div class="about-warning-title">&#9888; READ THIS FIRST</div>
          <div class="about-warning-text">
            Session logs are stored <strong>only in your phone's memory</strong>.
            If you refresh or close this page before downloading, your session data is gone.
          </div>
          <div class="about-warning-text" style="margin-top:10px;">
            To protect your data automatically, enable <strong>Auto-Save</strong> in the
            Logging settings. Every session will be saved to your Downloads folder the moment
            you stop recording. Warning: this creates one file per session.
            Leave it off and you control when to download — but one accidental swipe-to-refresh
            will wipe the session you were looking at.
          </div>
          <div class="about-warning-text" style="margin-top:10px; color:#ff0;">
            If you lose a session and wonder why — did you read the About page?
          </div>
          <div class="about-warning-text" style="margin-top:10px;">
            <strong>Audio Note:</strong> Mobile browsers require a user interaction before
            allowing audio to play. On first load, tap anywhere on the dashboard once before
            warnings trigger — this unlocks the audio system. The Sound On/Off button will
            always work after that. This is a browser security feature, not a bug.
          </div>
        </div>

        <div class="about-section">
          <div class="about-heading">What Is VDaT?</div>
          <div class="about-text">
            VDaT stands for Vehicle Data and Telemetry. It's a real-time
            telemetry system built for any vehicle, any scale — collecting
            data from the sensor and turning it into something you can
            actually look at and use to understand how your rig behaves.
            It runs on an ESP32-S3 SuperMini with a BNO085 IMU. The web dashboard you are looking at
            right now is served directly from the ESP over its own WiFi access
            point — no internet connection required, no app to install, no
            account needed.
          </div>
        </div>

        <div class="about-section">
          <div class="about-heading">Session Logging</div>
          <div class="about-text">
            Every session can be recorded two ways at once, independently: Phone Logging
            saves to this device's browser, and Rig Logging (Settings &gt; Logging) writes
            a second copy directly onto the truck's own storage. Rig-stored sessions can be
            downloaded straight from this dashboard — useful if you'd rather not rely on your
            phone at all, or if you want a backup either way.
          </div>
        </div>

        <div class="about-section">
          <div class="about-heading">Hardware Required</div>
          <div class="about-text">&#9654; ESP32-S3 SuperMini</div>
          <div class="about-text">&#9654; BNO085 IMU (I2C)</div>
          <div class="about-text">&#9654; SSD1306 0.96" OLED display</div>
          <div class="about-text">&#9654; Any phone or tablet with a modern browser</div>
        </div>

        <div class="about-section">
          <div class="about-heading">How To Connect</div>
          <div class="about-text">
            Power on the ESP. On your phone, connect to the WiFi network named
            <strong>VDATTELE</strong> using the configured password. Open your browser
            and navigate to <strong>192.168.4.1</strong>. The dashboard loads instantly.
          </div>
          <div class="about-text" style="margin-top:8px; color:#aaa; font-size:0.95em;">
            The SSID and password can be changed in config.h before flashing.
          </div>
        </div>

        <div class="about-section">
          <div class="about-heading">Capabilities</div>
          <div class="about-text">&#9654; Live pitch and roll with independent peak tracking</div>
          <div class="about-text">&#9654; Attitude indicator (artificial horizon) with live heading</div>
          <div class="about-text">&#9654; Configurable warning thresholds with HUD alerts and audio beeps</div>
          <div class="about-text">&#9654; Flashing card borders on warning state</div>
          <div class="about-text">&#9654; Per-axis bar color thresholds (yellow and red zones)</div>
          <div class="about-text">&#9654; IMU zero calibration with persistent flash storage</div>
          <div class="about-text">&#9654; Session logging with chart, peak summary, warning count, and CSV export</div>
          <div class="about-text">&#9654; Auto-save option for session logs</div>
          <div class="about-text">&#9654; Full settings menu: Thresholds, Calibration, Logging, Rig Name</div>
          <div class="about-text">&#9654; OLED display with animated boot sequence</div>
          <div class="about-text">&#9654; AP-only WiFi — no home network credentials stored in firmware</div>
        </div>

        <div class="about-section">
          <div class="about-heading">Find Us</div>
          <div class="about-links">
            <div class="social-badge github-badge">
              <span class="social-icon">&#9906;</span>
              <div class="social-info">
                <div class="social-platform">GitHub</div>
                <div class="social-handle">GyverX1</div>
              </div>
            </div>
            <div class="social-badge instagram-badge">
              <span class="social-icon">&#128247;</span>
              <div class="social-info">
                <div class="social-platform">Instagram</div>
                <div class="social-handle">@gyverx</div>
              </div>
            </div>
            <div class="social-badge youtube-badge">
              <span class="social-icon">&#9654;</span>
              <div class="social-info">
                <div class="social-platform">YouTube</div>
                <div class="social-handle">@GyverX</div>
              </div>
            </div>
          </div>
        </div>

        <div class="about-section" style="text-align:center;">
          <div class="about-text" style="color:#888; font-size:0.9em;">VDaT Beta &nbsp;|&nbsp; &copy; 2026 GyverX</div>
          <div class="about-text" style="color:#888; font-size:0.9em; margin-top:6px;">Built by a maker, for makers.</div>
        </div>

      </div>
    </div>

    <div class="settings-screen hidden" id="screenDiagnostics">
      <div class="settings-topbar">
        <button class="settings-back" onclick="showScreen('screenMenu')"><span class="back-arrow">&#8249;</span> Back</button>
        <span class="settings-title">DIAGNOSTICS</span>
        <button class="settings-close" onclick="closeSettings()">&times;</button>
      </div>
      <div class="settings-body">

        <div class="settings-section">
          <h3>I2C Bus Health Check</h3>
          <p class="settings-help">
            Scans both I2C buses for a responding device. This is the real wiring test —
            an I2C line that's unplugged simply won't have anything answer on it.
          </p>
          <button class="btn" onclick="runDiagnostics()" style="width:100%; margin-bottom:16px;">
            &#128260; RUN DIAGNOSTIC SCAN
          </button>

          <div class="diag-card" id="diagImuCard">
            <div class="diag-card-title">IMU Bus (BNO085)</div>
            <div class="diag-card-pins" id="diagImuPins">SDA / SCL: --</div>
            <div class="diag-card-status" id="diagImuStatus">Not yet scanned</div>
          </div>

          <div class="diag-card" id="diagOledCard">
            <div class="diag-card-title">OLED Bus (SSD1306)</div>
            <div class="diag-card-pins" id="diagOledPins">SDA / SCL: --</div>
            <div class="diag-card-status" id="diagOledStatus">Not yet scanned</div>
          </div>

          <div class="diag-card" id="diagIntCard">
            <div class="diag-card-title">IMU Data-Ready Signal (INT)</div>
            <div class="diag-card-pins" id="diagIntPins">Pin: --</div>
            <div class="diag-card-status" id="diagIntStatus">Not yet scanned</div>
          </div>
        </div>

        <div class="settings-section">
          <h3>IMU Hardware Reset</h3>
          <p class="settings-help">
            Sends a real hardware reset to the sensor and confirms it responds
            afterward. Live readings will interrupt for a moment. Only use this
            if the sensor already seems stuck or unresponsive.
          </p>
          <button class="btn btn-danger" onclick="confirmTestImuReset()">TEST IMU RESET</button>
          <div class="diag-card-status" id="diagResetStatus" style="margin-top:10px;"></div>
        </div>

      </div>
    </div>

  </div>

  <div id="offlineOverlay" class="offline-overlay hidden">
    <div class="offline-content">
      <div class="offline-title">VDaT</div>
      <div class="offline-subtitle">No Truck Connected</div>
      <p class="settings-help" style="text-align:center;">
        Lost connection to the VDATTELE network. Your saved sessions are still here.
      </p>
      <div class="offline-sessions-header">Saved Sessions</div>
      <div id="offlineSessionsList"></div>
    </div>
  </div>

  )rawliteral";

#endif
