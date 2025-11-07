/**
 * lightshift_debug.h
 */

#pragma once
#include "quantum.h"

/**
 * @brief Is lightshift debugging turned on?
 * 
 * @return true if LIGHTSHIFT_DEBUG is defined
 * 
 */
static inline bool lghts_debug(void) {
    #ifdef LIGHTSHIFT_DEBUG
        return true;
    #else
        return false;
    #endif
}


/**
 * @brief Print debug messages to the console, only if LIGHTSHIFT_DEBUG defined
 * 
 */
#define lghts_dprintf(fmt, ...)                                     \
    do {                                                            \
        if (lghts_debug()) xprintf(fmt "\n", ##__VA_ARGS__);   \
    } while (0)
