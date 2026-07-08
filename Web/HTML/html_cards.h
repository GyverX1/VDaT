/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/HTML/html_cards.h
 *
 * Purpose:
 *    Telemetry cards — pitch, roll, attitude indicator
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef HTML_CARDS_H
#define HTML_CARDS_H

#include <Arduino.h>

const char HTML_CARDS[] PROGMEM = R"rawliteral(

  <div class="card-full" id="pitchCard">
    <div class="label">PITCH</div>
    <div class="value-large" id="pitch">--</div>
    <div class="bar-wrap">
      <div class="bar-center"></div>
      <div class="bar-fill" id="pitchBar"></div>
    </div>
    <div class="bar-label"><span>-45&deg;</span><span>0&deg;</span><span>+45&deg;</span></div>
    <div class="peak-row">
      <div>&#9660; PEAK: <span id="peakPitchDown">0.0&deg;</span></div>
      <div>&#9650; PEAK: <span id="peakPitchUp">0.0&deg;</span></div>
    </div>
  </div>

  <div class="grid">
    <div class="card" id="rollCard">
      <div class="label">ROLL</div>
      <div class="value" id="roll">--</div>
      <div class="bar-wrap">
        <div class="bar-center"></div>
        <div class="bar-fill" id="rollBar"></div>
      </div>
      <div class="bar-label"><span>-45&deg;</span><span>0&deg;</span><span>+45&deg;</span></div>
      <div class="peak-row">
        <div>&#9664; PEAK: <span id="peakRollLeft">0.0&deg;</span></div>
        <div>&#9654; PEAK: <span id="peakRollRight">0.0&deg;</span></div>
      </div>
    </div>

    <div class="card heading-card">
      <div class="label">ATTITUDE</div>
      <div class="rose-wrap">
        <canvas id="attitudeCanvas" width="142" height="142"></canvas>
      </div>
      <div class="hdg-text" id="attitudeHdg">---&deg;</div>
    </div>
  </div>

  )rawliteral";

#endif
