#include "lumberjack_utils.h"
#include "lumberjack_config.h"
#include "lumberjack_tracking.h"

///////////////////////////////////////////////////////////////////////////////
//
// Hook to Config
//
///////////////////////////////////////////////////////////////////////////////

// xprintf wrapper; used for all lumberjack printing
#define lj_printf(fmt, ...)                                            \
    do {                                                               \
        if (lumberjack_is_logging()) xprintf(fmt, ##__VA_ARGS__);      \
    } while (0)


///////////////////////////////////////////////////////////////////////////////
//
// String Buffer Sizes
//
///////////////////////////////////////////////////////////////////////////////

// Non-coloured pipe: ANSI colour + 1 char pipe + LUMBERJACK_ANSI_RESET + null
#define MAX_PIPE_LEN ( LUMBERJACK_MAX_ANSI_CODE_LEN + 1                \
                       + LUMBERJACK_ANSI_RESET_LEN + 1 )

// Delta: 5 chars (uint16_t max is 65536) + null
#define MAX_DELTA_LEN ( 5 + 1 )

// Handedness: " [L]" + null 
#define MAX_HANDEDNESS_LEN ( 4 + 1 )

// Hex Keycode: "0x" + 4 hex digits (for 16-bit keycode: 2^16 = 16^4) + null
#define MAX_HEX_KEYCODE_LEN ( 2 + 4 + 1 )

// Pretty Keycode: LUMBERJACK_KEYCODE_LENGTH + null
#define MAX_KEYCODE_LEN LUMBERJACK_KEYCODE_LENGTH + 1

// Handedness + Keycode: subtract extra null terminator
#define MAX_HANDED_KEYCODE_LEN ( MAX_HANDEDNESS_LEN + MAX_KEYCODE_LEN - 1 )

// MAX_KEYCODE_LEN + MAX_HANDEDNESS_LEN must fit in a uint8_t
#if MAX_KEYCODE_LEN > 200 + 1
    #error "Maximum LUMBERJACK_KEYCODE_LENGTH is 200 chars"
#endif
// MAX_KEYCODE_LEN must be at least as big as MAX_HEX_KEYCODE_LENGTH to avoid
// possible buffer overflow in prettify_handed_keycode()
#if MAX_KEYCODE_LEN < MAX_HEX_KEYCODE_LEN
    #error "Minimum LUMBERJACK_KEYCODE_LENGTH is 6 chars"
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Handedness
//
///////////////////////////////////////////////////////////////////////////////

char chordal_hold_handedness(keypos_t key);
char lightshift_handedness(keypos_t key);

// Return either lightshift or chordal hold's handedness, or '?' if neither
// in use
char handedness(keypos_t key) {
    #ifdef LIGHTSHIFT_ENABLE
        return lightshift_handedness(key);
    #elif CHORDAL_HOLD
        return chordal_hold_handedness(key);
    #endif
    
    return '?';
}


///////////////////////////////////////////////////////////////////////////////
//
// Pretty String Manipulation
//
///////////////////////////////////////////////////////////////////////////////

// Get non-coloured pipe for use within a coloured log entry
// (dest return buffer must be at least MAX_PIPE_LEN+1 chars)
static void non_colored_pipe(char* dest, const char* color) {
    uint8_t pipe_len = lumberjack_str_len(LUMBERJACK_ANSI_RESET, MAX_PIPE_LEN)
                       + 1 + lumberjack_str_len(color, MAX_PIPE_LEN);
    if (pipe_len <= MAX_PIPE_LEN) {
        strcpy(dest, LUMBERJACK_ANSI_RESET);
        strcat(dest, "|");
        strcat(dest, color);
    }
    else { // in case of overflow, use naked pipe instead
        strcpy(dest, "|");
    }
}


// Get 4-char string for handedness, e.g. " [L]"
static void prettify_handedness(char* dest, char handedness) {
    // if no handedness, return empty string
    if (handedness == '?') dest[0] = '\0';

    
    dest[0] = '<';
    dest[1] = handedness;
    dest[2] = '>';
    dest[3] = ' ';
    dest[4] = '\0';
}


// Get human-readable string for a given keycode
// (dest buffer must be at least MAX_KEYCODE_LEN chars)
static void prettify_keycode(char* dest, uint16_t keycode) {
    #ifdef KEYCODE_STRING_ENABLE
        lumberjack_safe_copy(dest, MAX_KEYCODE_LEN,
                             get_keycode_string(keycode));
    #else
        lumberjack_keycode_to_hex_string(dest, MAX_KEYCODE_LEN, keycode);
    #endif
}


// Get human-readable, right-aligned string for a given hand & keycode
// (dest buffer must be at least MAX_KEYCODE_LEN + MAX_HANDEDNESS_LEN chars)
static void prettify_handed_keycode(char* dest, char hand, uint16_t keycode) {
    
    // prettify handedness and call it combined_string
    char combined_string[MAX_HANDED_KEYCODE_LEN];
    prettify_handedness(combined_string, hand);
    
    // prettify keycode and concatenate it to combined_string
    char keycode_string[MAX_KEYCODE_LEN];
    prettify_keycode(keycode_string, keycode);
    strcat(combined_string, keycode_string);
    
    // right align & copy to dest
    lumberjack_right_align_string(dest, MAX_HANDED_KEYCODE_LEN,
                                  combined_string);
}


