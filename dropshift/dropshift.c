#include "dropshift.h"
#include "dropshift_debug.h"
#include "dropshift_state.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);


///////////////////////////////////////////////////////////////////////////////
//
// State Machine
//
///////////////////////////////////////////////////////////////////////////////

/**
*
*                   ┌─────────────────────────────────────┐
*                   │               INACTIVE              │
*                   └──────────────────┬──────────────────┘
*                                      │
*                                 shift held
*                                      │
*                                      v
*                    ┌────────────────────────────┐
*                    │      SINGLE SHIFTING       │────┐
*                    └──────┬─────────────────────┘    │
*                           │                          │
*                       consuming                    shift
*                       shiftable                   released
*                       keypress                       │
*                           │                          │
*                           v                          │
*                    ┌────────────────────────────┐    │
*                    │      DOUBLE SHIFTING       │    │
*                    └──────┬────────────┬────────┘    │
*                           │            │             │
*                      disallowed        └─── shift ───┤
*                       shiftable            released  │
*                       keypress                       │
*                           │                          │
*                           v                          v
*                   ┌─────────────────────────────────────┐
*                   │               INACTIVE              │
*                   └─────────────────────────────────────┘
*
* 
*    All transitions occur in process_record.
*
*/


///////////////////////////////////////////////////////////////////////////////
//
// Interpreting Keypresses
//
///////////////////////////////////////////////////////////////////////////////

// Is this keypress a letter (A-Z)?
static bool is_letter(uint16_t keycode, const keyrecord_t *record) {

    // convert tapped MT / LT keys to tap keycode
    if (record->tap.count > 0) {
        if (IS_QK_MOD_TAP(keycode)) {
            keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        }
        if (IS_QK_LAYER_TAP(keycode)) {
            keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
        }
    }

    return keycode >= KC_A && keycode <= KC_Z;
}


// Is this a layer / modifier key (including held MT / LT keys)?
// Only call with data from process_record(), not from pre_process_record()
static bool is_layer_or_mod(uint16_t keycode, const keyrecord_t *record) {
    return IS_MODIFIER_KEYCODE(keycode) // KC_LEFT_CTRL to KC_RIGHT_GUI
           || (keycode >= QK_LAYER_MOD
               && keycode <= QK_PERSISTENT_DEF_LAYER_MAX) // layers & OSM
           || (keycode == QK_LAYER_LOCK) // layer lock
           || ((IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode))
               && record->tap.count == 0);  // held MT / LT
}


// Is this a plain (non mod-tap) shift key?
static bool is_plain_shift(uint16_t keycode) {
    return keycode == KC_LEFT_SHIFT || keycode == KC_RIGHT_SHIFT;
}


// Is this a one-shot shift key?
static bool is_oneshot_shift(uint16_t keycode) {
    return IS_QK_ONE_SHOT_MOD(keycode)
           && (QK_ONE_SHOT_MOD_GET_MODS(keycode) & MOD_LSFT); // L or R
}


