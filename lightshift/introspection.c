// Hook into lightshift tapping terms
#ifndef LIGHTSHIFT_USER_TAPPING_TERM
    uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
        if (is_lightshift(keycode)) {
            return get_lightshift_term(keycode, record);
        }
        // fall back to QMK core implementation, from action_tapping.c
        #ifdef DYNAMIC_TAPPING_TERM_ENABLE
            return g_tapping_term;
        #else
            return TAPPING_TERM;
        #endif
    }
#endif

// Disable PERMISSIVE_HOLD for lightshift keys
#if defined(PERMISSIVE_HOLD) && !defined(LIGHTSHIFT_USER_PERMISSIVE_HOLD)
    bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
        if (is_lightshift(keycode)) {
            return false;
        }
        return true;
    }
#endif

// Disable CHORDAL_HOLD for lightshift keys
#if defined(CHORDAL_HOLD) && !defined(LIGHTSHIFT_USER_CHORDAL_HOLD)
    bool get_chordal_hold(uint16_t tap_hold_keycode,
                      keyrecord_t* tap_hold_record,
                      uint16_t other_keycode,
                      keyrecord_t* other_record) {
        if (is_lightshift(tap_hold_keycode)) return true;
        return get_chordal_hold_default(tap_hold_record, other_record);
    }
#endif

// Disable Flow Tap for lightshift keys
#if defined(FLOW_TAP_TERM) && !defined(LIGHTSHIFT_USER_FLOW_TAP)
    uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t* record,
                               uint16_t prev_keycode) {
        if (is_lightshift(keycode)) return 0;
        if (is_flow_tap_key(keycode) && is_flow_tap_key(prev_keycode)) {    
            return FLOW_TAP_TERM;
        }
        return 0;
    }
#endif
