#include "switcher.h"
#include "switcher_config.h"
#include "switcher_timers.h"
#include "switcher_cache.h"


///////////////////////////////////////////////////////////////////////////////
//
// State
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
    bool active;                     // is the switcher in use?
    bool expose_mode;                // are we in exposé mode or app switcher mode?
    bool primary_key_held;           // is a Primary switcher key currently held down?
    uint16_t last_primary_keycode;   // which was the last Primary switcher key to be held down?
} switcher_state_t;

static switcher_state_t state = {0};

// Resets state to zero (i.e. inactive)
static void reset_state(void) {
    state = (switcher_state_t){0};
}


///////////////////////////////////////////////////////////////////////////////
//
// Exiting Switcher
//
///////////////////////////////////////////////////////////////////////////////

static void exit_switcher(void) {
    unregister_code(SWITCHER_VIRTUAL_HOLD_KEY);
    reset_state();
    switcher_reset_cache();
}

static void select_highlighted_item(void) {
    if (using_macos_switcher()) {
        if (state.expose_mode) { // exposé: select the highlighted window
            tap_code(KC_ENTER);
            exit_switcher();
        }
        else { // app switcher: hold alt to open window even if minimised
            register_code(KC_LALT);
            exit_switcher();
            unregister_code(KC_LALT);
        }
    }
    else exit_switcher();
}


///////////////////////////////////////////////////////////////////////////////
//
// Ending Keys
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SWITCHER_FORWARD_ENDING_KEYCODE
    // Is this keycode a layer switch key?
    static bool is_layer_switch(uint16_t keycode) {
        switch (keycode) {
        case QK_MOMENTARY ... QK_MOMENTARY_MAX:
        case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX:
        case QK_TO ... QK_TO_MAX:
        case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
      #ifndef NO_ACTION_ONESHOT
        case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX:
      #endif //NO_ACTION_ONESHOT
      #ifdef LAYER_LOCK_ENABLE
        case QK_LLCK:
      #endif //LAYER_LOCK_ENABLE
            return true;
        default:
            return false;
        }
    }
#endif

