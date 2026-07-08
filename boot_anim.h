/******************************************************************************
 * VDaT
 *
 * File:
 *    boot_anim.h
 *
 * Purpose:
 *    Boot animation interface declarations
 *
 * Created:
 *    [06-28-26]
 *
 * Last Modified:
 *    [07-01-26]
 *
 ******************************************************************************/
#ifndef BOOT_ANIM_H
#define BOOT_ANIM_H

// Runs the full boot animation sequence and blocks until complete.
// Call once after initDisplay(), before initIMU().
void showBootAnimation();

#endif
