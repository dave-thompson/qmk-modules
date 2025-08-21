#if defined(CONSOLE_ENABLE) && defined(LUMBERJACK_LOG_KEYS)

#include "quantum.h"
#include "lumberjack_utils.h"

///////////////////////////////////////////////////////////////////////////////
//
// Config Defaults
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LUMBERJACK_MAX_TRACKED_KEYS
    #define LUMBERJACK_MAX_TRACKED_KEYS 10 // one per finger
#endif

#ifndef LUMBERJACK_KEYCODE_LENGTH
    #define LUMBERJACK_KEYCODE_LENGTH 15 // 6-char modifier, 7-char basic keycode, & 2 parens
#endif


///////////////////////////////////////////////////////////////////////////////
//
// State
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    bool active;                // Has there been a keypress in the last 60 secs?
    uint16_t last_event_time;   // Time of last key event

} lumberjack_state_t;

static lumberjack_state_t state = {0};


///////////////////////////////////////////////////////////////////////////////
//
// Calculating Hold Durations
//
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Calculate hold duration for key press/release events
 * 
 * @details Tracks up to LUMBERJACK_MAX_TRACKED_KEYS simultaneous key presses
 *          and calculates hold duration when keys are released. Uses O(n) 
 *          linear search for key matching.
 * 
 * @param keycode The QMK keycode that was pressed / released
 * @param record The corresponding key event record
 * 
 * @return Hold duration in milliseconds, iff this is a key release.
 *         0 if this is a press (rather than a release)
 *         0 if the corresponding press wasn't recorded (i.e. MAX_TRACKED_KEYS exceeded)
 * 
 * @note Must be called for both _presses_ and _releases_ for duration calculation
 */
static uint16_t track_duration(uint16_t keycode, keyrecord_t *record) {

    // Times of previous Key Presses
    static struct {
        uint16_t keycode;
        uint16_t down_time;
    } key_times[LUMBERJACK_MAX_TRACKED_KEYS];

    // Numer of keys currently pressed
    static uint8_t num_tracked = 0;

    // Calculate duration
    uint16_t hold_duration = 0;
    if (record->event.pressed) { // Key DOWN
        // record down event
        if (num_tracked < LUMBERJACK_MAX_TRACKED_KEYS) {
            key_times[num_tracked].keycode = keycode;
            key_times[num_tracked].down_time = record->event.time;
            num_tracked++;
        }
    } else { // Key UP
        // how long has the current key been pressed?
        for (uint8_t i = 0; i < num_tracked; i++) {
            if (key_times[i].keycode == keycode) {

                hold_duration = record->event.time - key_times[i].down_time;
                // delete the down event now we've used it
                for (uint8_t j = i; j < num_tracked - 1; j++) {
                    key_times[j] = key_times[j + 1];
                }
                num_tracked--;
                break;
            }
        }
    }
    return hold_duration;
}


///////////////////////////////////////////////////////////////////////////////
//
// Handling Key Presses
//
///////////////////////////////////////////////////////////////////////////////

// Handles all key presses/releases
bool pre_process_record_lumberjack(uint16_t current_keycode, keyrecord_t *record) {

    // calculate hold duration
    uint16_t hold_duration = track_duration(current_keycode, record);
  
    // create pretty keycode string
    char keycode_string[LUMBERJACK_KEYCODE_LENGTH+1];
    #ifdef KEYCODE_STRING_ENABLE
        lumberjack_right_align_string(keycode_string, LUMBERJACK_KEYCODE_LENGTH+1, get_keycode_string(current_keycode));
    #else
        char hex_string[6+1];
        lumberjack_keycode_to_hex_string(hex_string, current_keycode);
        lumberjack_right_align_string(keycode_string, LUMBERJACK_KEYCODE_LENGTH+1, hex_string);
    #endif

    // log the key event
    if (!state.active) {
        uprintf(" %s  |  DOWN  |                    |\n", keycode_string);
        state.active = true;
    } else {
        // calculate delta
        uint16_t delta = record->event.time - state.last_event_time;

        // convert delta to a string
        char delta_string[6+1]; // 5 digits, 1 minus sign, 1 terminating NULL
        if (delta > 61000) { // if well over 60s, must be negative
            // note: negative deltas only arise in process_record, not in pre_process_record
            delta = -(int)delta;
            lumberjack_uint_to_string(delta_string, (int)delta);
            lumberjack_prepend_char(delta_string, '-');
        }
        else
            lumberjack_uint_to_string(delta_string, delta);
        
        // pad delta string for prettier printing
        char padded_delta_string[6+1];
        lumberjack_right_align_string(padded_delta_string, 6+1, delta_string);

        // write to log
        if (record->event.pressed) {
            uprintf(" %s  |  DOWN  |  Delta: %s ms  |\n", keycode_string, padded_delta_string);
        } else {
            uprintf(" %s  |  UP    |  Delta: %s ms  |  Hold: %u ms\n", keycode_string, padded_delta_string, hold_duration);
        }
    }

    state.last_event_time = record->event.time;

    return true;
}

// Stop clock if idle (over 60 seconds from last key event)
void housekeeping_task_lumberjack(void) {
    if (state.active) {
        uint16_t delta = timer_read() - state.last_event_time;
        if (delta > 60000 && delta < 62000) state.active = false;
    }
}

#endif // CONSOLE_ENABLE && LUMBERJACK_LOG_KEYS
