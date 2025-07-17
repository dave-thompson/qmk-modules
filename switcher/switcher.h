/**
 * Allows for Mac app switching (holding down cmd, then tab-ing multiple times)
 * from a single physical key.
 *
 * More generally, can be used anytime a modifier needs to be held while a key is
 * repeatedly tapped. In lieu of a physical key trigger, it's also possible to use
 * combos, tap dances, or any other means to generate the trigger_keycode.
 *
 * Optionally allows for a secondary trigger, facilitating an alternative action
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
 * @brief Switcher Virtual Hold Key
 *
 * Define `SWITCHER_VIRTUAL_HOLD_KEY` and assign it to the keycode you would like held
 * down whenever Switcher is open.  For example, to use with the MacOS app switcher,
 * set it to the Command key as follows:
 *
 *     uint16_t SWITCHER_VIRTUAL_HOLD_KEY = KC_LGUI;
 */

extern uint16_t SWITCHER_VIRTUAL_HOLD_KEY;

/**
 * @brief Switcher Virtual Tap Key
 *
 * Define `SWITCHER_VIRTUAL_TAP_KEY` and assign it to the keycode you would like sent
 * every time the SWITCHER key is tapped.  For example, to use with the MacOS app switcher,
 * set it to the Tab key as follows:
 *
 *     uint16_t SWITCHER_VIRTUAL_TAP_KEY = KC_TAB;
 */

extern uint16_t SWITCHER_VIRTUAL_TAP_KEY;

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

/** Table of switcher secondary keys.
 * 
 *  Define any secondary keys to be used while in Switcher mode in your keymap.c,
 *  as follows:
 *  
 *  const switcher_key_t switcher_secondary_keys[] = {
 *    // When switcher is active:
 *    {KC_LEFT, KC_LEFT}, // 'left' functions as usual
 *    {KC_RIGHT, KC_RIGHT}, // 'right' functions as usual
 *    {KC_UP, KC_UP}, // 'up' functions as usual
 *    {KC_DOWN, KC_DOWN}, // 'down' functions as usual
 *    {KC_A, KC_Q}, // left pinky ('A' on qwerty) sends 'Q' to quit app
 *    {KC_F, KC_H}, // left index ('F' on qwerty) sends 'H' to hide app
 *    {KC_D, KC_DOT}, // left middle ('D' on qwerty) sends '.' to silently exit switcher
 *  };
 * 
 *  Any keycodes not specified here will exit the switcher and then be sent for
 *  processing.
 * 
 * */

extern const switcher_key_t switcher_secondary_keys[];

#ifdef __cplusplus
}
#endif