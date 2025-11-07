#include "lightshift_state.h"
#include "lightshift_tracking.h"
#include "lightshift_debug.h"
#include "lightshift_drop.h"

///////////////////////////////////////////////////////////////////////////////
//
// State Machine
//
///////////////////////////////////////////////////////////////////////////////

/**
*     
*                                  ┌─────────────────┐
*                                  │     INACTIVE    │
*                                  └────────┬────────┘
*                                           │
*                                      [pp] shift
*                                        pressed
*                                           │
*                                           v      
*                                  ┌─────────────────┐
*          ┌───────────────────────│    UNRESOLVED   │─────────────────┐
*          │                       └────────┬────────┘                 │
*          │                                │                          │
*          │                    ┌───────────┴───────────┐              │
*          │                    │                       │              │
*       tapping          [pp] opp-side           [pp] same-side    [pp] shift
*        term                  key                     key          released
*          │                    │                       │            (=tap)
*          │                    v                       v              │
*          │         ┌─────────────────────┐    ┌───────────────┐      │
*          │         │    LIGHTSHIFT TT    │    │  EXTENDED TT  │      │
*          │         └───┬─────────────┬───┘    └───────┬───────┘      │
*          │             │             │                │              │
*          │          tapping     [pp] shift       [pp] shift          │
*          │           term        released         released           │
*          │             │          (=tap)           (=tap)            │
*          │             │             │                │              │
*          │             │             │                │              │
*          v             v             │                v              │
*    ┌────────────────────────────┐    │        ┌───────────────┐      │
*    │      SINGLE SHIFTING       │    │        │   RELEASED,   │      │
*    └─────┬─────────────┬────────┘    │        │   EXTENDED    │      │
*          │             │             │        └───────┬───────┘      │
*      consuming         └─── shift ───┐                │              │
*      shiftable             released  │                │              │
*      keypress                        │                │              │
*          │                           │                │              │
*          v                           │                │              │
*    ┌────────────────────────────┐    │                │              │
*    │      DOUBLE SHIFTING       │    │                │              │
*    └──────┬────────────┬────────┘    │                │              │
*           │            │             │                │              │
*      disallowed        └─── shift ───┐          [pp] any next        │
*       shiftable            released  │            key event          │
*       keypress                       │                │              │
*           │                          │                │              │
*           v                          v                v              v
*    ┌─────────────────────────────────────────────────────────────────────┐
*    │                            INACTIVE                                 │
*    └─────────────────────────────────────────────────────────────────────┘
*
*    Transition Triggers:
*    --------------------
*    [pp]   => pre_process_record  (for Extended Tapping Term)
*    others => process_record      (for Dropshift)
*
* 
*    NB: EXTENDED_TT -> SINGLE_SHIFTING is also possible, should the
*        Extended Tapping Term expire. The default ETT of 65,535ms renders
*        this impossible in real-world use, but shorter, custom ETTs may
*        allow for it.
* 
*/

///////////////////////////////////////////////////////////////////////////////
//
// Shiftability
//
///////////////////////////////////////////////////////////////////////////////

// Is this a layer / modifier key (excluding MT / LT keys)?
// For use in pre_process_record, where MT / LT keys are not yet resolved
bool is_layer_or_mod_ppr(uint16_t keycode) {
    return IS_MODIFIER_KEYCODE(keycode) // KC_LEFT_CTRL to KC_RIGHT_GUI
           || (keycode >= QK_LAYER_MOD
               && keycode <= QK_PERSISTENT_DEF_LAYER_MAX) // layers & OSM
           || (keycode == QK_LAYER_LOCK); // layer lock
}


// Is this a layer / modifier key (including MT / LT keys)?
// Only call with data from process_record(), not from pre_process_record()
bool lghts_is_layer_or_mod(uint16_t keycode, const keyrecord_t *record) {
    return is_layer_or_mod_ppr(keycode)
           ||((IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode))
               && record->tap.count == 0);  // held MT / LT
}


///////////////////////////////////////////////////////////////////////////////
//
// Handedness 
//
///////////////////////////////////////////////////////////////////////////////

// if chordal hold installed, use highest available of:
// 1. user's custom lightshift_handedness()
// 2. chordal_hold_handedness()
#ifdef CHORDAL_HOLD

    __attribute__((weak)) char lightshift_handedness(keypos_t pos) {
        return chordal_hold_handedness(pos);
    }