// Get right-aligned 5-char string for integer delta, e.g. "  243"
// (UINT16_MAX interpreted as no delta, returns "    -")
static void prettify_delta(char* dest, uint16_t delta) {
    if (delta == UINT16_MAX) {
        strcpy(dest, "    -");
    }
    else {
        char delta_string[MAX_DELTA_LEN];
        lumberjack_uint_to_string(delta_string, MAX_DELTA_LEN, delta);
        lumberjack_right_align_string(dest, MAX_DELTA_LEN, delta_string);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Writing to Log (Pre-PR, i.e. Physical Keypresses)
//
///////////////////////////////////////////////////////////////////////////////

// Log single line (DOWN, colour)
static void log_down_color(const char* keycode_string,
                           const char* delta_string, const char* color) {
    char pipe[MAX_PIPE_LEN];
    non_colored_pipe(pipe, color);
    lj_printf("%s%s  %s--DOWN--%s  Delta: %s ms  %s%s\n",
              color, keycode_string, pipe, pipe, delta_string, pipe,
              LUMBERJACK_ANSI_RESET);
}


// Log single line (DOWN, monochrome)
static void log_down_mono(const char* keycode_string,
                          const char* delta_string) {
    // monochrome versions have different text, for better readability
    lj_printf("%s  |  DOWN  |  Delta: %s ms  |\n",
              keycode_string, delta_string);
}


// Log single line (UP, colour)
static void log_up_color(const char* keycode_string, const char* delta_string,
                         uint16_t duration, const char* color) {
    char pipe[MAX_PIPE_LEN];
    non_colored_pipe(pipe, color);
    lj_printf("%s%s      UP      Delta: %s ms  %s  Hold: %u ms%s\n",
              color, keycode_string, delta_string, pipe, duration,
              LUMBERJACK_ANSI_RESET);
}


// Log single line (UP, monochrome)
static void log_up_mono(const char* keycode_string, const char* delta_string,
                        uint16_t duration) {
    // monochrome versions have different text, for better readability
    lj_printf("%s  |  UP    |  Delta: %s ms  |  Hold: %u ms\n",
              keycode_string, delta_string, duration);
}


// Log single line (untracked)
static void log_untracked(const char* keycode_string) {
    lj_printf("%s - NOT TRACKED\n", keycode_string);
}


// Triages a normal key event out to the logging functions
static void log_normally(const keypress_t* keypress_data,
                         const char* keycode_string,
                         const char* delta_string, bool pressed) {
    if (pressed) {
        if (lumberjack_color()) {
            log_down_color(keycode_string, delta_string, keypress_data->color);
        }
        else {
            log_down_mono(keycode_string, delta_string);
        }
    } else {
        const uint16_t duration
                           = keypress_data->up_time - keypress_data->down_time;
        if (lumberjack_color()) {
            log_up_color(keycode_string, delta_string, duration,
                         keypress_data->color);
        }
        else {
            log_up_mono(keycode_string, delta_string, duration);
        }
    }
}


// Log a (pre-PR) physical key event (DOWN or UP) to the console
void lumberjack_log_input(const keypress_t* keypress_data,
                          uint16_t keycode, uint16_t delta,
                          bool pressed) {

    // convert hand & keycode to pretty string
    char hand = handedness(keypress_data->key);
    char keycode_string[MAX_KEYCODE_LEN + MAX_HANDEDNESS_LEN];
    prettify_handed_keycode(keycode_string, hand, keycode);

    // convert delta to pretty string
    char delta_string[MAX_DELTA_LEN];
    prettify_delta(delta_string, delta);

    // if key press was not tracked, log warning
    if (keypress_data->keycode == 0) {
        log_untracked(keycode_string);
        return;
    }

    // otherwise log normally
    log_normally(keypress_data, keycode_string, delta_string, pressed);
}


///////////////////////////////////////////////////////////////////////////////
//
// Writing to Log (PR & Post-PR)
//
///////////////////////////////////////////////////////////////////////////////

// Log a PR or post-PR event
void lumberjack_log_interpreted_event(const char *prefix, uint16_t keycode,
                                      keyrecord_t *record) {

    // convert keycode to pretty string
    char keycode_string[MAX_KEYCODE_LEN];
    prettify_keycode(keycode_string, keycode);

    // log
    lj_printf("%s: %s - pressed: %u, tapcount: %u, interrupted: %u, "
              "time: %5u, col: %2u, row: %2u\n",
              prefix,
              keycode_string,
              record->event.pressed,
              record->tap.count,
              record->tap.interrupted,
              record->event.time,
              record->event.key.col,
              record->event.key.row);
}
