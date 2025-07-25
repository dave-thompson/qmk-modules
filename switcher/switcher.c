#include "switcher.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

static bool switcher_active = false; // Is the switcher in use?
static bool window_mode = false; // Are we in window switcher mode or app switcher mode?

#define SWITCHER_IDLE_TIMEOUT 0 // Max time between keypresses before Switcher closes automatically, selectimg the highlighted item
static uint16_t idle_timer = 0;
static bool switcher_key_held = false;

#define SWITCHER_BOOT_DURATION 200 // Estimated time in ms for the switcher _software_ to boot up after the SWITCHER keystroke is sent; during this time window, any keystrokes will be cached and then sent once boot is expected to have completed
#define SWITCHER_WINDOWS_BOOT_DURATION 10000// 400 // Maximum time in ms for the switcher _software_ to load the window manager; during this time window, any keystrokes will be cached and then sent once loading is expected to have completed
static uint16_t initial_boot_timer = 0; // Estimated switcher _software_ boot completion time; 0 if and only if app switcher not currently booting
static uint16_t window_boot_timer = 0; // Estimated switcher _software_ window mode boot completion time; 0 if and only if window mode not currently booting

// Boot cache
// MacOS only caches Cmd-tab for basic app switching; other keycodes need to be cached at the keyboard level
#define CACHE_SIZE 8
static uint16_t key_cache[CACHE_SIZE];
static uint8_t cache_count = 0;
static keyrecord_t ending_record;
static bool ending_record_cached = false;

// Defined in introspection.c
uint16_t switcher_secondary_keys_count(void);
const switcher_key_t* switcher_secondary_keys_get(uint16_t index);

static void begin_timer(uint16_t *timer, uint16_t duration) {
    if (duration != 0) // 0 disables timed feature (either caching or idle timeout)
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

static void reset_idle_timer(void) {
    begin_timer(&idle_timer, SWITCHER_IDLE_TIMEOUT);
}

static bool loading(void) {
    return initial_boot_timer || window_boot_timer;
}

static void cache_keycode(uint16_t keycode) {
    if (cache_count < CACHE_SIZE) {
        key_cache[cache_count] = keycode;
        cache_count++;
    }
}

static void exit_switcher(void) {
    unregister_code(SWITCHER_VIRTUAL_HOLD_KEY);
    switcher_active = window_mode = ending_record_cached = false;
    cache_count = 0;
}

static void select_highlighted_item(void) {
    if (window_mode) { // if window switcher in use: select the highlighted window
        tap_code(KC_ENTER);
        exit_switcher();
    }
    else { // app switcher in use: hold alt while exiting to instruct macos to open window even if minimised
        register_code(KC_LALT);
        exit_switcher();
        unregister_code(KC_LALT);
    }
}

static void process_secondary_key(uint16_t virtual_keycode) {
    reset_idle_timer();
    if (loading()) {
        cache_keycode(virtual_keycode);
    }
    else {
        tap_code(virtual_keycode);
        // if entering window browsing (expose):
        if (!window_mode &&
            ((virtual_keycode == KC_UP) || (virtual_keycode == KC_DOWN) || (virtual_keycode == KC_1))) {
            begin_timer(&window_boot_timer, SWITCHER_WINDOWS_BOOT_DURATION);
            window_mode = true;
        }
        // if the user selected a window themselves: clean up
        if (window_mode && (virtual_keycode == KC_ENTER)) {
            exit_switcher();
        }
        // if app switcher cancelled: clean up
        if ((virtual_keycode == KC_ESC) || (!window_mode && (virtual_keycode == KC_DOT))) {
            exit_switcher();
        }
    }
}

static void process_ending_key(keyrecord_t *record) {
    if (loading()) { // if loading, cache the record for later processing
        ending_record = *record;
        ending_record_cached = true;
    }
    else {
        select_highlighted_item();
        process_record(record); // Send the ending key for regular processing
    }
}

void process_cached_keys(void) {
    reset_idle_timer(); // in case idle timer expired during the preceding boot delay
    // Prepare to re-cache keys if necessary (in case the processing of a cached key leads to an additional boot delay)
    uint16_t keys_to_process = cache_count;
    cache_count = 0;
    // Process any cached secondary keys
    for (uint8_t i = 0; i < keys_to_process; i++) {
        process_secondary_key(key_cache[i]);
    }
    // Process any cached ending key
    if (ending_record_cached) {
        process_ending_key(&ending_record);
    }
}

bool process_record_switcher(uint16_t current_keycode, keyrecord_t *record) {
    if (current_keycode == SWITCHER) {
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
        // it's not a secondary trigger: end the switching sequence
        process_ending_key(record);
        return false; // swallow the ending key; process_ending_key will explicitly send it on for additional processing if needed
    }
    return true;
}

void housekeeping_task_switcher(void){
    // After App Switcher Boot Up:
    if (timer_just_ended(&initial_boot_timer)) {
        tap_code16(S(KC_TAB)); // Highlight the current app rather than the previous app
        process_cached_keys();
    }

    // After Window Switcher Boot Up:
    if (timer_just_ended(&window_boot_timer)) {
        tap_code(KC_RIGHT); // Select the first window automatically
        process_cached_keys();
    }

    // After Idle for SWITCHER_IDLE_TIMEOUT ms
    if(timer_just_ended(&idle_timer)) {
        // Automatically choose the highlighted item, unless the switcher key is held down, a new screen is booting, or there are cached keys.
        //     (If neither boot timer is running, there are no cached keys.)
        // If this conditional fails, the idle timer is reset when the switcher key is released or a boot timer expires.
        if ((!switcher_key_held) && (!(initial_boot_timer || window_boot_timer))) {
            select_highlighted_item();
        }
    }
}