// Is this the press of a shift key that is now held?
// i.e. a plain shift press, or a mod-tap / one-shot shift that QMK resolved
// to HOLD
static bool is_shift_hold(uint16_t keycode, const keyrecord_t *record) {
    if (!record->event.pressed) return false;
    if (is_plain_shift(keycode)) return true;
    if (is_oneshot_shift(keycode)) {
        return record->tap.count == 0            // one-shot is held
               || !keymap_config.oneshot_enable; // o-s in 'plain shift' mode
    }
    return IS_QK_MOD_TAP(keycode)
           && (QK_MOD_TAP_GET_MODS(keycode) & MOD_LSFT) // any shift (L or R)
           && record->tap.count == 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Mods
//
///////////////////////////////////////////////////////////////////////////////

// Are there any mods active, other than shift?
static bool non_shift_mods_active(void) {
    uint8_t mods = get_mods();
    mods &= ~(MOD_MASK_SHIFT);
    return mods != 0;
}


// Clears any mods held by the given shift key; e.g. LSFT_T(KC_S),
// OSM(MOD_LSFT) or KC_LSFT clears left shift
static void clear_shift_mods(uint16_t keycode) {
    uint8_t mod_mask;

    if (is_plain_shift(keycode)) {
        mod_mask = MOD_BIT(keycode);
    } else {
        uint8_t mods_5bit = is_oneshot_shift(keycode)
                                ? QK_ONE_SHOT_MOD_GET_MODS(keycode)
                                : QK_MOD_TAP_GET_MODS(keycode);

        // Convert 5-bit mods to 8-bit mask
        // e.g. 0x12 (right shift) -> 0x20; 0x02 (left shift) -> 0x02
        mod_mask =
            // first bit (L/R) ? remaining 4-bits, then 4 0's : just the 4-bits
            (mods_5bit & 0x10) ? ((mods_5bit & 0x0F) << 4) : mods_5bit;
    }

    // Remove the modifiers from the currently set modifiers
    unregister_mods(mod_mask);
    drps_dprintf("%s - Cleared", get_keycode_string(keycode));
}


///////////////////////////////////////////////////////////////////////////////
//
// User-Overridable Config
//
///////////////////////////////////////////////////////////////////////////////

// Returns true if this keycode, once shifted, should prevent further keycodes
// from being shifted afterwards
bool dropshift_consume_single_raw(uint16_t keycode,
                                  const keyrecord_t *record) {
    return is_letter(keycode, record);
}


// Returns true if this keycode may be shifted even after another keycode
// has been shifted already
bool dropshift_allow_double_raw(uint16_t keycode,
                                const keyrecord_t *record) {
    return !is_letter(keycode, record);
}


__attribute__((weak)) bool dropshift_consume_single(uint16_t keycode,
                                                  const keyrecord_t *record) {
    return dropshift_consume_single_raw(keycode, record);
}


__attribute__((weak)) bool dropshift_allow_double(uint16_t keycode,
                                                  const keyrecord_t *record) {
    return dropshift_allow_double_raw(keycode, record);
}


///////////////////////////////////////////////////////////////////////////////
//
// Tracking
//
///////////////////////////////////////////////////////////////////////////////

// Update State: Shift key newly held
static void track_new_shift(uint16_t keycode, const keyrecord_t *record) {
    // INACTIVE => SINGLE SHIFTING
    if (is_shift_hold(keycode, record)) {
        drps_add_shift(record->event.key, keycode);
    }
}


// Update State: Held shift released
static void release_held_shift(const keyrecord_t *record) {
    // SINGLE SHIFTING => INACTIVE, DOUBLE SHIFTING => INACTIVE (1 of 2)
    dropshift_t *shift = drps_find_shift(record->event.key);
    if (shift) {
        drps_set_state(shift, DROPSHIFT_INACTIVE);
    }
}


// Update State: Consume single shifts, if this keypress was shifted by them
static void consume_single_shifts(uint16_t keycode,
                                  const keyrecord_t *record) {
    // don't mark shift as used if...
    if (!record->event.pressed // release rather than press
        || is_layer_or_mod(keycode, record) // layer switch or mod
        || !dropshift_consume_single(keycode, record)) { // should not consume
            return;
    }

    // SINGLE SHIFTING => DOUBLE SHIFTING
    // (future shift uses are now doubles)
    for (uint8_t i = 0; i < DROPSHIFT_MAX_SHIFTS; i++) {
        dropshift_t *shift = drps_get_shift(i);
        if (shift->state == DROPSHIFT_SINGLE) {
                drps_dprintf("Single consumed by %s",
                             get_keycode_string(keycode));
                drps_set_state(shift, DROPSHIFT_DOUBLE);
        }
    }
}


// Stops tracking any (first found) DOUBLE SHIFTING key, and returns
// its keycode, or KC_NO if none found
// DOUBLE SHIFTING => INACTIVE (2 of 2)
static uint16_t set_double_inactive(uint16_t keycode) {
    for (uint8_t i = 0; i < DROPSHIFT_MAX_SHIFTS; i++) {
        dropshift_t *shift = drps_get_shift(i);
        if (shift->state == DROPSHIFT_DOUBLE) {
            // set state to INACTIVE (= stop tracking)
            drps_dprintf("Shift dropped by disallowed %s double",
                         get_keycode_string(keycode));
            drps_set_state(shift, DROPSHIFT_INACTIVE);
            return shift->keycode;
        }
    }
    return KC_NO;
}


///////////////////////////////////////////////////////////////////////////////
//
// Dropping Shift
//
///////////////////////////////////////////////////////////////////////////////


// If this key may not be double shifted, clear any double shift
static void clear_any_double_shift(uint16_t keycode,
                                   const keyrecord_t* record) {
    // leave shifts active if:
    if (!record->event.pressed // event is a release, not a press
            || is_layer_or_mod(keycode, record) // layer switch / mod key
            || dropshift_allow_double(keycode, record) // double shift allowed
            || non_shift_mods_active()) { // other mods active (shortcut)
        return;
    }

    // clear any already used shift
    uint16_t shift_keycode = set_double_inactive(keycode);
    if (shift_keycode != KC_NO) {
        clear_shift_mods(shift_keycode);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// QMK Hooks
//
///////////////////////////////////////////////////////////////////////////////

// Called after QMK decides TAP or HOLD
bool process_record_dropshift(uint16_t keycode, keyrecord_t* record) {

    // if this key may not be double shifted, clear any double shift
    clear_any_double_shift(keycode, record);

    // track held shifts
    if (!record->event.pressed) {
        // Tracking ceases here (in process_record), rather than in
        // pre_process_record, to ensure all potentially shifted keys are
        // processed before tracking ends
        release_held_shift(record);
    }
    consume_single_shifts(keycode, record);
    track_new_shift(keycode, record);

    return true;
}
