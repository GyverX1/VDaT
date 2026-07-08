/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_attitude.h
 *
 * Purpose:
 *    Attitude indicator canvas renderer
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef JS_ATTITUDE_H
#define JS_ATTITUDE_H

#include <Arduino.h>

const char JS_ATTITUDE[] PROGMEM = R"rawliteral(

    function drawAttitudeIndicator(pitch, roll, heading) {
      const canvas = document.getElementById('attitudeCanvas');
      if (!canvas) return;
      const ctx = canvas.getContext('2d');
      const size = canvas.width;
      const cx = size / 2;
      const cy = size / 2;
      const r  = size * 0.46;

      ctx.clearRect(0, 0, size, size);

      // --- Clip to circle ---
      ctx.save();
      ctx.beginPath();
      ctx.arc(cx, cy, r, 0, Math.PI * 2);
      ctx.clip();

      // Pitch offset: how many pixels the horizon shifts per degree
      // At 45 deg pitch the horizon rides to the edge of the instrument
      const pixPerDeg = r / 45.0;
      const pitchOffset = pitch * pixPerDeg;

      // Roll: rotate the entire scene
      ctx.save();
      ctx.translate(cx, cy);
      ctx.rotate(roll * Math.PI / 180);
      ctx.translate(-cx, -cy);

      // Sky (upper half) - dark blue represented by dark fill
      ctx.fillStyle = '#003366';
      ctx.fillRect(0, 0, size, cy - pitchOffset);

      // Ground (lower half) - dark brown
      ctx.fillStyle = '#3a1f00';
      ctx.fillRect(0, cy - pitchOffset, size, size);

      // Horizon line
      ctx.strokeStyle = '#fff';
      ctx.lineWidth = 2;
      ctx.beginPath();
      ctx.moveTo(0,    cy - pitchOffset);
      ctx.lineTo(size, cy - pitchOffset);
      ctx.stroke();

      // Pitch ladder lines (every 10 degrees)
      ctx.strokeStyle = 'rgba(255,255,255,0.6)';
      ctx.fillStyle   = 'rgba(255,255,255,0.6)';
      ctx.lineWidth   = 1;
      ctx.font        = '8px monospace';
      ctx.textAlign   = 'center';

      for (let deg = -40; deg <= 40; deg += 10) {
        if (deg === 0) continue;
        const ly = cy - pitchOffset - (deg * pixPerDeg);
        const lw = deg % 20 === 0 ? 28 : 16;
        ctx.beginPath();
        ctx.moveTo(cx - lw, ly);
        ctx.lineTo(cx + lw, ly);
        ctx.stroke();
        ctx.fillText(Math.abs(deg), cx + lw + 6, ly + 3);
      }

      ctx.restore(); // end roll rotation

      // --- Overlay: fixed aircraft symbol (always centered, no rotation) ---
      ctx.strokeStyle = '#ff0';
      ctx.lineWidth   = 2.5;
      ctx.lineCap     = 'round';

      // Left wing
      ctx.beginPath();
      ctx.moveTo(cx - r * 0.5, cy);
      ctx.lineTo(cx - r * 0.15, cy);
      ctx.stroke();

      // Right wing
      ctx.beginPath();
      ctx.moveTo(cx + r * 0.15, cy);
      ctx.lineTo(cx + r * 0.5, cy);
      ctx.stroke();

      // Center dot
      ctx.fillStyle = '#ff0';
      ctx.beginPath();
      ctx.arc(cx, cy, 3, 0, Math.PI * 2);
      ctx.fill();

      // --- Roll arc at top ---
      ctx.strokeStyle = 'rgba(255,255,255,0.4)';
      ctx.lineWidth   = 1;
      ctx.beginPath();
      ctx.arc(cx, cy, r * 0.88, -Math.PI * 0.75, -Math.PI * 0.25);
      ctx.stroke();

      // Roll tick marks
      [0, -15, 15, -30, 30, -45, 45, -60, 60].forEach(deg => {
        const rad = (deg - 90) * Math.PI / 180;
        const inner = r * 0.82;
        const outer = r * (deg % 30 === 0 ? 0.94 : 0.88);
        ctx.beginPath();
        ctx.moveTo(cx + Math.cos(rad) * inner, cy + Math.sin(rad) * inner);
        ctx.lineTo(cx + Math.cos(rad) * outer, cy + Math.sin(rad) * outer);
        ctx.strokeStyle = 'rgba(255,255,255,0.6)';
        ctx.lineWidth = deg % 30 === 0 ? 1.5 : 1;
        ctx.stroke();
      });

      // Roll pointer (rotates with roll)
      ctx.save();
      ctx.translate(cx, cy);
      ctx.rotate(roll * Math.PI / 180);
      ctx.fillStyle = '#ff0';
      ctx.beginPath();
      ctx.moveTo(0, -(r * 0.88));
      ctx.lineTo(-5, -(r * 0.78));
      ctx.lineTo(5,  -(r * 0.78));
      ctx.closePath();
      ctx.fill();
      ctx.restore();

      // --- Instrument bezel ring ---
      ctx.restore(); // end clip
      ctx.beginPath();
      ctx.arc(cx, cy, r, 0, Math.PI * 2);
      ctx.strokeStyle = '#0ff';
      ctx.lineWidth   = 2;
      ctx.stroke();

      // --- Heading text below instrument ---
      const hdgEl = document.getElementById('attitudeHdg');
      if (hdgEl) {
        hdgEl.textContent = String(Math.round(heading)).padStart(3,'0') + '\u00B0';
      }
    }

)rawliteral";

#endif