// Handles Ending Keys
static void process_ending_key(uint16_t keycode, keyrecord_t *record) {
    if (switcher_loading()) { // if loading, cache the record for later processing
        switcher_cache_ending_key(keycode, record);
    }
    else {
        select_highlighted_item();
        #ifdef SWITCHER_FORWARD_ENDING_KEYCODE
            process_record(record); // forward ending key for regular processing
        #else // discard ending key (unless it's a layer switch)
            if (is_layer_switch(keycode)) {process_record(record);}
        #endif
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Secondary Keys
//
///////////////////////////////////////////////////////////////////////////////

#ifdef SWITCHER_ENABLE_SECONDARY_KEYS
    // Defined in introspection.c
    uint16_t switcher_secondary_keys_count(void);
    const switcher_key_t* switcher_secondary_keys_get(uint16_t index);
#endif

#ifdef SWITCHER_MACOS_APP_SWITCHER
    static void transition_to_expose(void) {
        switcher_start_expose_boot_timer();
        state.expose_mode = true;
    }
#else
    static void transition_to_expose(void) { }
#endif

// Returns the corresponding virtual keycode for a given secondary key,
// or KC_NO if the supplied keycode is not a secondary key
static uint16_t virtual_keycode_for_secondary_key(uint16_t keycode) {
    #ifdef SWITCHER_ENABLE_SECONDARY_KEYS
        for (int i = 0; i < switcher_secondary_keys_count(); ++i) {
            const switcher_key_t* switcher_secondary_key = switcher_secondary_keys_get(i);
            if (keycode == switcher_secondary_key->keycode) {
                return switcher_secondary_key->virtual_keycode;
            }
        }
    #endif
    return KC_NO;
}

// Sends a virtual secondary keycode
static void send_virtual_secondary_keycode_immediately(uint16_t virtual_keycode) {
    tap_code16(virtual_keycode);
    // if app switcher cancelled: clean up (presumes all switching software uses Escape to quit)
    if (virtual_keycode == KC_ESC) {
        exit_switcher();
    }
    if (virtual_keycode == SWITCHER_OPEN_ITEM) {
        select_highlighted_item();
    }
    if (using_macos_switcher()) {
        // if entering window browsing (exposé):
        if (!state.expose_mode &&
            ((virtual_keycode == KC_UP) || (virtual_keycode == KC_DOWN) || (virtual_keycode == KC_1))) {
            transition_to_expose();
        }
        // if the user selected a window themselves: clean up
        if ((state.expose_mode && (virtual_keycode == KC_ENTER))||(!state.expose_mode && (virtual_keycode == KC_SPACE))) {
            exit_switcher();
        }
        // if app switcher cancelled: clean up
        if (!state.expose_mode && (virtual_keycode == KC_DOT)) {
            exit_switcher();
        }
    }
}

// Sends (or caches) a virtual secondary keycode
static void process_virtual_secondary_key(uint16_t virtual_keycode) {
    switcher_restart_idle_timer();
    if (switcher_loading()) {
        switcher_cache_secondary_keycode(virtual_keycode);
    }
    else {
        send_virtual_secondary_keycode_immediately(virtual_keycode);
    }
}

// User callable wrapper for process_virtual_secondary_key.
// Used only when defining custom automated sequences.
void switcher_send_keycode(uint16_t virtual_keycode) {
    process_virtual_secondary_key(virtual_keycode);
}


///////////////////////////////////////////////////////////////////////////////
//
// Primary Keys
//
///////////////////////////////////////////////////////////////////////////////

// Is this keycode a Primary key?
static bool is_primary_keycode(uint16_t keycode) {
    if (keycode == SWITCHER ||
        keycode == SWITCHER_EXPOSE ||
        keycode == SWITCHER_CUSTOM ||
        is_switcher_keycode_user(keycode)) {
        state.last_primary_keycode = keycode;
        return true;
    }
    else
        return false;
}

// Callback to allow user to define additional Primary keycodes
__attribute__((weak)) bool is_switcher_keycode_user(uint16_t keycode) {
    return false;
}

// Handles Primary key presses/releases
static void process_primary_keycode(keyrecord_t *record) {
    if (record->event.pressed) {
        if (!state.active) { // start of the switching sequence
            // hold the hold key
            register_code(SWITCHER_VIRTUAL_HOLD_KEY);
            switcher_start_initial_boot_timer();
            state.active = true;
        }
        // tap the tap_key
        register_code(SWITCHER_VIRTUAL_TAP_KEY);
        state.primary_key_held = true;
    } else {
        unregister_code(SWITCHER_VIRTUAL_TAP_KEY);
        state.primary_key_held = false;
        switcher_restart_idle_timer();
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Triaging Key Presses
//
// - Triages keys into four categories:
//
//   A) While Switcher is inactive:
//       0) Out of Scope Keys
//       1) Primary Keys (keys that activate Switcher, e.g. SWTCH)
//
//   B) While Switcher is active:
//       1) Primary Keys
//       2) Secondary Keys
//       3) Ending Keys
// 
///////////////////////////////////////////////////////////////////////////////

// Handles keys known to be either Secondary or Ending keys
// (i.e. all keys received while Switcher is active, except for Primary keys)
static void process_secondary_or_ending_key (uint16_t keycode, keyrecord_t *record) {
    uint16_t virtual_keycode = virtual_keycode_for_secondary_key(keycode);
    if (virtual_keycode != KC_NO) { // Secondary key
        if (record->event.pressed) {
            process_virtual_secondary_key(virtual_keycode);
        }
    }
    else { // Ending key
        process_ending_key(keycode, record);
    }
}

// Handles all key presses/releases
bool process_record_switcher(uint16_t current_keycode, keyrecord_t *record) {
    if (is_primary_keycode(current_keycode)) { // Primary key
        process_primary_keycode(record);
        return false;
    } else if (state.active) { // Secondary or Ending Key
        process_secondary_or_ending_key(current_keycode, record);
        return false;
    }
    return true; // Out of Scope key - ignore and pass on
}


///////////////////////////////////////////////////////////////////////////////
//
// Initial Keycodes
//     - including Switcher Macros
//
///////////////////////////////////////////////////////////////////////////////

// Sends any initial keycodes required to setup the switching software
// before it appears
static void switcher_send_initial_keycodes(uint16_t switcher_keycode) {

    // Enter Exposé
    if (using_macos_switcher()) {
         if (switcher_keycode == SWITCHER_EXPOSE) {
            process_virtual_secondary_key(KC_LEFT);
            process_virtual_secondary_key(KC_UP);
        }
    }

    // Highlight current app rather than previous app
    #ifdef SWITCHER_PRESELECT_CURRENT_APP
        if (switcher_keycode == SWITCHER) {
            // highlight current app instead of previous app
            process_virtual_secondary_key(S(SWITCHER_VIRTUAL_TAP_KEY));
        }
    #endif

    // Send Custom Macro
    #ifdef SWITCHER_CUSTOM_MACRO
        if (switcher_keycode == SWITCHER_CUSTOM) {
            // send custom macro
            static const uint16_t custom_macro_sequence[] = SWITCHER_CUSTOM_MACRO;
            for (uint8_t i = 0; i < ARRAY_SIZE(custom_macro_sequence); i++) {
                process_virtual_secondary_key(custom_macro_sequence[i]);
            }
        }
    #endif

    // Send additional, self-coded custom macros
    switcher_send_macros_user(switcher_keycode);
}

// User callback to define self-coded custom macros
__attribute__((weak)) void switcher_send_macros_user(uint16_t keycode) {}

#ifdef SWITCHER_MACOS_APP_SWITCHER
    // On entering Exposé: select the first window automatically
    static void switcher_send_expose_macro(uint16_t switcher_keycode) {
        process_virtual_secondary_key(KC_RIGHT);
    }
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Post-Boot Processing
//
///////////////////////////////////////////////////////////////////////////////

// Processes all keys in the cache
static void process_cached_keys(void) {
    switcher_restart_idle_timer(); // in case idle timer expired during the preceding boot delay

    // process any cached secondary keys
    if (switcher_secondary_keys_enabled()) {
        // prepare to re-cache keys if necessary (in case the processing of a cached key leads to an additional boot delay)
        uint8_t keys_to_process = switcher_secondary_cache_count();
        switcher_reset_secondary_cache_index(); // so that any processed keys that are re-cached are added to the _start_ of the cache
        for (uint8_t i = 0; i < keys_to_process; i++) {
            process_virtual_secondary_key(switcher_secondary_cache_get_item_at_index(i));
        }
    }

    // process any cached ending key
    if (switcher_is_ending_key_cached()) {
        process_ending_key(switcher_cached_ending_keycode(), switcher_cached_ending_record());
    }

}

// Checks if any boot timers have ended
// Sends initial keycodes and processes cached keys if they have
static void handle_any_boot_completion(void) {
    
    // after App Switcher boots:
    if (switcher_initial_boot_timer_ended()) {
        switcher_send_initial_keycodes(state.last_primary_keycode);
        process_cached_keys();
    }

    // after Exposé Boots:
    #ifdef SWITCHER_MACOS_APP_SWITCHER
        if (switcher_expose_boot_timer_ended()) {
            switcher_send_expose_macro(state.last_primary_keycode);
            process_cached_keys();
        }
    #endif

}


///////////////////////////////////////////////////////////////////////////////
//
// Idle Timeouts
//
///////////////////////////////////////////////////////////////////////////////

static void handle_any_idle_timeout(void) {
    if (switcher_idle_timed_out()) {
        // Ignore the timeout if a Primary key is held down, or if a new screen is booting (meaning there may be cached keys to deal with).
        // The idle timer will restart either when the Primary key is released or when the boot timer expires.
        if (!state.primary_key_held && !switcher_loading()) {
            select_highlighted_item();
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Timed Events
//
///////////////////////////////////////////////////////////////////////////////

void housekeeping_task_switcher(void) {
    handle_any_boot_completion(); // boot completion (and cached keys) must be dealt with _before_ any idle timeout
    handle_any_idle_timeout();
}
