/**
 * lightshift_tracking.h
 * 
 * Tracks shift keypress lifecycles.
 */

#pragma once
#include "quantum.h"

/**
 * @brief Tracks shift keypress lifecycle.
 * 
 * Call from pre_process_record on every key event, not just shift key events.
 * 
 * @param keycode Keycode assigned to the key event by QMK
 * @param record Record for the key event
 */
void lghts_track_ppr(uint16_t keycode, const keyrecord_t *record);


/**
 * @brief Tracks shift keypress lifecycle.
 * 
 * Call from process_record on every key event, not just shift key events.
 * 
 * @param keycode Keycode assigned to the key event by QMK
 * @param record Record for the key event
 */
void lghts_track_pr(uint16_t keycode, const keyrecord_t *record);
