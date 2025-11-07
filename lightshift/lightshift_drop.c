#include "lightshift_drop.h"
#include "lightshift_debug.h"

///////////////////////////////////////////////////////////////////////////////
//
// Interpreting Keypresses
//
///////////////////////////////////////////////////////////////////////////////

// Is this keypress a letter (A-Z)?
bool is_letter(uint16_t keycode, const keyrecord_t *record) {

    // convert tapped MT / LT keys to tap keycode
    if (record->tap.count == 1) {
        if (IS_QK_MOD_TAP(keycode)) {
            keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        }
        if (IS_QK_LAYER_TAP(keycode)) {
            keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
        }
    }

    return keycode >= KC_A && keycode <= KC_Z;
}


///////////////////////////////////////////////////////////////////////////////
//
// User-Overridable Config
//
///////////////////////////////////////////////////////////////////////////////

// Returns true if this keycode, once shifted, should prevent further keycodes
// from being shifted afterwards
bool lightshift_consume_single_raw(uint16_t keycode,
                                   const keyrecord_t *record) {
    return is_letter(keycode, record);
}


// Returns true if this keycode may be shifted even after another keycode
// has been shifted already
bool lightshift_allow_double_raw(uint16_t keycode,
                                 const keyrecord_t *record) {
    return !is_letter(keycode, record);
}


__attribute__((weak)) bool lightshift_consume_single(uint16_t keycode,
                                                   const keyrecord_t *record) {
    return lightshift_consume_single_raw(keycode, record);
}


__attribute__((weak)) bool lightshift_allow_double(uint16_t keycode,
                                                   const keyrecord_t *record) {
    return lightshift_allow_double_raw(keycode, record);
}


///////////////////////////////////////////////////////////////////////////////
//
// Interrogating Mod State
//
///////////////////////////////////////////////////////////////////////////////

// Are there any mods active, other than shift?
bool lghts_non_shift_mods_active(void) {
    uint8_t mods = get_mods();
    mods &= ~(MOD_MASK_SHIFT);
    return mods != 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Clearing Mods
//
///////////////////////////////////////////////////////////////////////////////

// Clears any mods matching MT keycode; e.g. LSFT_T(KC_S) clears left shift
void lghts_clear_modtap_mods(uint16_t keycode) {
    uint8_t mods_5bit = QK_MOD_TAP_GET_MODS(keycode);
    
    // Convert 5-bit mods to 8-bit mask
    // e.g. 0x12 (right shift) -> 0x20; 0x02 (left shift) -> 0x02
    uint8_t mod_mask =
        // first bit (L/R) ? remaining 4-bits, then 4 0's : just the 4-bits
        (mods_5bit & 0x10) ? ((mods_5bit & 0x0F) << 4) : mods_5bit;
    
    // Remove the modifiers from the currently set modifiers
    unregister_mods(mod_mask);
    lghts_dprintf("%s - Cleared", get_keycode_string(keycode));
}
