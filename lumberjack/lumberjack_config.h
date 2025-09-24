/**
 * @file lumberjack_config.h
 * 
 * @author dave-thompson
 */

#pragma once

#include "quantum.h"

///////////////////////////////////////////////////////////////////////////////
//
// Config Defaults
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LUMBERJACK_MAX_TRACKED_KEYS
    #define LUMBERJACK_MAX_TRACKED_KEYS 10 // one per finger
#endif

#ifndef LUMBERJACK_KEYCODE_LENGTH
    #define LUMBERJACK_KEYCODE_LENGTH 15 // 6-char modifier, 7-char basic
                                         // keycode, 2 parentheses
#endif

#define LUMBERJACK_MAX_DELTA 60000 // before wraparound at 65536ms


///////////////////////////////////////////////////////////////////////////////
//
// Access Methods
//
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Convenience method for access to LUMBERJACK_COLOR config parameter
 */
inline bool lumberjack_color(void) {
    #ifdef LUMBERJACK_COLOR
        return true;
    #else
        return false;
    #endif
}


///////////////////////////////////////////////////////////////////////////////
//
// Runtime Config
//
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Turn on lumberjack's logging
 * 
 * Logging is on by default.  You do not need to call this unless you disabled
 * logging previously.
 * 
 */
void lumberjack_on(void);


/**
 * @brief Turn off lumberjack's logging
 */
void lumberjack_off(void);


/**
 * @brief true if lumberjack is on, otherwise false
 */
bool lumberjack_is_logging(void);


/**
 * @brief Toggles logging when LUMBERJ key tapped
 * 
 * @note Toggle occurs when LUMBERJ key is pressed DOWN, so you will see just
 *       the LUMBERJ UP event when turning logging on, and just the LUMBERJ
 *       DOWN event when turning logging off.
 * 
 * Takes no action if passed keycode is not LUMBERJ, or if key was released
 * rather than pressed.
 * 
 * @param current_keycode keycode currently being processed
 * @param *record record currently being processed
 * 
 * @return true if keycode was LUMBERJ, otherwise false
 */
bool lumberjack_toggle_if_lumberj_key(uint16_t current_keycode,
                                      keyrecord_t *record);


///////////////////////////////////////////////////////////////////////////////
//
// Colours Config
//
///////////////////////////////////////////////////////////////////////////////

#define LUMBERJACK_RESET_COLOR "\033[0m"
#define LUMBERJACK_RESET_COLOR_LEN 7
#define LUMBERJACK_MAX_ANSI_COLOR_CODE_LEN 9

void lumberjack_init_colors(void);
