#include "switcher.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

static bool switcher_active = false; // Is the switcher in use?
static bool window_mode = false; // Are we in window switcher mode or app switcher mode?

static uint16_t initial_boot_timer = 0; // Estimated switcher _software_ boot completion time; 0 if and only if app switcher not currently booting
static uint16_t window_boot_timer = 0; // Estimated switcher _software_ window mode boot completion time; 0 if and only if window mode not currently booting
#define SWITCHER_BOOT_DURATION 150 // Estimated time in ms for the switcher _software_ to boot up after the SWITCHER keystroke is sent; during this time window, any keystrokes will be cached and then sent once boot is expected to have completed
#define SWITCHER_WINDOWS_BOOT_DURATION 400 // Maximum time in ms for the switcher _software_ to load the window manager; during this time window, any keystrokes will be cached and then sent once loading is expected to have completed

// Defined in introspection.c
uint16_t switcher_secondary_keys_count(void);
const switcher_key_t* switcher_secondary_keys_get(uint16_t index);

static void begin_timer(uint16_t *timer, uint16_t duration) {
    // The bitwise OR here, "| 1", increments any even number by 1 to make it odd. This addresses the 
    // corner case where the computed value is 0 (which otherwise would happen roughly 1 in 65k times:
    // 16-bit timers loop every 2^16 = 65536ms), falsely signalling that we're not currently booting.
    // The resulting 1ms inaccuracy is unimportant.
    *timer = (timer_read() + duration) | 1;
}

static bool timer_has_ended(uint16_t *timer) {
    // If the specified timer is running (i.e. not 0) and expired
    if (*timer && timer_expired(timer_read(), *timer)) {
        *timer = 0;
        return true;
    }
    return false;
}

static void exit_switcher(void) {
    unregister_code(SWITCHER_VIRTUAL_HOLD_KEY);
    switcher_active = window_mode = false;
}

bool process_record_switcher(uint16_t current_keycode, keyrecord_t *record) {
    if (current_keycode == SWITCHER) {
        if (record->event.pressed) {
            // primary trigger pressed
            if (!switcher_active) { // start of the switching sequence
                // hold the hold key
                register_code(SWITCHER_VIRTUAL_HOLD_KEY);
            }
            // tap the tap_key //
            register_code(SWITCHER_VIRTUAL_TAP_KEY);
        } else {
            unregister_code(SWITCHER_VIRTUAL_TAP_KEY);
            if (!switcher_active) {
                begin_timer(&initial_boot_timer, SWITCHER_BOOT_DURATION);
                switcher_active = true;
            }
        }
        return false;
    } else if (switcher_active) { // switcher active; some key (other than primary trigger) pressed / released
        for (int i = 0; i < switcher_secondary_keys_count(); ++i) {
            // if it's a secondary trigger: send the corresponding secondary tap
            const switcher_key_t* switcher_secondary_key = switcher_secondary_keys_get(i);
            if (current_keycode == switcher_secondary_key->keycode) {
                uint16_t virtual_keycode = switcher_secondary_key->virtual_keycode;
                if (record->event.pressed) {
                    register_code(virtual_keycode);
                } else {
                    unregister_code(virtual_keycode);
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
                return false; // swallow the secondary trigger, we don't want it being typed
            }
        }
        // it's not a secondary trigger: end the switching sequence
        if (window_mode) { // if window switcher in use: select the highlighted window
            tap_code(KC_ENTER);
            exit_switcher();
        }
        else { // app switcher in use: hold alt while exiting to instruct macos to open window even if minimised
            register_code(KC_LALT);
            exit_switcher();
            unregister_code(KC_LALT);
        }
        return true; // ensure the terminating keycode _is_ typed
    }
    return true;
}

void housekeeping_task_switcher(void){
    // After App Switcher Boot Up:
    if (timer_has_ended(&initial_boot_timer)) {
        tap_code16(S(KC_TAB)); // Highlight the current app rather than the previous app
    }

    // After Window Switcher Boot Up:
    if (timer_has_ended(&window_boot_timer)) {
        tap_code(KC_RIGHT); // Select the first window automatically
    }
}
