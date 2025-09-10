/**
 * Switcher - QMK Community Module - Single-Key App Switching
 * 
 * See README.md in this folder for a detailed setup guide.
 * 
 * A single 'Switcher' key can be used anytime a modifier needs to be held 
 * while a key is repeatedly tapped. In place of a physical key trigger, 
 * it's also possible to use combos, tap dances, or any other means to 
 * generate the Switcher keycode.
 *                          
 */

#pragma once

#include "quantum.h"

/**
 * @brief Secondary key mapping structure
 * 
 * Maps a physical keycode to a virtual keycode that should be sent while
 * in switcher mode. This allows remapping keys for more convenient access
 * to switcher functions.
 * 
 * Example: {KC_H, KC_LEFT} maps the 'H' key to send 'Left Arrow' while 
 * switching
 */
typedef struct {
    uint16_t keycode;          ///< physical key pressed on keyboard
    uint16_t virtual_keycode;  ///< virtual keycode sent to switcher software
} switcher_key_t;

/** 
 *  @brief Table of secondary keys
 * 
 * Define any secondary keys to be used while in Switcher mode in your 
 * keymap.c, as follows:
 *  
 *  SWITCHER_SECONDARY_KEYS(
 *    {KC_LEFT, KC_LEFT},   // 'left' functions as usual
 *    {KC_RIGHT, KC_RIGHT}, // 'right' functions as usual  
 *    {KC_UP, KC_UP},       // 'up' functions as usual
 *    {KC_DOWN, KC_DOWN},   // 'down' functions as usual
 *    {KC_A, KC_Q},         // left pinky ('A' on QWERTY) sends 'Q' to quit app
 *    {KC_F, KC_H},         // left index ('F' on QWERTY) sends 'H' to hide app
 *  );
 * 
 *  Any keycodes not specified here will exit the switcher and then be sent
 *  for processing.
 * 
 * */
#define SWITCHER_SECONDARY_KEYS(...) \
    const switcher_key_t switcher_secondary_keys[] = { __VA_ARGS__ }

 /**
 * @brief User callback to define additional primary switcher keycodes
 * 
 * Implement this function in your keymap.c to add custom primary keycodes
 * beyond the built-in SWITCHER, SWITCHER_EXPOSE, and SWITCHER_CUSTOM keycodes.
 * 
 * @param keycode Switcher needs to know if this keycode is for a primary key.
 * @return true if supplied keycode is a _primary_ switcher keycode
 * 
 * Example implementation:
 * @code
 * bool is_switcher_keycode_user(uint16_t keycode) {
 *     return keycode == MY_CUSTOM_SWITCHER_KEY;
 * }
 * @endcode
 */
bool is_switcher_keycode_user(uint16_t keycode);

/**
 * @brief User callback to define macro key sequences for custom switcher keys
 * 
 * Called when a switcher key is first pressed, allowing you to send an 
 * initial sequence of keys automatically. This enables complex switcher 
 * behaviors with a single key press.  Macro key sequences are sent after 
 * booting but before user key presses.
 * 
 * @param keycode The primary keycode that activated Switcher
 * 
 * @note Use switcher_send_keycode() to send keys from within this function
 * 
 * Example implementation:
 * @code
 * void switcher_send_macros_user(uint16_t keycode) {
 *     if (keycode == OLDEST_WINDOW) {
 *         switcher_send_keycode(KC_LEFT);             // Select current window
 *         switcher_send_keycode(KC_LEFT);             // Select oldest window
 *                                                     //      (by wraparound)
 *         switcher_send_keycode(SWITCHER_OPEN_ITEM);  // Open window and exit
 *     }
 * }
 * @endcode
 */
void switcher_send_macros_user(uint16_t keycode);

/**
 * @brief Send a virtual keycode to the switcher software
 * 
 * Sends a keycode to the active switcher software, with automatic caching
 * if the switcher is still loading. Use this function within custom initial
 * sequences for programmatic switcher control.
 * 
 * @param virtual_keycode The keycode to send
 * 
 * @note To open a highlighted item, send special keycode SWITCHER_OPEN_ITEM
 * 
 * Example usage:
 * @code
 * switcher_send_keycode(KC_RIGHT);  // Navigate right
 * @endcode
 */
void switcher_send_keycode(uint16_t virtual_keycode);