// otherwise, use highest available of:
// 1. user's custom lightshift_handedness()
// 2. user's custom chordal_hold_handedness()
// 3. user's custom chordal_hold_layout[][]
// 4. lightshift's best guess
#else

    // 4. best guess (the achordion algo)
    static char best_guess_handedness(keypos_t pos) {
        #ifdef SPLIT_KEYBOARD
            return pos.row < MATRIX_ROWS / 2 ? 'L' : 'R';
        #else // NON_SPLIT
            // check position relative to midpoint of larger dimension
            int midpoint = (MATRIX_COLS > MATRIX_ROWS ?
                           MATRIX_COLS : MATRIX_ROWS) / 2;
            int position = MATRIX_COLS > MATRIX_ROWS ? pos.col : pos.row;
            return position < midpoint ? 'L' : 'R';
        #endif
    }

    // 3. chordal_hold_layout[][] - return 0 (unless overridden by user)
    __attribute__((weak)) const char
        chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = {{0}};

    // 2. chordal_hold_handedness() - use chordal_hold_layout[][] if
    // available, otherwise return 0 (unless overridden by user)
    __attribute__((weak)) char chordal_hold_handedness(keypos_t key) {
        // note: pgm_read_byte() is expensive on firmware space
        // save space with a custom chordal_hold_handedness() in your keymap
        char first = (char)pgm_read_byte(&chordal_hold_layout[0][0]);
        return first ?
            (char)pgm_read_byte(&chordal_hold_layout[key.row][key.col]) : 0;
    }

    // 1. use chordal_hold_handedness() if available, else best_guess
    __attribute__((weak)) char lightshift_handedness(keypos_t pos) {
        char hand = chordal_hold_handedness(pos);
        return hand ? hand : best_guess_handedness(pos);
    }

#endif


///////////////////////////////////////////////////////////////////////////////
//
// Extended Tapping Term
//
///////////////////////////////////////////////////////////////////////////////

// Update State: Lightshift pressed
static void track_new_shift(const keyrecord_t *record) {
    // newly pressed lightshifts => UNRESOLVED
    lghts_set_state(record->event.key, SHIFT_UNRESOLVED);
}


// Update State: Decide Tapping Term
// - if an UNRESOLVED lightshift was pressed before this key, update its
//   status based on whether this is a same or opposite side key
static void decide_tapping_term(uint16_t keycode, const keyrecord_t *record) {

    // leave lightshifts UNRESOLVED if
    if ((!record->event.pressed) // key released, not pressed
        || is_layer_or_mod_ppr(keycode)) // key is (non-MT/LT) layer or mod key
    {
        return;
    }
    
    // otherwise, resolve any UNRESOLVEDs
    for (uint8_t i = 0; i < num_active_lghts(); i++) {
        shift_t shift = get_lghts(i);
        if (shift.state == SHIFT_UNRESOLVED) {
            bool same_hand = lightshift_handedness(record->event.key)
                           == lightshift_handedness(shift.key);
            lghts_set_state(shift.key, same_hand ?
                            SHIFT_EXTENDED_TT : SHIFT_LIGHTSHIFT_TT);
        }
    }
}


// Update State: Non-held lightshift released
static void release_non_held_shift(const keyrecord_t *record) {
    keypos_t key = record->event.key;
    switch (lghts_get_state(key)) {

            // released EXTENDED TTs => RELEASED EXTENDED
            // (we need to retain the correct TT in state, as QMK doesn't
            //  decide mod-taps until later in the processing cycle)
            case SHIFT_EXTENDED_TT:
                lghts_set_state(key, SHIFT_RELEASED_EXTENDED);
                break;

            // other non-held releases: set INACTIVE
            case SHIFT_UNRESOLVED:
            case SHIFT_LIGHTSHIFT_TT:
                lghts_set_state(key, SHIFT_INACTIVE);
                break;

            default:
                break;
        }
}


// Update State: Stop tracking any outstanding RELEASED EXTENDEDs
static void update_expired_states(void) {
    for (uint8_t i = 0; i < num_active_lghts(); i++) {
        shift_t shift = get_lghts(i);
        if (shift.state == SHIFT_RELEASED_EXTENDED) {
            lghts_set_state(shift.key, SHIFT_INACTIVE);
        }
    }
}


