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


/**
 * @brief Stops tracking a DOUBLE SHIFTING lightshift, if one exists
 * 
 * @param keycode Double-ineligible keycode attempting to use the shift
 * 
 * @return keycode of DOUBLE SHIFTING lightshift, or KC_NO if none found
 * 
 * @note multiple used lightshifts are rare, but possible; function returns
 *       first found only
 */
uint16_t lghts_set_double_inactive(uint16_t keycode);


/**
 * @brief Checks if given keycode and record are a layer switch or modifier
 * 
 * Checks for all layer switches or modifiers, including held MT / LT keys
 * 
 * @warning must be called with a record from process_record, not from
 *          pre-process_record (so that MT/LTs are resolved)
 * 
 * @param keycode Keycode to be checked
 * @param record Record to be checked
 * 
 * @return true if layer switch or modifier, false if not
 */
bool lghts_is_layer_or_mod(uint16_t keycode, const keyrecord_t *record);
