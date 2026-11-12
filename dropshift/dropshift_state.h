/**
 * dropshift_state.h
 *
 * Stores state of currently held shift keys.
 */

#pragma once
#include "quantum.h"

// # of simultaneously held shifts tracked; any more will not be dropped
#define DROPSHIFT_MAX_SHIFTS 2 // very rare to hold > 2 shift keys at once

typedef enum {
    DROPSHIFT_INACTIVE,        // Not held, or Dropped (slot is free)
    DROPSHIFT_SINGLE,          // Held; not yet consumed
    DROPSHIFT_DOUBLE,          // Held; already consumed, only doubles left
    DROPSHIFT_STATE_COUNT,
  // Update debug messages in dropshift_state.c when changing
} dropshift_state_t;

// Maps a physical shift key to its current dropshift state
typedef struct {
    keypos_t key;             ///< physical key location
    uint16_t keycode;         ///< keycode
    dropshift_state_t state;  ///< current state: single or double?
} dropshift_t;

/**
 * @brief Returns the tracking slot at index
 *
 * Slots in state DROPSHIFT_INACTIVE are free and hold no shift.
 *
 * @param index slot index, from 0 to DROPSHIFT_MAX_SHIFTS - 1
 *
 * @return the slot at index (never NULL for a valid index)
 */
dropshift_t *drps_get_shift(uint8_t index);


/**
 * @brief Returns the slot tracking the given key
 *
 * @param key Key position of the shift
 *
 * @return the slot tracking key, or NULL if key is not a tracked shift
 */
dropshift_t *drps_find_shift(keypos_t key);


/**
 * @brief Start tracking a newly held shift, in state DROPSHIFT_SINGLE
 *
 * Ignored if the key is already tracked, or if too many shifts are held.
 *
 * @param key Key position of the held shift
 * @param keycode Keycode of the held shift (to clear its mods on drop)
 */
void drps_add_shift(keypos_t key, uint16_t keycode);


/**
 * @brief Update a tracked shift's state, or stop tracking it
 *
 * Pass in DROPSHIFT_INACTIVE to stop tracking a shift (freeing its slot).
 *
 * @param shift Slot of the shift to be updated
 * @param state desired state enum (above)
 */
void drps_set_state(dropshift_t *shift, dropshift_state_t state);
