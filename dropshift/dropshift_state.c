#include "dropshift_state.h"
#include "dropshift_debug.h"

///////////////////////////////////////////////////////////////////////////////
//
// State
//
///////////////////////////////////////////////////////////////////////////////

// fixed tracking slots; a slot is free when its state is DROPSHIFT_INACTIVE
// (zero-initialised, so all slots start free)
static dropshift_t shift_keys[DROPSHIFT_MAX_SHIFTS];


///////////////////////////////////////////////////////////////////////////////
//
// Debugging
//
///////////////////////////////////////////////////////////////////////////////

#ifdef DROPSHIFT_DEBUG

    static const char* debug_text[] = {
      "INACTIVE",
      "SINGLE SHIFTING",
      "DOUBLE SHIFTING",
    };

    // @brief Generates a text representation for a given enum state
    // @param state for which text is needed, e.g. DROPSHIFT_SINGLE
    // @return text for the given enum state, e.g. "SINGLE SHIFTING"
    static inline const char* state_debug_text(int state) {
      if (state >= 0 && state < DROPSHIFT_STATE_COUNT) {
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

// Returns the tracking slot at index
dropshift_t *drps_get_shift(uint8_t index) {
    return index < DROPSHIFT_MAX_SHIFTS ? &shift_keys[index] : NULL;
}


// Returns the slot tracking the given key, or NULL if untracked
dropshift_t *drps_find_shift(keypos_t key) {
    for (uint8_t i = 0; i < DROPSHIFT_MAX_SHIFTS; i++) {
        if (shift_keys[i].state != DROPSHIFT_INACTIVE
                && KEYEQ(shift_keys[i].key, key)) {
            return &shift_keys[i];
        }
    }
    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
// Setters
//
///////////////////////////////////////////////////////////////////////////////

// Update a tracked shift's state; DROPSHIFT_INACTIVE frees its slot
void drps_set_state(dropshift_t *shift, dropshift_state_t state) {
    shift->state = state;
    drps_dprintf("%s - d_state: %s", get_keycode_string(shift->keycode),
                 state_debug_text(state));
}


// Start tracking a newly held shift, as a SINGLE
void drps_add_shift(keypos_t key, uint16_t keycode) {

    // no action if already tracked
    if (drps_find_shift(key)) return;

    // track in the first free slot
    for (uint8_t i = 0; i < DROPSHIFT_MAX_SHIFTS; i++) {
        if (shift_keys[i].state == DROPSHIFT_INACTIVE) {
            shift_keys[i].key = key;
            shift_keys[i].keycode = keycode;
            drps_set_state(&shift_keys[i], DROPSHIFT_SINGLE);
            return;
        }
    }

    // no free slot
    drps_dprintf("WARNING: Max shifts (%d) exceeded", DROPSHIFT_MAX_SHIFTS);
}
