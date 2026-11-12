/**
 * dropshift_debug.h
 */

#pragma once
#include "quantum.h"

/**
 * @brief Is dropshift debugging turned on?
 * 
 * @return true if DROPSHIFT_DEBUG is defined
 * 
 */
static inline bool drps_debug(void) {
    #ifdef DROPSHIFT_DEBUG
        return true;
    #else
        return false;
    #endif
}


/**
 * @brief Print debug messages to the console, only if DROPSHIFT_DEBUG defined
 * 
 */
#define drps_dprintf(fmt, ...)                                      \
    do {                                                            \
        if (drps_debug()) xprintf(fmt "\n", ##__VA_ARGS__);   \
    } while (0)
