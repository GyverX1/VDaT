/******************************************************************************
 * VDaT
 *
 * File:
 *    dashboard.h
 *
 * Purpose:
 *    Dashboard sub-header aggregator.
 *    Includes all HTML and JS PROGMEM strings so web.cpp
 *    can access them for chunked streaming via sendContent_P().
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <Arduino.h>

#include "Web/HTML/html_header.h"
#include "Web/HTML/html_cards.h"
#include "Web/HTML/html_menu.h"
#include "Web/HTML/html_footer.h"

#include "Web/JS/js_utils.h"
#include "Web/JS/js_audio.h"
#include "Web/JS/js_attitude.h"
#include "Web/JS/js_settings.h"
#include "Web/JS/js_calibration.h"
#include "Web/JS/js_storage.h"
#include "Web/JS/js_logging.h"
#include "Web/JS/js_saved_sessions.h"
#include "Web/JS/js_rigname.h"
#include "Web/JS/js_menu.h"
#include "Web/JS/js_fetch.h"

#endif
