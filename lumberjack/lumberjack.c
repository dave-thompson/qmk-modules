#if defined(CONSOLE_ENABLE) && defined(LUMBERJACK_LOG_KEYS)

#include "quantum.h"
#include "lumberjack_utils.h"
#include "lumberjack_colors.h"


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
// Colour Initialisation
//
///////////////////////////////////////////////////////////////////////////////

#define RESET_COLOR "\033[0m"


void keyboard_post_init_lumberjack(void) {

    // LUMBERJACK_COLOR_CODES(
    //     "A",  // Bright Red
    //     "B",  // Bright Green  
    //     "C",  // Bright Blue
    //     "D",  // Bright Yellow
    //     "E",  // Bright Magenta
    //     "F",  // Bright Cyan
    //     "G",  // Red
    //     "H",  // Magenta
    //     "I",  // Yellow
    //     "J"   // White
    // );


    // lumberjack_release_color("\033[91m");  // Bright Red
    // lumberjack_release_color("\033[92m");  // Bright Green 
    // lumberjack_release_color("\033[94m");  // Bright Blue
    // lumberjack_release_color("\033[93m");  // Bright Yellow
    // lumberjack_release_color("\033[95m");  // Bright Magenta
    // lumberjack_release_color("\033[96m");  // Bright Cyan
    // lumberjack_release_color("\033[31m");  // Red
    // lumberjack_release_color("\033[35m");  // Magenta
    // lumberjack_release_color("\033[33m");  // Yellow
    // lumberjack_release_color("\033[37m");  // White

    // lumberjack_release_color("0: ");  // Bright Red
    // lumberjack_release_color("1: ");  // Bright Green 
    // lumberjack_release_color("2: ");  // Bright Blue
    // lumberjack_release_color("3: ");  // Bright Yellow
    // lumberjack_release_color("4: ");  // Bright Magenta
    // lumberjack_release_color("5: ");  // Bright Cyan
    // lumberjack_release_color("6: ");  // Red
    // lumberjack_release_color("7: ");  // Magenta
    // lumberjack_release_color("8: ");  // Yellow
    // lumberjack_release_color("9: ");  // White


    lumberjack_release_color("0: \033[91m");  // Bright Red
    lumberjack_release_color("1: \033[92m");  // Bright Green 
    lumberjack_release_color("2: \033[94m");  // Bright Blue
    lumberjack_release_color("3: \033[93m");  // Bright Yellow
    lumberjack_release_color("4: \033[95m");  // Bright Magenta
    lumberjack_release_color("5: \033[96m");  // Bright Cyan
    lumberjack_release_color("6: \033[37m");  // White
    lumberjack_release_color("7: \033[90m");  // Grey


    //lumberjack_print_queue();

    // LUMBERJACK_COLOR_CODES(
    //     "\033[91m",  // Bright Red
    //     "\033[92m",  // Bright Green  
    //     "\033[94m",  // Bright Blue
    //     "\033[93m",  // Bright Yellow
    //     "\033[95m",  // Bright Magenta
    //     "\033[96m",  // Bright Cyan
    //     "\033[31m",  // Red
    //     "\033[35m",  // Magenta
    //     "\033[33m",  // Yellow
    //     "\033[37m"   // White
    // );

}

///////////////////////////////////////////////////////////////////////////////
//
// State
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    bool active;                // has there been a keypress in the last 60 secs?
    uint16_t last_event_time;   // time of last key event
} lumberjack_state_t;

static lumberjack_state_t state = {0};


///////////////////////////////////////////////////////////////////////////////
//
// Key Press Lifecycle
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    uint16_t keycode;    // key that was pressed
    uint16_t down_time;  // time it was pressed DOWN
    uint16_t up_time;    // time is was released UP
    const char* color;   // log colour allocated to the key press
} keypress_t;

// log of all currently depressed keys
static keypress_t depressed_keys[LUMBERJACK_MAX_TRACKED_KEYS];
static uint8_t num_depressed_keys = 0;


/**
 * @brief Tracks an individual key press: call on key DOWN event
 * 
 * @param keycode The QMK keycode that was pressed / released
 * @param record The corresponding key event record
 * 
 * @return keypress_t with keycode, down_time and color for this keypress, or
 *         an empty keypress_t if the corresponding press wasn't recorded
 *         (i.e. MAX_TRACKED_KEYS exceeded)
 * 
 * @note Tracks up to LUMBERJACK_MAX_TRACKED_KEYS simultaneous key presses
 */
static keypress_t track_pressed_key(uint16_t keycode, keyrecord_t *record) {
    
    // if not a key press DOWN, return empty keypress
    if (!record->event.pressed) {
        keypress_t empty_keypress = {0};
        return empty_keypress;    
    }

    // if no tracking space left, return empty keypress
    if (num_depressed_keys >= LUMBERJACK_MAX_TRACKED_KEYS) {
        keypress_t empty_keypress = {0};
        return empty_keypress;
    }

    // record the key DOWN time
    depressed_keys[num_depressed_keys].keycode = keycode;
    depressed_keys[num_depressed_keys].down_time = record->event.time;

    // assign a (not recently used) colour to the key press
    depressed_keys[num_depressed_keys].color = lumberjack_get_next_color();
    num_depressed_keys++;
    return depressed_keys[num_depressed_keys-1];
}


