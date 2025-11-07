#include "lightshift.h"
#include "lightshift_debug.h"
#include "lightshift_tracking.h"
#include "lightshift_state.h"
#include "lightshift_drop.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);


///////////////////////////////////////////////////////////////////////////////
//
// Low Lightshift Tapping Term (vs. Missed Opposite Side Shifts)
//
// Fixes:
// ------
//     opp-side  s-i    :  'si'  ->  'I'       (Graphite)
//
///////////////////////////////////////////////////////////////////////////////
//
// Extended Tapping Term (vs. Accidental Same-Side Shifts)
//
// Fixes:
// ------
//     s-t-r  :  'Tring' -> 'string'   (Graphite)
//
///////////////////////////////////////////////////////////////////////////////

// Returns the current tapping term for lightshift keys
uint16_t calculate_term(uint16_t keycode, const keyrecord_t *record) {
    
    // extended tapping term
    if (lghts_use_extended_tt(record->event.key)) {
        return LIGHTSHIFT_EXTENDED_TAPPING_TERM;
    }

    // low regular tapping term
    if (is_lightshift(keycode)) return LIGHTSHIFT_TAPPING_TERM;

    // failsafe
    return TAPPING_TERM;
}


///////////////////////////////////////////////////////////////////////////////
//
// Dropshift (vs. Accidental Double Shifts)
//
// Fixes:
// ------
//     s-i-t   :  'IT'  ->  'It'   (Graphite)
//
///////////////////////////////////////////////////////////////////////////////

static void clear_any_double_shift(uint16_t keycode,
                                   const keyrecord_t* record) {
    // leave shifts active if:
    if (!record->event.pressed // event is a release, not a press
            || lghts_is_layer_or_mod(keycode, record) // layer switch / mod key
            || lightshift_allow_double(keycode, record) // double shift allowed
            || lghts_non_shift_mods_active()) { // other mods active (shortcut)
        return;
    }

    // clear any already used shift
    uint16_t shift_keycode = lghts_set_double_inactive(keycode);
    if (shift_keycode != KC_NO) {
        lghts_clear_modtap_mods(shift_keycode);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// QMK Hooks
//
///////////////////////////////////////////////////////////////////////////////

// Called on _physical_ keypresses
bool pre_process_record_lightshift(uint16_t keycode, keyrecord_t *record) {
    // track lightshift state
    lghts_track_ppr(keycode, record);
    return true;
}

// Called after QMK decides TAP or HOLD
bool process_record_lightshift(uint16_t keycode, keyrecord_t* record) {
    if (lghts_dropshift()) {
        // if this key may not be double shifted, clear any double shift
        clear_any_double_shift(keycode, record);
        // track lightshift state
        lghts_track_pr(keycode, record);
    }
    return true;
}

// Called continuously while there are unresolved TAP / HOLD decisions
// Hooked via get_tapping_term() in introspection.c
uint16_t get_lightshift_term(uint16_t keycode, const keyrecord_t *record) {
    // return contextual tapping term for lightshift keys
    const uint16_t term = calculate_term(keycode, record);
    // debug print (noisy!)
    // lghts_dprintf("%s: TT=%u", get_keycode_string(keycode), term);
    return term;
}
