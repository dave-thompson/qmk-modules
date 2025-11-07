/**
 * lightshift_state.h
 * 
 * Stores state of a currently pressed lightshift.
 */

#pragma once
#include "quantum.h"

typedef enum {
    SHIFT_INACTIVE,                // Not pressed, or Cleared (or not tracked)
    SHIFT_UNRESOLVED,              // Newly pressed, awaiting more data
    SHIFT_LIGHTSHIFT_TT,           // Pressed, use Lightshift TT
    SHIFT_EXTENDED_TT,             // Pressed, use Extended TT
    SHIFT_RELEASED_EXTENDED,       // Just released, use Extended TT
    SHIFT_SINGLE_SHIFTING,         // Held; not yet consumed
    SHIFT_DOUBLE_SHIFTING,         // Held; already consumed, only doubles left
    LIGHTSHIFT_STATE_COUNT,
  // Update debug messages in lightshift_state.c when changing
} lightshift_state_t;

// Maps a physical shift key to its current lightshift state
typedef struct {
    keypos_t key;             ///< physical key location
    uint16_t keycode;         ///< keycode
    lightshift_state_t state; ///< current state: pressed? which tt?
} shift_t;

/**
 * @brief Returns state data for the active lightshift at index
 * 
 * @param index index of the active lightshift for which state data is needed
 *
 * @return state data, including state, keycode and key
 *         (or SHIFT_INACTIVE, KC_NO if no lightshift at index)
 */
shift_t get_lghts(uint8_t index);


/**
 * @brief Returns the number of active (pressed & not cleared) lightshifts
 *
 * @return number of currently active lightshifts
 */
uint8_t num_active_lghts(void);


/**
 * @brief Update a specific lshift's state, or add/remove a press to/from state
 * 
 * Pass in SHIFT_UNRESOLVED to add a new shift press, or
 * SHIFT_INACTIVE to remove an old shift press.
 * 
 * @param key Key position of the lightshift to be updated
 * @param state desired state enum (above)
 */
void lghts_set_state(keypos_t key, lightshift_state_t state);


/**
 * @brief Get the current state of a given shift key
 * 
 * @param key Key position of the lightshift whose state is needed
 * 
 * @return state enum of the requested key or SHIFT_INACTIVE if no state found
 */
lightshift_state_t lghts_get_state(keypos_t key);


/**
 * @brief Is this key a lightshift that should use an Extended TT?
 * 
 * @param key Key position to be checked
 * 
 * @return true  if this is a lightshift that should use an Extended TT
 *         false if this is a lightshift that should NOT use an Extended TT
 *         false if this is not a lightshift release
 */
bool lghts_use_extended_tt(keypos_t key);


/**
 * @brief Determines if this keycode is a mod-tap shift key
 * 
 * @param keycode Keycode that may be a mod-tap shift
 * 
 * @return true if this is a mod-tap shift keycode
 * 
 * @note Unlike is_tracked_lghts(), this returns true even for untracked
 *       lightshifts, including non-pressed lightshifts and lightshifts which
 *       did not fit in the tracking array.  Use it to identify lightshifts
 * 		   which are not currently pressed.
 */

bool is_lightshift(uint16_t keycode);


/**
 * @brief Determines if this key is a pressed & tracked lightshift key
 * 
 * @param key Key position which may be a pressed lightshift
 * 
 * @return true if this is a currently pressed lightshift
 * 
 * @note Unlike is_lightshift(), this returns true even for released
 * 		   lightshifts that have changed keycode to a non-mod-tap shift
 *       mid-keypress.  Use it to identify lightshifts which are currently
 *       pressed.
 */
bool is_tracked_lghts(keypos_t key);
