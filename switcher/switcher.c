#include "switcher.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

/* BASIC STATUS */
static bool switcher_key_held = false;
static bool switcher_active = false; // Is the switcher in use?
static uint16_t latest_switcher_keycode;
#ifdef SWITCHER_MACOS_APP_SWITCHER
    static bool window_mode = false; // Are we in window switcher mode or app switcher mode?
#endif

/* IDLE TIMER */
#ifdef SWITCHER_IDLE_TIMEOUT // Max time between keypresses before Switcher closes automatically, selectimg the highlighted item
    static void begin_timer(uint16_t *timer, uint16_t duration);
    static uint16_t idle_timer = 0;
    static inline void reset_idle_timer(void) { begin_timer(&idle_timer, SWITCHER_IDLE_TIMEOUT); }
#else
    static inline void reset_idle_timer(void) { }
#endif

/* SECONDARY KEYS */
#ifdef SWITCHER_ENABLE_SECONDARY_KEYS
    // Defined in introspection.c
    uint16_t switcher_secondary_keys_count(void);
    switcher_key_t* switcher_secondary_keys_get(uint16_t index);
#endif

/* BOOT CACHE */
// (MacOS only caches Cmd-tab for basic app switching; secondary keycodes need to be cached at the keyboard level.)
#ifdef SWITCHER_ENABLE_SECONDARY_KEYS
    #ifndef SWITCHER_SECONDARY_KEY_CACHE_SIZE
        #define SWITCHER_SECONDARY_KEY_CACHE_SIZE 8
    #endif
    static uint16_t secondary_key_cache[SWITCHER_SECONDARY_KEY_CACHE_SIZE];
    static uint8_t secondary_key_cache_count = 0;
#endif

static keyrecord_t ending_record;
static uint16_t ending_keycode;
static bool ending_record_cached = false;

/* BOOT TIMERS */
#ifndef SWITCHER_BOOT_DURATION // Maximum time in ms for the switcher _software_ to boot up after the SWITCHER keystroke is sent; during this time window, any keystrokes will be cached and then sent once boot is expected to have completed
    #define SWITCHER_BOOT_DURATION 180
#endif

#if defined(SWITCHER_MACOS_APP_SWITCHER) && !defined(SWITCHER_EXPOSE_BOOT_DURATION) // Maximum time in ms for the stock MacOS App Switcher to load the window manager; during this time window, any keystrokes will be cached and then sent once loading is expected to have completed
    #define SWITCHER_EXPOSE_BOOT_DURATION 400
#endif

// Constrain boot durations:
//   - They must be at least 1 millisecond, so that boot timers always run and post-boot processing is always carried out.
//   - They must not exceed 30 seconds, so as not to overflow the 16-bit timer (32,768ms).
#if SWITCHER_BOOT_DURATION < 1 || SWITCHER_BOOT_DURATION > 30000
    #error "switcher: SWITCHER_BOOT_DURATION must be between 1 and 30000 ms"
#endif
#if defined(SWITCHER_EXPOSE_BOOT_DURATION) && (SWITCHER_EXPOSE_BOOT_DURATION < 1 || SWITCHER_EXPOSE_BOOT_DURATION > 30000)
    #error "switcher: SWITCHER_EXPOSE_BOOT_DURATION must be between 1 and 30000 ms"
#endif

static uint16_t initial_boot_timer = 0; // Estimated switcher _software_ boot completion time; 0 if and only if app switcher not currently booting
#ifdef SWITCHER_MACOS_APP_SWITCHER
    static uint16_t expose_boot_timer = 0; // Estimated MacOS Expose boot completion time; 0 if and only if window mode not currently booting
    static inline bool loading(void) { return initial_boot_timer || expose_boot_timer; }
#else
    static inline bool loading(void) { return initial_boot_timer; }
#endif

static void begin_timer(uint16_t *timer, uint16_t duration) {
    if (duration != 0) // 0 disables timed feature (only for idle timeout; caching never has a duration of < 1)
    {
        // The bitwise OR here, "| 1", increments any even number by 1 to make it odd. This addresses the 
        // corner case where the computed value is 0 (which otherwise would happen roughly 1 in 65k times:
        // 16-bit timers loop every 2^16 = 65536ms), falsely signalling that we're not currently booting.
        // The resulting 1ms inaccuracy is unimportant.
        *timer = (timer_read() + duration) | 1;
    }
}

