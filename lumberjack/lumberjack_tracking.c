#include "lumberjack_config.h"
#include "lumberjack_color_queue.h"
#include "lumberjack_tracking.h"

///////////////////////////////////////////////////////////////////////////////
//
// State
//
///////////////////////////////////////////////////////////////////////////////

// log of all currently depressed keys
static keypress_t depressed_keys[LUMBERJACK_MAX_TRACKED_KEYS];
static uint8_t num_depressed_keys = 0;


///////////////////////////////////////////////////////////////////////////////
//
// Tracking
//
///////////////////////////////////////////////////////////////////////////////

// Tracks a DOWN key press
// Returns keypress_t with the keypress details, or an empty keypress_t if
//         the press wasn't recorded (i.e. MAX_TRACKED_KEYS exceeded)
static keypress_t track_pressed_key(uint16_t keycode,
                                    const keyrecord_t *record) {
    
    // if not a key press DOWN, return empty keypress
    if (!record->event.pressed) {
        return (keypress_t){0};
    }

    // if no tracking space left, return empty keypress
    if (num_depressed_keys >= LUMBERJACK_MAX_TRACKED_KEYS) {
        return (keypress_t){0};
    }

    // record the keycode and key DOWN time
    depressed_keys[num_depressed_keys].row = record->event.key.row;
    depressed_keys[num_depressed_keys].col = record->event.key.col;
    depressed_keys[num_depressed_keys].keycode = keycode;
    depressed_keys[num_depressed_keys].down_time = record->event.time;

    // assign (the least recently used) colour to the key press
    if (lumberjack_color()) {
        depressed_keys[num_depressed_keys].color = lumberjack_next_color();
    }

    // return the key press
    num_depressed_keys++;
    return depressed_keys[num_depressed_keys-1];
}


// Tracks an UP key release
// Returns keypress_t with the keypress details, or an empty keypress_t if
//         the corresponding DOWN press wasn't recorded
static keypress_t track_released_key(uint16_t keycode,
                                     const keyrecord_t *record) {
    
    // if not a key release UP, return empty keypress
    if (record->event.pressed) {
        return (keypress_t){0};
    }

    // search for the key DOWN data
    for (uint8_t i = 0; i < num_depressed_keys; i++) {
        if (depressed_keys[i].row == record->event.key.row
            && depressed_keys[i].col == record->event.key.col) {
            
            // record the key UP time        
            depressed_keys[i].up_time = record->event.time;

            // do NOT update keycode, even if QMK has changed it since DOWN
            // event (non-matching DOWN / UP pairs are confusing to the user;
            // Lumberjack's purpose is to show the user which _physical_ keys
            // they pressed and when, not which keycodes were sent to their
            // application)

            // copy data to return struct
            keypress_t keypress = depressed_keys[i];

            // release key press's colour for future re-use
            if (lumberjack_color()) {
                // but only if it HAS a colour
                if (!(keypress.color == NULL || *keypress.color == '\0')) {
                    lumberjack_add_color_to_queue(depressed_keys[i].color);
                }
            }

            // remove key from depressed_keys list
            for (uint8_t j = i; j < num_depressed_keys - 1; j++) {
                depressed_keys[j] = depressed_keys[j + 1];
            }
            num_depressed_keys--;

            return keypress;
        }
    }

    // if key DOWN data not found, return empty keypress
    return (keypress_t){0};
}


// Triages key track requests
keypress_t lumberjack_track_key(uint16_t keycode, const keyrecord_t *record) {
    if (record->event.pressed) {
        return track_pressed_key(keycode, record);
    } else {
        return track_released_key(keycode, record);
    }
}
