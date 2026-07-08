/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/HTML/html_header.h
 *
 * Purpose:
 *    HTML head, CSS, HUD header with warn zones
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef HTML_HEADER_H
#define HTML_HEADER_H
#include <Arduino.h>
const char HTML_HEADER[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VDaT</title>
  <link rel="manifest" href="/manifest.json">
  <meta name="theme-color" content="#000000">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
  <meta name="apple-mobile-web-app-title" content="VDaT">
  <link rel="apple-touch-icon" href="/icon-192.png">
  <link rel="icon" href="/icon-192.png">
  <style>
    body {
      background: #000;
      color: #0ff;
      font-family: monospace;
      text-align: center;
      margin: 0;
      padding: 20px;
    }
    .hud-header {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 12px;
      margin-bottom: 4px;
    }
    .hud-header h1 {
      color: #ff0;
      font-size: 2.6em;
      font-weight: bold;
      margin: 0;
      flex-shrink: 0;
      letter-spacing: 2px;
      white-space: nowrap;
    }
    .hud-warn {
      width: 120px;
      font-size: 0.72em;
      font-weight: bold;
      color: #f00;
      height: 2.8em;
      display: flex;
      flex-direction: column;
      justify-content: center;
      overflow: hidden;
    }
    .hud-warn.left  { text-align: right; }
    .hud-warn.right { text-align: left; }
    .hud-warn span  { display: block; visibility: hidden; white-space: nowrap; }
    .hud-warn span.active { visibility: visible; }
    .hud-controls {
      display: flex;
      justify-content: center;
      align-items: center;
      gap: 8px;
      margin-bottom: 8px;
      flex-wrap: wrap;
    }
    .timer { color: #aaa; font-size: 1em; font-weight: bold; width: 100%; text-align: center; margin-bottom: 4px; }
    .mute-btn {
      background: #222;
      color: #0ff;
      border: 1px solid #0ff;
      border-radius: 6px;
      padding: 3px 10px;
      font-family: monospace;
      font-size: 0.75em;
      cursor: pointer;
    }
    .mute-btn:active { background: #0ff; color: #000; }
    .grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 16px;
      max-width: 400px;
      margin: 16px auto;
      align-items: stretch;
    }
    .card {
      background: #111;
      border: 1px solid #0ff;
      border-radius: 8px;
      padding: 16px;
      box-sizing: border-box;
      min-height: 205px;
    }
    .card-full {
      background: #111;
      border: 1px solid #0ff;
      border-radius: 8px;
      padding: 16px;
      max-width: 400px;
      margin: 0 auto 16px auto;
    }
    .label { font-size: 1em; color: #aaa; margin-bottom: 4px; letter-spacing: 1px; }
    .value-large { font-size: 4em; color: #0ff; font-weight: bold; }
    .value { font-size: 2.6em; color: #0ff; font-weight: bold; }
    .hdg-text {
      font-size: 1em;
      color: #ff0;
      margin-top: 4px;
      text-align: center;
    }
    .heading-card {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: flex-start;
      padding-bottom: 12px;
    }
    .rose-wrap {
      position: relative;
      width: 142px;
      height: 142px;
      margin: 8px auto 0 auto;
    }
    #attitudeCanvas {
      width: 142px;
      height: 142px;
      display: block;
    }
    .status { font-size: 0.75em; color: #444; margin-top: 20px; }
    .status-error { color: #d32f2f; font-weight: bold; }

    .diag-card {
      background: #1a1a1a;
      border: 2px solid #333;
      border-radius: 8px;
      padding: 12px;
      margin-bottom: 12px;
    }
    .diag-card-title { font-weight: bold; margin-bottom: 6px; }
    .diag-card-pins { font-size: 0.85em; color: #999; margin-bottom: 6px; }
    .diag-card-status { font-size: 0.9em; }
    .diag-card.diag-ok   { border-color: #2e7d32; }
    .diag-card.diag-fail { border-color: #d32f2f; }

    .riglog-files { margin-top: 4px; }
    .riglog-file-row {
      display: flex;
      align-items: center;
      gap: 8px;
      background: #1a1a1a;
      border: 1px solid #333;
      border-radius: 6px;
      padding: 6px 10px;
      margin-bottom: 6px;
      font-size: 0.82em;
    }
    .riglog-file-name { flex: 1; overflow-wrap: anywhere; }
    .riglog-file-size { color: #888; }

    .settings-select {
      width: 100%;
      background: #1a1a1a;
      color: #fff;
      border: 1px solid #333;
      border-radius: 6px;
      padding: 8px 10px;
      font-size: 0.95em;
    }

    .saved-session-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      background: #1a1a1a;
      border: 1px solid #333;
      border-radius: 8px;
      padding: 10px 12px;
      margin-bottom: 8px;
      gap: 10px;
    }
    .saved-session-name { font-weight: bold; }
    .saved-session-meta { font-size: 0.8em; color: #999; margin-top: 2px; }
    .saved-session-actions { display: flex; gap: 6px; flex-shrink: 0; }
    .btn-small {
      background: #222;
      border: 1px solid #0af;
      color: #0af;
      border-radius: 6px;
      padding: 6px 10px;
      font-size: 0.75em;
      cursor: pointer;
    }
    .btn-small.btn-danger { border-color: #d32f2f; color: #d32f2f; }

    .offline-overlay {
      position: fixed;
      top: 0; left: 0; right: 0; bottom: 0;
      background: #000;
      z-index: 9999;
      overflow-y: auto;
      padding: 30px 16px;
    }
    .offline-overlay.hidden { display: none; }
    .offline-content { max-width: 400px; margin: 0 auto; }
    .offline-title {
      font-size: 2em;
      font-weight: bold;
      text-align: center;
      color: #0af;
      text-shadow: 0 0 10px rgba(0,170,255,0.5);
    }
    .offline-subtitle {
      text-align: center;
      color: #999;
      margin-bottom: 16px;
    }
    .offline-sessions-header {
      font-weight: bold;
      margin: 20px 0 10px;
      border-bottom: 1px solid #333;
      padding-bottom: 6px;
    }
    .peak-row {
      display: flex;
      justify-content: space-between;
      margin-top: 6px;
      font-size: 0.95em;
      color: #888;
    }
    .peak-row span { color: #f80; }
    .bar-wrap {
      margin-top: 12px;
      position: relative;
      height: 18px;
      background: #222;
      border-radius: 4px;
      overflow: hidden;
    }
    .bar-center {
      position: absolute;
      left: 50%;
      top: 0;
      width: 2px;
      height: 100%;
      background: #444;
    }
    .bar-fill {
      position: absolute;
      top: 0;
      height: 100%;
      border-radius: 4px;
      transition: width 0.1s, left 0.1s, background 0.1s;
    }
    .bar-label {
      font-size: 0.7em;
      color: #fff36a;
      margin-top: 4px;
      text-shadow: 0 0 4px #665f00;
      display: flex;
      justify-content: space-between;
    }
    @keyframes flashBorder {
      0%, 100% { border-color: #f00; box-shadow: 0 0 8px #f00; }
      50%      { border-color: #111; box-shadow: none; }
    }
    .card-warn {
      animation: flashBorder 0.4s step-start infinite;
    }
    .btn {
      background: #111;
      color: #0ff;
      border: 1px solid #0ff;
      border-radius: 6px;
      padding: 10px 14px;
      font-family: monospace;
      font-size: 0.9em;
      cursor: pointer;
      white-space: nowrap;
    }
    .btn:active { background: #0ff; color: #000; }
    .settings-btn {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      background: #1a1a1a;
      color: #aaa;
      border: 1px solid #555;
      border-radius: 8px;
      padding: 12px 32px;
      font-family: monospace;
      font-size: 1em;
      cursor: pointer;
      margin: 12px auto 0 auto;
      letter-spacing: 1px;
    }
    .settings-btn:active { background: #333; color: #fff; }
    .main-actions {
      display: flex;
      justify-content: center;
      gap: 10px;
      flex-wrap: wrap;
    }
    .btn-recording {
      border-color: #f00;
      color: #f00;
    }
    .btn-recording:active { background: #f00; color: #000; }
    .log-rec-indicator {
      text-align: center;
      font-size: 0.8em;
      color: #f00;
      height: 18px;
      margin-top: 6px;
    }
    @keyframes pulseRec {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.3; }
    }
    .log-rec-indicator.recording {
      animation: pulseRec 1s ease-in-out infinite;
    }
    .setting-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 10px;
    }
    .setting-row label { font-size: 1em; color: #aaa; }
    .setting-row input[type="number"] {
      background: #222;
      border: 1px solid #0ff;
      border-radius: 4px;
      color: #0ff;
      font-family: monospace;
      font-size: 1.15em;
      width: 75px;
      text-align: center;
      padding: 6px;
    }
    .save-status {
      font-size: 0.9em;
      color: #0f0;
      text-align: center;
      height: 18px;
      margin-top: 8px;
    }

    /* Settings Overlay / Menu System */
    .settings-overlay {
      display: none;
      position: fixed;
      inset: 0;
      background: #000;
      z-index: 1000;
      overflow-y: auto;
    }
    .settings-overlay.open { display: block; }
    .settings-screen { display: block; }
    .hidden { display: none !important; }
    .settings-screen.hidden { display: none; }
    .settings-topbar {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 16px;
      border-bottom: 1px solid #333;
      position: sticky;
      top: 0;
      background: #000;
      z-index: 1;
    }
    .settings-title {
      color: #ff0;
      font-size: 1.1em;
      font-weight: bold;
      letter-spacing: 1px;
    }
    .settings-close, .settings-back {
      background: none;
      border: none;
      color: #fff;
      font-family: monospace;
      font-weight: bold;
      cursor: pointer;
      padding: 8px 12px;
      -webkit-tap-highlight-color: transparent;
    }
    .settings-close {
      font-size: 2.4em;
      line-height: 1;
    }
    .settings-back {
      font-size: 1.1em;
      display: flex;
      align-items: center;
      gap: 4px;
    }
    .settings-back .back-arrow {
      font-size: 1.4em;
      line-height: 1;
    }
    .settings-menu-list {
      padding: 16px;
      max-width: 400px;
      margin: 0 auto;
    }
    .menu-item {
      width: 100%;
      display: flex;
      justify-content: space-between;
      align-items: center;
      background: #111;
      border: 1px solid #0ff;
      border-radius: 8px;
      color: #0ff;
      font-family: monospace;
      font-size: 1.15em;
      padding: 16px 18px;
      margin-bottom: 10px;
      cursor: pointer;
      text-align: left;
    }
    .menu-item:active { background: #0ff2; }
    .menu-arrow { color: #888; font-size: 1.4em; }
    .settings-body {
      padding: 16px;
      max-width: 400px;
      margin: 0 auto;
    }
    .settings-section {
      background: #111;
      border: 1px solid #444;
      border-radius: 8px;
      padding: 16px;
      margin-bottom: 16px;
      text-align: left;
    }
    .settings-section h3 {
      color: #ff0;
      font-size: 1.1em;
      margin: 0 0 8px 0;
    }
    .settings-help {
      font-size: 0.9em;
      color: #aaa;
      margin: 0 0 14px 0;
      line-height: 1.5;
    }
    .cal-live-row {
      display: flex;
      justify-content: space-around;
      gap: 16px;
    }
    .cal-live-item {
      text-align: center;
    }
    .cal-live-label {
      font-size: 0.95em;
      color: #aaa;
      margin-bottom: 4px;
    }
    .cal-live-value {
      font-size: 2em;
      color: #0ff;
      font-weight: bold;
    }
    .cal-status-row {
      display: flex;
      align-items: center;
      gap: 10px;
      justify-content: center;
      padding: 4px 0;
    }
    .cal-status-dot {
      width: 16px;
      height: 16px;
      border-radius: 50%;
      background: #888;
      flex-shrink: 0;
    }
    .cal-status-dot.cal-unreliable { background: #f00; }
    .cal-status-dot.cal-low        { background: #f80; }
    .cal-status-dot.cal-medium     { background: #ff0; }
    .cal-status-dot.cal-high       { background: #0f0; }
    .cal-status-text {
      font-size: 1.1em;
      color: #fff;
    }
    .btn-danger {
      border-color: #f00;
      color: #f00;
    }
    .btn-danger:active { background: #f00; color: #000; }
    .cal-status-msg {
      font-size: 0.85em;
      color: #0f0;
      text-align: center;
      min-height: 18px;
      margin-top: 8px;
    }
    .log-stat-row {
      display: flex;
      justify-content: space-around;
      gap: 12px;
      margin-bottom: 12px;
    }
    .log-stat-item {
      text-align: center;
      flex: 1;
    }
    .log-stat-label {
      font-size: 0.85em;
      color: #aaa;
      margin-bottom: 4px;
    }
    .log-stat-value {
      font-size: 1.4em;
      color: #0ff;
      font-weight: bold;
    }
    .log-chart {
      width: 100%;
      height: auto;
      margin-top: 8px;
      background: #0a0a0a;
      border-radius: 6px;
    }

    /* Auto-save toggle */
    .autosave-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-top: 4px;
    }
    .autosave-label {
      font-size: 1em;
      color: #aaa;
    }
    .autosave-status {
      font-size: 0.85em;
      color: #0f0;
      margin-top: 6px;
      min-height: 16px;
    }
    .toggle-switch {
      position: relative;
      display: inline-block;
      width: 52px;
      height: 28px;
      flex-shrink: 0;
    }
    .toggle-switch input { display: none; }
    .toggle-slider {
      position: absolute;
      top: 0; left: 0; right: 0; bottom: 0;
      background: #333;
      border-radius: 28px;
      cursor: pointer;
      transition: background 0.2s;
    }
    .toggle-slider:before {
      content: '';
      position: absolute;
      width: 22px;
      height: 22px;
      left: 3px;
      top: 3px;
      background: #888;
      border-radius: 50%;
      transition: transform 0.2s, background 0.2s;
    }
    .toggle-switch input:checked + .toggle-slider { background: #0a3; }
    .toggle-switch input:checked + .toggle-slider:before {
      transform: translateX(24px);
      background: #0f0;
    }

    /* About page */
    .about-warning {
      background: #1a0000;
      border: 2px solid #f00;
      border-radius: 8px;
      padding: 18px;
      margin-bottom: 16px;
    }
    .about-warning-title {
      color: #f00;
      font-size: 1.4em;
      font-weight: bold;
      margin-bottom: 10px;
      text-align: center;
    }
    .about-warning-text {
      color: #fff;
      font-size: 1.05em;
      line-height: 1.6;
    }
    .about-section {
      background: #111;
      border: 1px solid #333;
      border-radius: 8px;
      padding: 18px;
      margin-bottom: 14px;
    }
    .about-heading {
      color: #ff0;
      font-size: 1.2em;
      font-weight: bold;
      margin-bottom: 10px;
    }
    .about-text {
      color: #ddd;
      font-size: 1.05em;
      line-height: 1.7;
      margin-bottom: 10px;
    }
    .about-text:last-child { margin-bottom: 0; }
    .about-links {
      display: flex;
      flex-direction: column;
      gap: 10px;
      margin-top: 4px;
    }
    .social-badge {
      display: flex;
      align-items: center;
      gap: 14px;
      padding: 12px 16px;
      border-radius: 8px;
    }
    .github-badge {
      background: #1a1a1a;
      border: 1px solid #555;
    }
    .instagram-badge {
      background: #1a0d1a;
      border: 1px solid #c13584;
    }
    .youtube-badge {
      background: #1a0000;
      border: 1px solid #f00;
    }
    .social-icon {
      font-size: 1.8em;
      flex-shrink: 0;
      width: 36px;
      text-align: center;
    }
    .github-badge .social-icon { color: #ddd; }
    .instagram-badge .social-icon { color: #c13584; }
    .youtube-badge .social-icon { color: #f00; }
    .social-info { display: flex; flex-direction: column; gap: 2px; }
    .social-platform {
      font-size: 0.85em;
      color: #888;
      text-transform: uppercase;
      letter-spacing: 1px;
    }
    .social-handle {
      font-size: 1.1em;
      color: #fff;
      font-weight: bold;
    }

    /* About banner - main page */
    @keyframes flashBanner {
      0%, 49% { background: #1a0000; border-color: #f00; }
      50%, 100% { background: #3a0000; border-color: #ff0; }
    }
    .about-banner {
      border: 2px solid #f00;
      border-radius: 8px;
      padding: 14px 16px;
      max-width: 400px;
      margin: 16px auto 0 auto;
      text-align: center;
      animation: flashBanner 1s step-start infinite;
    }
    .about-banner-text {
      display: block;
      color: #ff0;
      font-size: 1.1em;
      font-weight: bold;
      margin-bottom: 10px;
    }
    .about-banner-btn {
      background: #f00;
      border: none;
      border-radius: 6px;
      color: #fff;
      font-family: monospace;
      font-size: 1em;
      font-weight: bold;
      padding: 8px 20px;
      cursor: pointer;
    }
    .about-banner-btn:active { background: #c00; }
  </style>
</head>
<body>
  <div class="hud-header">
    <div class="hud-warn left">
      <span id="warnRollLeft">&#9664; ROLL</span>
      <span id="warnRollRight">ROLL &#9654;</span>
    </div>
    <h1>VDaT</h1>
    <div class="hud-warn right">
      <span id="warnPitchUp">PITCH &#9650;</span>
      <span id="warnPitchDown">PITCH &#9660;</span>
    </div>
  </div>
  <div class="timer">Session: <span id="timer">00:00:00</span></div>

  )rawliteral";

#endif