static bool timer_just_ended(uint16_t *timer) {
    // If the specified timer is running (i.e. not 0) and expired
    if (*timer && timer_expired(timer_read(), *timer)) {
        *timer = 0;
        return true;
    }
    return false;
}

#ifdef SWITCHER_SWALLOW_ENDING_KEYCODE
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

static void cache_secondary_keycode(uint16_t keycode) {
    if (secondary_key_cache_count < SWITCHER_SECONDARY_KEY_CACHE_SIZE) {
        secondary_key_cache[secondary_key_cache_count] = keycode;
        secondary_key_cache_count++;
    }
}

static void exit_switcher(void) {
    unregister_code(SWITCHER_VIRTUAL_HOLD_KEY);
    switcher_active = ending_record_cached = false;
    #ifdef SWITCHER_MACOS_APP_SWITCHER
        window_mode = false;
    #endif
    #ifdef SWITCHER_ENABLE_SECONDARY_KEYS
        secondary_key_cache_count = 0;
    #endif
}

static void select_highlighted_item(void) {
    #ifdef SWITCHER_MACOS_APP_SWITCHER
        if (window_mode) { // if window switcher in use: select the highlighted window
            tap_code(KC_ENTER);
            exit_switcher();
        }
        else { // app switcher in use: hold alt while exiting to instruct macos to open window even if minimised
            register_code(KC_LALT);
            exit_switcher();
            unregister_code(KC_LALT);
        }
    #else
        exit_switcher();
    #endif
}

static void process_secondary_key(uint16_t virtual_keycode) {
    reset_idle_timer();
    if (loading()) {
        cache_secondary_keycode(virtual_keycode);
    }
    else {
        tap_code16(virtual_keycode);
        // if app switcher cancelled: clean up (presumes all switching software uses Escape to quit)
        if (virtual_keycode == KC_ESC) {
            exit_switcher();
        }
        if (virtual_keycode == SWITCHER_SELECT_ITEM) {
            select_highlighted_item();
        }
        #ifdef SWITCHER_MACOS_APP_SWITCHER
            // if entering window browsing (exposé):
            if (!window_mode &&
                ((virtual_keycode == KC_UP) || (virtual_keycode == KC_DOWN) || (virtual_keycode == KC_1))) {
                begin_timer(&expose_boot_timer, SWITCHER_EXPOSE_BOOT_DURATION);
                window_mode = true;
            }
            // if the user selected a window themselves: clean up
            if (window_mode && (virtual_keycode == KC_ENTER)) {
                exit_switcher();
            }
            // if app switcher cancelled: clean up
            if (!window_mode && (virtual_keycode == KC_DOT)) {
                exit_switcher();
            }
        #endif
    }
}

void switcher_send_keycode(uint16_t virtual_keycode) {
    process_secondary_key(virtual_keycode);
}

static bool is_switcher_keycode(uint16_t keycode) {
    if (keycode == SWITCHER ||
        keycode == SWITCHER_EXPOSE ||
        is_switcher_keycode_user(keycode)) {
        latest_switcher_keycode = keycode;
        return true;
    }
    else
        return false;
}

__attribute__((weak)) bool is_switcher_keycode_user(uint16_t keycode) {
    return false;
}

static void switcher_send_initial_keycodes(uint16_t switcher_keycode) {

    #ifdef SWITCHER_MACOS_APP_SWITCHER
        if (switcher_keycode == SWITCHER_EXPOSE) {
            process_secondary_key(KC_LEFT);
            process_secondary_key(KC_UP);
        }
    #endif

    #ifdef SWITCHER_SELECT_CURRENT_APP_NOT_PREVIOUS
        if (switcher_keycode == SWITCHER) {
            process_secondary_key(S(SWITCHER_VIRTUAL_TAP_KEY)); // Highlight the current app rather than the previous app
        }
    #endif

    switcher_send_initial_keycodes_user(switcher_keycode);
}

__attribute__((weak)) void switcher_send_initial_keycodes_user(uint16_t keycode) {}

