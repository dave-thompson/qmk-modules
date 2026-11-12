#include "lightshift_state.h"
#include "lightshift_tracking.h"
#include "lightshift_debug.h"

///////////////////////////////////////////////////////////////////////////////
//
// State Machine
//
///////////////////////////////////////////////////////////////////////////////

/**
*
*                          ┌─────────────────┐
*                          │     INACTIVE    │
*                          └────────┬────────┘
*                                   │
*                                 shift
*                                pressed
*                                   │
*                                   v
*                          ┌─────────────────┐
*        ┌─────────────────│    UNRESOLVED   │─────────────────┐
*        │                 └────────┬────────┘                 │
*        │                          │                          │
*        │              ┌───────────┴───────────┐              │
*        │              │                       │              │
*     tapping        opp-side               same-side        shift
*      term            key                     key          released
*     (=hold)           │                       │            (=tap)
*        │              │                       │              │
*        │              v                       v              │
*        │      ┌───────────────┐       ┌───────────────┐      │
*        │      │ LIGHTSHIFT TT │       │  EXTENDED TT  │      │
*        │      └───┬─────────┬─┘       └───────┬───────┘      │
*        │          │         │                 │              │
*        │       tapping    shift             shift            │
*        │        term     released          released          │
*        │       (=hold)    (=tap)            (=tap)           │
*        │          │         │                 │              │
*        v          v         │                 v              │
*    ┌───────────────────┐    │         ┌───────────────┐      │
*    │     SHIFTING      │    │         │   RELEASED,   │      │
*    └─────────┬─────────┘    │         │   EXTENDED    │      │
*              │              │         └───────┬───────┘      │
*              │              │                 │              │
*            shift            │              any next          │
*           released          │             key event          │
*              │              │                 │              │
*              v              v                 v              v
*    ┌───────────────────────────────────────────────────────────────┐
*    │                             INACTIVE                          │
*    └───────────────────────────────────────────────────────────────┘
*
*    Transition Triggers:
*    --------------------
*    tapping term => process_record      (QMK has resolved to HOLD)
*    all others   => pre_process_record
*
*    NB: EXTENDED TT -> SHIFTING is also possible, should the Extended
*        Tapping Term expire.  The default ETT of 65,535ms renders this
*        impossible in real-world use, but shorter, custom ETTs may
*        allow for it.
*
*    NB: SHIFTING doesn't change Lightshift's behaviour, as QMK stops
*        asking for the tapping term once it resolves a HOLD.  It is
*        tracked so that the state machine (and debug output) show when
*        a lightshift is shifting.
*
*    NB: RELEASED, EXTENDED exists because QMK decides a released
*        mod-tap only after pre_process_record, so the Extended TT must
*        persist until then.
*
*/

///////////////////////////////////////////////////////////////////////////////
//
// Shiftability
//
///////////////////////////////////////////////////////////////////////////////

// Is this a layer / modifier key (excluding MT / LT keys)?
// For use in pre_process_record, where MT / LT keys are not yet resolved
static bool is_layer_or_mod_ppr(uint16_t keycode) {
    return IS_MODIFIER_KEYCODE(keycode) // KC_LEFT_CTRL to KC_RIGHT_GUI
           || (keycode >= QK_LAYER_MOD
               && keycode <= QK_PERSISTENT_DEF_LAYER_MAX) // layers & OSM
           || (keycode == QK_LAYER_LOCK); // layer lock
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
// PPR Tracking
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
        lightshift_t shift = get_lghts(i);
        if (shift.state == SHIFT_UNRESOLVED) {
            bool same_hand = lightshift_handedness(record->event.key)
                           == lightshift_handedness(shift.key);
            lghts_set_state(shift.key, same_hand ?
                            SHIFT_EXTENDED_TT : SHIFT_LIGHTSHIFT_TT);
        }
    }
}


// Update State: Lightshift released
static void release_shift(const keyrecord_t *record) {
    keypos_t key = record->event.key;
    switch (lghts_get_state(key)) {

            // released EXTENDED TTs => RELEASED EXTENDED
            // (we need to retain the correct TT in state, as QMK doesn't
            //  decide mod-taps until later in the processing cycle)
            case SHIFT_EXTENDED_TT:
                lghts_set_state(key, SHIFT_RELEASED_EXTENDED);
                break;

            // other releases: set INACTIVE
            case SHIFT_UNRESOLVED:
            case SHIFT_LIGHTSHIFT_TT:
            case SHIFT_SHIFTING:
                lghts_set_state(key, SHIFT_INACTIVE);
                break;

            default:
                break;
        }
}


// Update State: Stop tracking any outstanding RELEASED EXTENDEDs
static void update_expired_states(void) {
    for (uint8_t i = 0; i < num_active_lghts(); i++) {
        lightshift_t shift = get_lghts(i);
        if (shift.state == SHIFT_RELEASED_EXTENDED) {
            lghts_set_state(shift.key, SHIFT_INACTIVE);
        }
    }
}


// Hook from pre_process_record()
// Updates: new shift presses/releases; handedness decisions
void lghts_track_ppr(uint16_t keycode, const keyrecord_t *record) {
    keypos_t key = record->event.key;

    // RELEASED EXTENDED => INACTIVE
    update_expired_states();

    // UNRESOLVED => INACTIVE, LIGHTSHIFT TT => INACTIVE,
    // SHIFTING => INACTIVE, EXTENDED TT => RELEASED EXTENDED
    if (!record->event.pressed && is_tracked_lghts(key)) {
        // Tracking ceases here (in pre_process_record) to ensure
        // ceasation when used in combos, etc.
        release_shift(record);
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
// PR Tracking
//
///////////////////////////////////////////////////////////////////////////////

// Hook from process_record()
// Updates: tapping term expiries
void lghts_track_pr(uint16_t keycode, const keyrecord_t *record) {

    // only pressed & held lightshifts (i.e. tapping term just expired)
    if (!record->event.pressed || record->tap.count
                               || !is_lightshift(keycode)) {
        return;
    }

    // UNRESOLVED => SHIFTING, LIGHTSHIFT TT => SHIFTING,
    // EXTENDED TT => SHIFTING
    keypos_t key = record->event.key;
    switch (lghts_get_state(key)) {
        case SHIFT_UNRESOLVED:
        case SHIFT_LIGHTSHIFT_TT:
        case SHIFT_EXTENDED_TT: // include Extended for short, custom ETTs
            lghts_set_state(key, SHIFT_SHIFTING);
            break;
        default:
            break;
    }
}
