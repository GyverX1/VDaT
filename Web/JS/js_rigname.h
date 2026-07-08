/******************************************************************************
 * VDaT
 *
 * File:
 *    Web/JS/js_rigname.h
 *
 * Purpose:
 *    Rig Name settings page — save and header update
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef JS_RIGNAME_H
#define JS_RIGNAME_H

#include <Arduino.h>

const char JS_RIGNAME[] PROGMEM = R"rawliteral(

    function saveRigName() {
      const input = document.getElementById('inputRigName');
      if (!input) return;

      let name = input.value.trim().toUpperCase();

      if (name.length === 0 || name.length > 10) {
        document.getElementById('rigNameStatus').textContent = '1-10 characters required';
        return;
      }

      fetch('/setname?name=' + encodeURIComponent(name))
        .then(res => res.json())
        .then(d => {
          if (d.status === 'saved') {
            // Update header immediately
            const h1 = document.querySelector('.hud-header h1');
            if (h1) h1.textContent = name;

            document.getElementById('rigNameStatus').textContent = 'Saved!';
            setTimeout(() => {
              document.getElementById('rigNameStatus').textContent = '';
            }, 2000);
          } else {
            document.getElementById('rigNameStatus').textContent = d.msg || 'Save failed';
          }
        })
        .catch(() => {
          document.getElementById('rigNameStatus').textContent = 'Save failed';
        });
    }

    // Called from js_menu.h showScreen() to pre-populate the field
    function initRigNameScreen() {
      const h1 = document.querySelector('.hud-header h1');
      const input = document.getElementById('inputRigName');
      if (h1 && input) input.value = h1.textContent;
    }

)rawliteral";

#endif