#ifdef SWITCHER_MACOS_APP_SWITCHER
void switcher_send_initial_expose_keycodes(uint16_t switcher_keycode) {
    process_secondary_key(KC_RIGHT); // Select the first window automatically
}
#endif //SWITCHER_MACOS_APP_SWITCHER

static void process_ending_key(uint16_t keycode, keyrecord_t *record) {
    if (loading()) { // if loading, cache the record for later processing
        ending_record = *record;
        ending_keycode = keycode;
        ending_record_cached = true;
    }
    else {
        select_highlighted_item();
        #ifndef SWITCHER_SWALLOW_ENDING_KEYCODE
            process_record(record); // Send the ending key for regular processing
        #else
            if (is_layer_switch(keycode)) {process_record(record);} // Swallow ending key (unless it's a layer switch)
        #endif
    }
}

static void process_cached_keys(void) {
    reset_idle_timer(); // in case idle timer expired during the preceding boot delay
    #ifdef SWITCHER_ENABLE_SECONDARY_KEYS
        // Prepare to re-cache keys if necessary (in case the processing of a cached key leads to an additional boot delay)
        uint16_t keys_to_process = secondary_key_cache_count;
        secondary_key_cache_count = 0;
        // Process any cached secondary keys
        for (uint8_t i = 0; i < keys_to_process; i++) {
            process_secondary_key(secondary_key_cache[i]);
        }
    #endif
    // Process any cached ending key
    if (ending_record_cached) {
        process_ending_key(ending_keycode, &ending_record);
    }
}

bool process_record_switcher(uint16_t current_keycode, keyrecord_t *record) {
    if (is_switcher_keycode(current_keycode)) {
        if (record->event.pressed) {
            // primary trigger pressed
            if (!switcher_active) { // start of the switching sequence
                // hold the hold key
                register_code(SWITCHER_VIRTUAL_HOLD_KEY);
                begin_timer(&initial_boot_timer, SWITCHER_BOOT_DURATION);
                switcher_active = true;
            }
            // tap the tap_key
            register_code(SWITCHER_VIRTUAL_TAP_KEY);
            switcher_key_held = true;
        } else {
            unregister_code(SWITCHER_VIRTUAL_TAP_KEY);
            switcher_key_held = false;
            reset_idle_timer();
        }
        return false;
    } else if (switcher_active) { // switcher active; some key (other than primary trigger) pressed / released
        #ifdef SWITCHER_ENABLE_SECONDARY_KEYS
            for (int i = 0; i < switcher_secondary_keys_count(); ++i) {
                // if it's a secondary trigger: send the corresponding secondary tap
                const switcher_key_t* switcher_secondary_key = switcher_secondary_keys_get(i);
                if (current_keycode == switcher_secondary_key->keycode) {
                    if (record->event.pressed) {
                        uint16_t virtual_keycode = switcher_secondary_key->virtual_keycode;
                        process_secondary_key(virtual_keycode);
                    }
                    return false; // swallow the secondary trigger; we don't want it being typed
                }
            }
        #endif
        // it's not a secondary trigger: end the switching sequence
        process_ending_key(current_keycode, record);
        return false; // swallow the ending key; process_ending_key will explicitly send it on for additional processing if needed
    }
    return true;
}

void housekeeping_task_switcher(void){
    // After App Switcher Boot Up:
    if (timer_just_ended(&initial_boot_timer)) {
        switcher_send_initial_keycodes(latest_switcher_keycode);
        process_cached_keys();
    }

    // After Window Switcher (Exposé) Boot Up:
    #ifdef SWITCHER_MACOS_APP_SWITCHER
        if (timer_just_ended(&expose_boot_timer)) {
            switcher_send_initial_expose_keycodes(latest_switcher_keycode);
            process_cached_keys();
        }
    #endif

    // After Idle Timer Expires:
    #ifdef SWITCHER_IDLE_TIMEOUT
        if(timer_just_ended(&idle_timer)) {
            // Automatically choose the highlighted item, unless the switcher key is held down, a new screen is booting, or there are cached keys.
            //     (If neither boot timer is running [=== if !loading()], then there are no cached keys.)
            // If this conditional fails, the idle timer is reset when the switcher key is released or a boot timer expires.
            if (!switcher_key_held && !loading()) {
                select_highlighted_item();
            }
        }
    #endif
}
