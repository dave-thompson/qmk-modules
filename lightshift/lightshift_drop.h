/**
 * lightshift_drop.h
 * 
 * Dropshift prevents accidental 'double' shifts, such as "HEllo".  See the
 * README for more details.
 * 
 * It categorises keys pressed during a shift into singles and doubles.
 * The first shiftable key is always a single. Once that single is 'consumed',
 * all subsequent keys are considered doubles. By default, only letters (A-Z)
 * consume the single shift.
 * 
 * When a letter double occurs, the shift is cleared.  Once a shift is cleared,
 * it remains cleared until shift is released and repressed.  (Non-letter
 * doubles are ignored.)
 * 
 * You can change which keys consume the single, and which keys clear the shift
 * on a double, by overriding lightshift_consume_single() and / or
 * lightshift_allow_double() in keymap.c.
 * 
 */

#pragma once
#include "quantum.h"

/**
 * @brief Checks if the given key should consume the single shift
 * 
 * By default, singles are only consumed by letters (A-Z).
 * 
 * Even if a shift is consumed by this function, subsequent letters will still
 * be shifted if they are designated as double-allowed in
 * lightshift_allow_double().
 * 
 * You may override this function in your keymap.c for custom behaviour.
 * (See example below.)
 * 
 * @param keycode Keycode to be checked
 * @param record Record to be checked
 * 
 * @return true if key should consume any active shift
 * 
 * @code
 * bool lightshift_consume_single(uint16_t keycode,
 *                                const keyrecord_t *record) {
 *
 *     // consume shift when typing @, preventing @Jess when intending @jess
 *     // (@ is a shifted KC_2)
 *      if (keycode == KC_2) return true;
 *
 *     // fallback to the default implementation for other keycodes
 *     return lightshift_consume_single_raw(keycode, record);
 * }
 * @endcode
 */
bool lightshift_consume_single(uint16_t keycode, const keyrecord_t *record);

/**
 * @brief Checks if double shift is allowed for the given key
 * 
 * By default, this function drops shift only on letter (A-Z) doubles,
 * to avoid inadvertent double capitalization, while allowing punctuation
 * and other keys to be shifted multiple times with a single shift hold.
 * 
 * You may override this function in your keymap.c for custom behaviour.
 * (See example below.)
 * 
 * @param keycode Keycode to be checked
 * @param record Record to be checked
 * 
 * @return true if shift should not be cleared, even if this is a double
 * 
 * @code
 * bool lightshift_allow_double(uint16_t keycode, const keyrecord_t *record) {
 *     
 *     // prevent accidental double-quote when typing: I"ve
 *     // (double quote is a shifted single quote, aka KC_QUOTE)
 *     if (keycode == KC_QUOTE) return false;
 * 
 *     // fallback to the default implementation for other keycodes
 *    return lightshift_allow_double_raw(keycode, record);
 * }
 * @endcode
 */
bool lightshift_allow_double(uint16_t keycode, const keyrecord_t *record);

/**
 * @brief Raw version of lightshift_consume_single()
 */
bool lightshift_consume_single_raw(uint16_t keycode,
	                               const keyrecord_t *record);

/**
 * @brief Raw version of lightshift_allow_double()
 */
bool lightshift_allow_double_raw(uint16_t keycode, const keyrecord_t *record);


/**
 * @brief Clears any mods matching MT keycode
 * 
 * @param keycode Keycode for which corresponding shift should be cleared,
 *        e.g. LSFT_T(KC_S) clears left shift
 */
void lghts_clear_modtap_mods(uint16_t keycode);

/**
 * @brief Are any mods active, other than shift?
 * 
 * @return true if non-shift mods are active
 */
bool lghts_non_shift_mods_active(void);