// Extended TT: Hook from pre_process_record()
// Updates: new shift presses; handedness decisions; releasing non-held shifts
void lghts_track_ppr(uint16_t keycode, const keyrecord_t *record) {
    keypos_t key = record->event.key;

    // RELEASED EXTENDED => INACTIVE
    update_expired_states();

    // UNRESOLVED => INACTIVE, LIGHTSHIFT TT => INACTIVE,
    // EXTENDED TT => RELEASED EXTENDED
    if (!record->event.pressed && is_tracked_lghts(key)) {
        // QMK Non-held lightshifts: Tracking ceases here (in
        // pre_process_record) to ensure ceasation when used in combos, etc.
        release_non_held_shift(record);
    }

    // UNRESOLVED => LIGHTSHIFT TT, UNRESOLVED => EXTENDED TT
    decide_tapping_term(keycode, record);

    // INACTIVE => UNRESOLVED
    if (record->event.pressed && is_lightshift(keycode)) {
        track_new_shift(record);
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Dropshift
//
///////////////////////////////////////////////////////////////////////////////

// Stops tracking any (first found) SHIFT_DOUBLE_SHIFTING key, and returns
// its keycode
// DOUBLE SHIFTING => INACTIVE (2 of 2)
uint16_t lghts_set_double_inactive(uint16_t keycode) {
    for (uint8_t i = 0; i < num_active_lghts(); i++) {
        shift_t shift = get_lghts(i);
        if (shift.state == SHIFT_DOUBLE_SHIFTING) {
            // set state to INACTIVE (= stop tracking)
            lghts_dprintf("Shift dropped by disallowed %s double",
                          get_keycode_string(keycode));
            lghts_set_state(shift.key, SHIFT_INACTIVE);
            return shift.keycode;
        }
    }
    return KC_NO;
}


// Update State: Held lightshift released
static void release_held_shift(const keyrecord_t *record) {
    // Released shift => SHIFT_INACTIVE
    keypos_t key = record->event.key;
    switch (lghts_get_state(key)) {
            case SHIFT_SINGLE_SHIFTING:
            case SHIFT_DOUBLE_SHIFTING:
                lghts_set_state(key, SHIFT_INACTIVE);
                break;
            default:
                break;
        }
}


// Update State: Consume single shifts, if this keypress was shifted by them
static void consume_single_shifts(uint16_t keycode,
                                  const keyrecord_t *record) {
    // don't mark shift as used if...
    if (!record->event.pressed // release rather than press
        || lghts_is_layer_or_mod(keycode, record) // layer switch or mod
        || !lightshift_consume_single(keycode, record)) { // should not consume
            return;
    }

    // future shift uses are now doubles
    for (uint8_t i = 0; i < num_active_lghts(); i++) {
        shift_t shift = get_lghts(i);
        if (shift.state == SHIFT_SINGLE_SHIFTING) {
                lghts_dprintf("Single consumed by %s",
                              get_keycode_string(keycode));
                lghts_set_state(shift.key, SHIFT_DOUBLE_SHIFTING);
        }
    }
}


// Update State: Tapping term expired
static void handle_tapping_term_expiries(uint16_t keycode,
                                         const keyrecord_t *record) {
    // if a pressed and held lightshift (i.e. tapping_term just expired)
    if (is_lightshift(keycode) && record->event.pressed
                               && !record->tap.count) {
        keypos_t key = record->event.key;
        switch (lghts_get_state(key)) {
            case SHIFT_UNRESOLVED:
            case SHIFT_LIGHTSHIFT_TT:
            case SHIFT_EXTENDED_TT: // include Extended for short, custom ETTs
                lghts_set_state(key, SHIFT_SINGLE_SHIFTING);
                break;
            default:
                break;
        }
    }
}


// Dropshift: Hook from process_record()
// Update: tapping_term expiries; consuming singles; releasing held shifts
void lghts_track_pr(uint16_t keycode, const keyrecord_t *record) {
    keypos_t key = record->event.key;

    // SINGLE SHIFTING => INACTIVE, DOUBLE SHIFTING => INACTIVE (1 of 2)
    if (!record->event.pressed && is_tracked_lghts(key))
    {
        // QMK Held lightshifts: Tracking ceases here (in process_record)
        // to ensure all potentially shifted keys are processed before
        // tracking ends (pre-process record is too early)
        release_held_shift(record);
    }

    // SINGLE SHIFTING => DOUBLE SHIFTING
    consume_single_shifts(keycode, record);

    // UNRESOLVED => SINGLE SHIFTING, LIGHTSHIFT TT => SINGLE SHIFTING
    handle_tapping_term_expiries(keycode, record);
}
