#include "lightshift_state.h"
#include "lightshift_debug.h"

///////////////////////////////////////////////////////////////////////////////
//
// State
//
///////////////////////////////////////////////////////////////////////////////

// # of simult. MT shift presses; any more will be normal MTs, not lightshifts
#define MAX_TRACKED_SHIFTS 2 // very rare to have > 2 home-row shift keys
                             // even rarer to press > 2 at the same time
                             
// array of current shift presses
static shift_t shift_keys[MAX_TRACKED_SHIFTS];
static uint8_t shifts_count = 0;

// sentinel value, in case requested shift data not in state array
#define SHIFT_NO UINT8_MAX
// check MAX_TRACKED_SHIFTS < SHIFT_NO (to avoid interfering with sentinel)
_Static_assert(MAX_TRACKED_SHIFTS < SHIFT_NO, 
               "MAX_TRACKED_SHIFTS must be no more than 254");


///////////////////////////////////////////////////////////////////////////////
//
// Debugging
//
///////////////////////////////////////////////////////////////////////////////

#ifdef LIGHTSHIFT_DEBUG

    static const char* debug_text[] = {
      "INACTIVE",
      "UNRESOLVED",
      "LIGHTSHIFT TT",
      "EXTENDED TT",
      "RELEASED, EXTENDED",
      "SINGLE SHIFTING",
      "DOUBLE SHIFTING",
    };

    // @brief Generates a text representation for a given enum state
    // @param state for which text is needed, e.g. SHIFT_EXTENDED_TT
    // @return text for the given enum state, e.g. "EXTENDED TT"
    static inline const char* state_debug_text(int state) {
      if (state >= 0 && state < LIGHTSHIFT_STATE_COUNT) {
        return debug_text[state];
      }
      return "BUG";
    }

#else

    static inline const char* state_debug_text(int state) {
        return "";
    }

#endif


///////////////////////////////////////////////////////////////////////////////
//
// Getters
//
///////////////////////////////////////////////////////////////////////////////

// Returns tracking data for the lightshift at index
shift_t get_lghts(uint8_t index) {
    // Return not a valid index, return INACTIVE shift
    if (index >= shifts_count) {
        shift_t inactive = {0};
        inactive.keycode = KC_NO;
        inactive.state = SHIFT_INACTIVE;
        return inactive;        
    }
    return shift_keys[index];
}


// returns the number of active lightshifts
uint8_t num_active_lghts(void) {
    return shifts_count;
}


// Returns the shift_keys array index for the given record
// Returns SHIFT_NO if this is not a tracked shift key
static uint8_t get_index(keypos_t key) {
    for (uint8_t i = 0; i < shifts_count; i++) {
        if (KEYEQ(shift_keys[i].key, key)) {
            return i;
        }
    }
    return SHIFT_NO;
}


// Returns the current state for a given shift key
lightshift_state_t lghts_get_state(keypos_t key) {
    uint8_t index = get_index(key);
    if (index == SHIFT_NO) return SHIFT_INACTIVE;
    return shift_keys[index].state;
}


// Returns true if this is a mod-tap shift key, even if not currently tracked
bool is_lightshift(const uint16_t keycode) {
    return IS_QK_MOD_TAP(keycode) && (QK_MOD_TAP_GET_MODS(keycode) & 0x02);
}


// Returns true if the key is a tracked lightshift
bool is_tracked_lghts(keypos_t key) {
    return get_index(key) != SHIFT_NO;
}


// Returns true if the key is a lightshift that should use an Extended TT
bool lghts_use_extended_tt(keypos_t key) {
    for (uint8_t i = 0; i < shifts_count; i++) {
        if (KEYEQ(shift_keys[i].key, key)) {
            return (shift_keys[i].state == SHIFT_EXTENDED_TT
                 || shift_keys[i].state == SHIFT_RELEASED_EXTENDED);
        }
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////
//
// Setters
//
///////////////////////////////////////////////////////////////////////////////

// Start tracking a new lightshift keypress; returns the index of the newly
// added shift, or SHIFT_NO if the shift could not be added
static uint8_t add_shift(keypos_t shift_key, uint16_t keycode) {
    
    // no action if no space in state array
    if (shifts_count >= MAX_TRACKED_SHIFTS) {
        lghts_dprintf("WARNING: Max shifts (%d) exceeded", MAX_TRACKED_SHIFTS);
        return SHIFT_NO;
    }

    // add new shift to state array
    shift_keys[shifts_count].key = shift_key;
    shift_keys[shifts_count].keycode = keycode;
    shift_keys[shifts_count].state = SHIFT_UNRESOLVED;
    shifts_count++;
    return shifts_count - 1;
}


// Stop tracking an old lightshift keypress
static void remove_shift(uint8_t index) {
    if (shifts_count == 0 || index >= shifts_count) return;
    for (uint8_t i = index; i < shifts_count - 1; i++) {
        shift_keys[i] = shift_keys[i + 1];
    }
    shifts_count--;
}


// Update a specific lightshift's state, or start / stop tracking it
void lghts_set_state(keypos_t key, lightshift_state_t state) {

    // if invalid state, ignore & warn
    if (state >= LIGHTSHIFT_STATE_COUNT) {
        lghts_dprintf("BUG: Invalid state %d", state);
        return;
    }

    // if shift is untracked, ignore
    uint8_t shift_index = get_index(key);
    if (shift_index == SHIFT_NO && state != SHIFT_UNRESOLVED) return;
    
    uint16_t keycode;

    // start tracking new shift presses
    if (state == SHIFT_UNRESOLVED) {
        keycode = layer_switch_get_action(key).code;
        shift_index = add_shift(key, keycode);
    }

    // stop tracking old shift presses
    else if (state == SHIFT_INACTIVE) {
        remove_shift(shift_index);
    }

    // update state normally
    else {
        shift_keys[shift_index].state = state;
    }

    // debug logging
    if (shift_index != SHIFT_NO) {
        keycode = shift_keys[shift_index].keycode;
        lghts_dprintf("%s - state: %s", get_keycode_string(keycode),
                      state_debug_text(state));
    }
}
