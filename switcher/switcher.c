#include "switcher.h"

ASSERT_COMMUNITY_MODULES_MIN_API_VERSION(1, 0, 0);

#define SWITCHER_WINDOWS_OPEN_DELAY 500

static bool switcher_active = false; // Is the switcher in use?
static bool window_mode = false; // Are we in window switcher mode or app switcher mode?

// Defined in introspection.c
uint16_t switcher_secondary_keys_count(void);
const switcher_key_t* switcher_secondary_keys_get(uint16_t index);

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
                // Highlight the current app rather than the previous app
                //wait_ms(115);
                //tap_code16(S(KC_TAB));
                //tap_code16_delay(S(KC_TAB), 115);
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
                    // if entering window browsing (expose): preselect the first window in the list
                    if (!window_mode &&
                        ((virtual_keycode == KC_UP) || (virtual_keycode == KC_DOWN) || (virtual_keycode == KC_1))) {
                        wait_ms(500); // Waiting for the window switcher to be shown
                        tap_code(KC_RIGHT);
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
