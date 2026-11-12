#include "lightshift.h"
#include "lightshift_debug.h"
#include "lightshift_tracking.h"
#include "lightshift_state.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);


///////////////////////////////////////////////////////////////////////////////
//
// Lightshift Tapping Term vs. Extended Tapping Term
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
bool process_record_lightshift(uint16_t keycode, keyrecord_t *record) {
    // track lightshift state
    lghts_track_pr(keycode, record);
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
