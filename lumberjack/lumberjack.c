#if defined(CONSOLE_ENABLE) && defined(LUMBERJACK_LOG_KEYS)

#include "quantum.h"

#ifndef LUMBERJACK_MAX_TRACKED_KEYS
    #define LUMBERJACK_MAX_TRACKED_KEYS 10 // one per finger
#endif

static uint16_t last_time = 0;
static bool first_record = true;

///////////////////////////////////////////////////////////////////////////////
//
// Duration Calculation
//
///////////////////////////////////////////////////////////////////////////////

// Reurns duration of keypress if this was a key release
// Returns 0 if this is a press (rather than release)
// Returns 0 if this is a release but the press wasn't recorded (presumably as MAC_TRACKED_KEYS exceeded)
// @note For accurate durations, all key presses must be passed to this function as well as all key releases
uint16_t get_duration(uint16_t keycode, keyrecord_t *record) {

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
bool process_record_lumberjack(uint16_t current_keycode, keyrecord_t *record) {

    uint16_t current_time = record->event.time;    
    uint16_t hold_duration = get_duration(current_keycode, record);

    // create pretty keycode string
    char keycode_string[15+1];
    #ifdef KEYCODE_STRING_ENABLE
        snprintf(keycode_string, 15+1, "%15s", get_keycode_string(current_keycode));
    #else
        char hex_string[6+1];
        snprintf(hex_string, 6+1, "0x%04X", current_keycode);
        snprintf(keycode_string, 15+1, "%15s", hex_string);
    #endif

    // log the key event
    if (first_record == true) {
        uprintf(" %s  |  DOWN  |                   |\n", keycode_string);
        first_record = false;
    } else
        {
            // calculate delta
            uint16_t delta = current_time - last_time;

            // pad delta for pretty printing
            char delta_string[5+1];
            if (delta > 60000)
                snprintf(delta_string, 5+1, "%5d", delta); // if over 60000, assume negative
            else
                snprintf(delta_string, 5+1, "%5u", delta);

        if (record->event.pressed) {
            uprintf(" %s  |  DOWN  |  Delta: %s ms  |\n", keycode_string, delta_string);
        } else {
            uprintf(" %s  |  UP    |  Delta: %s ms  |  Hold: %u ms\n", keycode_string, delta_string, hold_duration);
        }
    }

    last_time = current_time;

return true;
}

void housekeeping_task_lumberjack(void) {
    // reset if over 60 seconds from last key event
    if (first_record == false) {
        uint16_t delta = timer_read() - last_time;
        if (delta > 60000 && delta < 62000) first_record = true;
    }
}

#endif // CONSOLE_ENABLE && LUMBERJACK_LOG_KEYS