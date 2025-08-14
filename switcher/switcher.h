/**
 * QMK Switcher Module - Single-key app switching with advanced features
 * 
 * This module enables single-key app switching by simulating hold+tap sequences
 * (like Cmd+Tab on Mac or Alt+Tab on Windows). Features include:
 * - Single key cycling through applications
 * - Secondary key support for advanced actions (quit, hide, etc.)
 * - Keystroke caching during app switcher boot time
 * - Automatic selection via idle timeout
 * - MacOS-specific optimizations (including Exposé support)
 * 
 * See README.md in this folder for a detailed setup guide.
 *
 * **********
 * 
 * A single 'Switcher' key  can be used anytime a modifier needs to be held while a
 * key is repeatedly tapped. In lieu of a physical key trigger, it's also possible to
 * use combos, tap dances, or any other means to generate the trigger_keycode.
 *
 * Optionally allows for secondary keys, allowing additional actions to be taken
 * without releasing the virtual_hold_key. In the Mac app switcher, this is useful
 * to send a 'Q' while cmd remains held, quitting the highlighted app. This does
 * not require a physical press of the 'Q' key but rather can be triggered by any
 * specified keycode.
 *
 * The virtual_hold_key (cmd in the Mac app switcher case) is released on the press
 * or release of any key other than the specified triggers. This means that if the
 * trigger is on a higher layer and you're holding a layer switch key to access it,
 * the virtual_hold_key will be released as soon as the layer switch key is released.
 */

#pragma once

#include "quantum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Custom switcher secondary keys. The `keycode` field is the keycode as it appears in
 * your layout and determines what is typed normally. The `virtual_keycode` is the
 * keycode that will be sent while in Switcher Mode.
 * 
 */
typedef struct {
    uint16_t keycode;
    uint16_t virtual_keycode;
} switcher_key_t;

/** 
 *  @brief Table of Switcher Secondary Keys
 * 
 *  Define any secondary keys to be used while in Switcher mode in your keymap.c,
 *  as follows:
 *  
 *  SWITCHER_SECONDARY_KEYS(
 *    {KC_LEFT, KC_LEFT},     // 'left' functions as usual
 *    {KC_RIGHT, KC_RIGHT},   // 'right' functions as usual  
 *    {KC_UP, KC_UP},         // 'up' functions as usual
 *    {KC_DOWN, KC_DOWN},     // 'down' functions as usual
 *    {KC_A, KC_Q},           // left pinky ('A' on QWERTY) sends 'Q' to quit app
 *    {KC_F, KC_H},           // left index ('F' on QWERTY) sends 'H' to hide app
 *  );
 * 
 *  Any keycodes not specified here will exit the switcher and then be sent for
 *  processing.
 * 
 * */
#define SWITCHER_SECONDARY_KEYS(...) \
    const switcher_key_t switcher_secondary_keys[] = { __VA_ARGS__ }

bool is_switcher_keycode_user(uint16_t keycode);
void switcher_send_initial_keycodes_user(uint16_t keycode);
void switcher_send_keycode(uint16_t virtual_keycode);

#ifdef __cplusplus
}
#endif