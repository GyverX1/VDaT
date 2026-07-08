/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_audio.h
 *
 * Purpose:
 *    Web Audio API warning beeps with mute toggle
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef JS_AUDIO_H
#define JS_AUDIO_H

#include <Arduino.h>

const char JS_AUDIO[] PROGMEM = R"rawliteral(

    let audioCtx = null;
    let audioMuted = false;

    // Mobile browsers block audio until the user interacts with the page.
    // This listener initializes and resumes the AudioContext on the very
    // first touch or click so beeps work without needing to cycle the button.
    function initAudioOnInteraction() {
      if (!audioCtx) {
        audioCtx = new (window.AudioContext || window.webkitAudioContext)();
      }
      if (audioCtx.state === 'suspended') audioCtx.resume();
      document.removeEventListener('touchstart', initAudioOnInteraction);
      document.removeEventListener('click', initAudioOnInteraction);
    }
    document.addEventListener('touchstart', initAudioOnInteraction, { once: true });
    document.addEventListener('click', initAudioOnInteraction, { once: true });

    // Per-axis beep state
    const beepState = {
      pitch: { active: false, lastBeep: 0 },
      roll:  { active: false, lastBeep: 0 }
    };

    const BEEP_REMIND_MS = 3000;   // reminder interval while in warning
    const BEEP_DUR_MS    = 500;    // beep duration
    const PITCH_HZ       = 880;    // pitch warning tone
    const ROLL_HZ        = 440;    // roll warning tone

    function getAudioCtx() {
      if (!audioCtx) {
        audioCtx = new (window.AudioContext || window.webkitAudioContext)();
      }
      // Resume if suspended (browser autoplay policy)
      if (audioCtx.state === 'suspended') audioCtx.resume();
      return audioCtx;
    }

    function playBeep(freq) {
      if (audioMuted) return;
      try {
        const ctx = getAudioCtx();
        const osc = ctx.createOscillator();
        const gain = ctx.createGain();
        osc.connect(gain);
        gain.connect(ctx.destination);
        osc.type = 'sine';
        osc.frequency.setValueAtTime(freq, ctx.currentTime);
        gain.gain.setValueAtTime(0.4, ctx.currentTime);
        gain.gain.exponentialRampToValueAtTime(0.001, ctx.currentTime + BEEP_DUR_MS / 1000);
        osc.start(ctx.currentTime);
        osc.stop(ctx.currentTime + BEEP_DUR_MS / 1000);
      } catch(e) {}
    }

    function handleBeep(axis, isWarning, freq) {
      const state = beepState[axis];
      const now = Date.now();

      if (isWarning) {
        if (!state.active) {
          // Just crossed threshold - immediate beep
          playBeep(freq);
          state.lastBeep = now;
          state.active = true;
        } else if (now - state.lastBeep >= BEEP_REMIND_MS) {
          // Still in warning - reminder beep
          playBeep(freq);
          state.lastBeep = now;
        }
      } else {
        // Cleared - reset so next crossing triggers immediately
        state.active = false;
      }
    }

    function toggleMute() {
      audioMuted = !audioMuted;
      const btn = document.getElementById('muteBtn');
      if (btn) btn.textContent = audioMuted ? '\uD83D\uDD07 MUTED' : '\uD83D\uDD0A SOUND ON';
    }

)rawliteral";

#endif