/**
 * @brief Provides data on an individual key press: call on key UP event
 * 
 * @details Key press data is supplied a maximum of one time, after which
 *          it is deleted from the tracking system.
 * 
 * @param keycode The QMK keycode that was released
 * @param record The corresponding key event record
 * 
 * @return keypress_t with keycode, down_time, up_time and color for this
 *         keypress, or an empty keypress_t if the corresponding press
 *         wasn't recorded
 * 
 * @note Tracks up to LUMBERJACK_MAX_TRACKED_KEYS simultaneous key presses.
 */
static keypress_t data_for_released_key(uint16_t keycode, keyrecord_t *record) {
    
    // if not a key release UP, return empty keypress
    if (record->event.pressed) {
        keypress_t empty_keypress = {0};
        return empty_keypress;
    }

    // search for the key DOWN data
    for (uint8_t i = 0; i < num_depressed_keys; i++) {
        if (depressed_keys[i].keycode == keycode) {
            
            // record the key UP time
            depressed_keys[i].up_time = record->event.time;

            // copy data to return struct
            keypress_t keypress = depressed_keys[i];

            // remove key from depressed_keys list
            for (uint8_t j = i; j < num_depressed_keys - 1; j++) {
                depressed_keys[j] = depressed_keys[j + 1];
            }
            num_depressed_keys--;

            // release key press's colour for future re-use
            lumberjack_release_color(depressed_keys[i].color);
            return keypress;
        }
    }

    // if key DOWN data not found, return empty keypress
    keypress_t empty_keypress = {0};
    return empty_keypress;
}


///////////////////////////////////////////////////////////////////////////////
//
// Handling Key Presses
//
///////////////////////////////////////////////////////////////////////////////

// Handles all key presses/releases
bool pre_process_record_lumberjack(uint16_t current_keycode, keyrecord_t *record) {

    // calculate delta
    uint16_t delta = record->event.time - state.last_event_time;
    state.last_event_time = record->event.time;

    // track keypress & get data for it
    keypress_t keypress_data;
    if (record->event.pressed) { // key DOWN
        // starting key press: start tracking & assign a color
        keypress_data = track_pressed_key(current_keycode, record);
    } else { // key UP
        // ending key press: calculate hold duration
        keypress_data = data_for_released_key(current_keycode, record);
    }

    // prettify keycode string (convert to human-readable keycode or hex string, then pad for alignment)
    char keycode_string[LUMBERJACK_KEYCODE_LENGTH+1];
    #ifdef KEYCODE_STRING_ENABLE
        lumberjack_right_align_string(keycode_string, LUMBERJACK_KEYCODE_LENGTH+1, get_keycode_string(current_keycode));
    #else
        char hex_string[6+1];
        lumberjack_keycode_to_hex_string(hex_string, current_keycode);
        lumberjack_right_align_string(keycode_string, LUMBERJACK_KEYCODE_LENGTH+1, hex_string);
    #endif

    // if key press failed to be tracked, log error
    if (keypress_data.keycode == 0) {
        uprintf("%s - NOT TRACKED\n", keycode_string);
        return true;
    }

    // if first key press in over 60 seconds: log without delta
    if (!state.active && record->event.pressed) { // 
        // log without a delta
        uprintf("%s %s  |  DOWN  |                    |%s\n", keypress_data.color, keycode_string, RESET_COLOR);
        state.active = true;
        return true;
    }

    // convert delta to string
    char delta_string[6+1]; // 5 digits, 1 minus sign, 1 terminating NULL
    if (delta > 61000) { // if well over 60s, must be negative
        // note: negative deltas only arise in process_record, not in pre_process_record
        delta = -(int)delta;
        lumberjack_uint_to_string(delta_string, (int)delta);
        lumberjack_prepend_char(delta_string, '-');
    }
    else
        lumberjack_uint_to_string(delta_string, delta);
    
    // prettify delta string (i.e. pad it for alignment)
    char padded_delta_string[6+1];
    lumberjack_right_align_string(padded_delta_string, 6+1, delta_string);

    // log normally
    if (record->event.pressed) {
        uprintf("%s %s  |  DOWN  |  Delta: %s ms  |%s\n", keypress_data.color, keycode_string, padded_delta_string, RESET_COLOR);
    } else {
        uint16_t hold_duration = keypress_data.up_time - keypress_data.down_time;
        uprintf("%s %s  |  UP    |  Delta: %s ms  |  Hold: %u ms%s\n", keypress_data.color, keycode_string, padded_delta_string, hold_duration, RESET_COLOR);
    }

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
