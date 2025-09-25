#include "quantum.h"
#include "lumberjack_config.h"
#include "lumberjack_color_queue.h"

///////////////////////////////////////////////////////////////////////////////
//
// Runtime Config
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LUMBERJACK_OFF_AT_BOOT
    static bool is_logging = true;
#else
    static bool is_logging = false;
#endif

void lumberjack_off(void) {
    is_logging = false;
}

void lumberjack_on(void) {
    is_logging = true;
}

bool lumberjack_is_logging(void) {
    return is_logging;
}

// Toggle logging when LUMBERJ key tapped
bool lumberjack_toggle_if_lumberj_key(uint16_t current_keycode,
                                      const keyrecord_t *record) {
    if (current_keycode == LUMBERJ) {
        if (record->event.pressed) {
            is_logging = !is_logging;
        }
        return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////
//
// Colours Config
//
///////////////////////////////////////////////////////////////////////////////

// Only declare five colours to be used in log output.  This is enough for most
// use cases, while avoiding distracting colours like red and ensuring all
// colours are easily distinguished.  If more than five keys are depressed
// simultaneously, the extra keys will be printed in the default colour
// (typically white / black).
void lumberjack_init_colors(void) {
    if (lumberjack_color()) {
        // all ANSI codes below must be shorter than
        // LUMBERJACK_MAX_ANSI_CODE_LEN in lumberjack_config.h
        lumberjack_add_color_to_queue("\033[35m");  // Magenta
        lumberjack_add_color_to_queue("\033[32m");  // Green
        lumberjack_add_color_to_queue("\033[33m");  // Yellow
        lumberjack_add_color_to_queue("\033[34m");  // Purple
        lumberjack_add_color_to_queue("\033[36m");  // Cyan
    }
}
