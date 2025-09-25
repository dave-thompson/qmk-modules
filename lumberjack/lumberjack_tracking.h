/**
 * @file lumberjack_tracking.h
 * 
 * @author dave-thompson
 */

#pragma once

#include "quantum.h"

/**
 * @brief Struct representing a single keypress, from press DOWN to release UP
 */
typedef struct {
    uint8_t row;         // key pressed: row
    uint8_t col;         // key pressed: column
    uint16_t keycode;    // keycode reported by QMK (sometimes differs on
                         // DOWN and UP)
    uint16_t down_time;  // time it was pressed DOWN
    uint16_t up_time;    // time is was released UP
    const char* color;   // log colour allocated to the key press
} keypress_t;


/**
 * @brief Tracks keypress lifecycle.
 * 
 * Call on press DOWN _and_ on release UP.
 * 
 * @param keycode Keycode assigned to the key event by QMK
 * @param record Record for the key event
 * 
 * @return keypress_t with the keypress details, or an empty keypress_t if
 *         the key wasn't tracked (i.e. MAX_TRACKED_KEYS exceeded)
 * 
 */
keypress_t lumberjack_track_key(uint16_t keycode, const keyrecord_t *record);
