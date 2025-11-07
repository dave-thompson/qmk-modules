/**
 * Lightshift - QMK Community Module - Home-Row Shifts at Speed
 * 
 * See README.md in this folder for a detailed setup guide.
 * 
 */

#pragma once
#include "quantum.h"

#ifndef LIGHTSHIFT_TAPPING_TERM
	#define LIGHTSHIFT_TAPPING_TERM 150
#endif
#if LIGHTSHIFT_TAPPING_TERM > UINT16_MAX
    #error "LIGHTSHIFT_TAPPING_TERM must be no more than 65,535ms"
#endif

#ifndef LIGHTSHIFT_EXTENDED_TAPPING_TERM
	#define LIGHTSHIFT_EXTENDED_TAPPING_TERM UINT16_MAX
#endif
#if LIGHTSHIFT_EXTENDED_TAPPING_TERM > UINT16_MAX
    #error "LIGHTSHIFT_EXTENDED_TAPPING_TERM must be no more than 65,535ms"
#endif

/**
 * @brief Convenience method for access to DROPSHIFT_ENABLE parameter
 */
inline bool lghts_dropshift(void) {
    #ifdef DROPSHIFT_ENABLE
        return true;
    #else
        return false;
    #endif
}

/**
 * @brief Returns the correct tapping term for a lightshift key press
 */
uint16_t get_lightshift_term(uint16_t keycode, const keyrecord_t *record);
