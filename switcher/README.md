# Switcher

<table>
<tr><td><b>Module</b></td><td><tt>dave-thompson/switcher</tt></td></tr>
<tr><td><b>Version</b></td><td>2025-08-19</td></tr>
<tr><td><b>Maintainer</b></td><td>Dave Thompson (@dave-thompson)</td></tr>
<tr><td><b>License</b></td><td><a href="../LICENSE.txt">GNU GPLv3</a></td></tr>
</table>



## Supercharge your App Switcher & Get More Done

App switchers tend to use awkward combinations like Cmd+Tab or Alt+Tab.  Then they'll put a nice feature on a ridiculous key like ***backtick***, which is unlikely to be handy on a custom QMK board.

With [Switcher](https://github.com/dave-thompson/qmk-modules/switcher) you can cycle through apps with a single key press, remap favourite features to your home row, and a whole lot more.

## Table of Contents

- [Features](#features)

- [Quick Start](#quick-start)

- [Optional Additional Setup](#optional-additional-setup)
  - [1. Secondary Keys](#1-secondary-keys)
  - [2. Switcher Macros](#2-switcher-macros)
  - [3. Other Options](#3-other-options)

- [Troubleshooting](#troubleshooting)
  - [Boot Durations](#boot-durations)
  - [Cache Size](#cache-size)

- [Full Examples](#full-examples)
  - [1. Switcher Key on Nav Layer](#1-switcher-key-on-nav-layer)
  - [2. Exposé Key with Vim Bindings](#2-exposé-key-with-vim-bindings)
  - [3. Same Three Files](#3-same-three-files)

- [Appendix A: Full List of Keycodes, Parameters and Options](#appendix-a-full-list-of-keycodes-parameters-and-options)

- [Appendix B: Resource Requirements](#appendix-b-resource-requirements)

## Features
### Single Key Switching
![](doc/mac_app_switcher.png)
***The MacOS App Switcher:***  *Usually you have to hold* ***Command*** *and repeatedly tap* ***Tab*** *to cycle through apps.  Switcher lets you do this with a single* ***'Switcher'*** *key.*


Tap the **Switcher** key once to open your switcher, then tap it again to cycle through windows or apps.  Switcher supports Windows, Mac or Linux and can simulate any **Hold Key** and **Tap Key** you like.

### Easy App Selection

Switcher opens the highlighted item when you press or release any other key. So if your Switcher key is on a alternative layer and you hold a layer key to access it, the highlighted item will open automatically when you release the layer key.

If instead your Switcher key is on the base layer, then tap any non-Switcher key to open the item.  Switcher can also send this **'ending key'** on to the selected app if you'd like, so there are no wasted keystrokes.

Your other option is to specify an idle timeout, in which case Switcher will open the highlighted item a short while after your last keypress.

### Secondary Keys: Remapped to the Home Row

Many app switchers offer additional features accessed by **'secondary keys'**.  MacOS, for example, allows hiding an app (with 'H'), quitting an app (with 'Q'), or entering App Exposé to view all open windows of an app (with '⬆'). You can give Switcher a list of keys you want to be treated as secondary keys rather than ending keys.

Often, these secondary keys are not conveniently placed on your keyboard, so Switcher lets you remap them to any other keys you like, just while Switcher is active.  How about the home row?

### Secondary Key Caching: Switch Even Faster
Most switching software takes a few hundred milliseconds to appear, and only caches Tab – not other key – presses during this time.  So if you plan to use the arrows or other secondary keys, you have to wait for the switcher to load before doing so.  If you use secondary keys a lot, you inevitably get faster and start losing key presses.

Switcher caches **all** key presses while the software is loading, so you need never lose a key press again.

### Switcher Macros: Do More with a Single Key
Want a single key that sends a whole sequence of switching keystrokes for you? Opening MacOS Exposé normally takes five actions with a wait in the middle: Cmd Down > Tab > _Wait_ > Left > Up > Cmd Up.  With Switcher Macros, your Switcher key can do all this (or anything else) instantly.


### MacOS-Specific Niceties
If you're on a Mac, Switcher resolves many of the shortcomings of the built-in app switcher. With Switcher:

- The app switcher and Exposé use the same navigation keys.
- Opening Exposé automatically highlights the first window.
- Windows open even when minimised.


![](doc/expose.png)
***MacOS Exposé:***  *One of several hidden features of the MacOS App Switcher, accessed via a secondary key press. From here you can select individual app windows using your keyboard.*
***

## Quick Start
Follow the steps below to set up Switcher.

For a real-life example, see my [personal keymap](https://github.com/dave-thompson/qmk-userspace/tree/main/keyboards/zsa/ergodox_ez/m32u4/glow/keymaps/dave-thompson).  For fully coded examples of different use cases, see [Full Examples](#full-examples) below.

### a. keymap.json

Add switcher to your `keymap.json` file.  If you are already using other modules, then in most cases you should add Switcher to the **front** of the list, to prevent your app switcher keystrokes from interfering with other modules.

```json
{
  "modules": ["dave-thompson/switcher", "getreuer/sentence_case"]
}
```

If you don't yet have a `keymap.json`, create one with the below content in the same folder as your `keymap.c` file.

```json
{
    "modules": ["dave-thompson/switcher"]
}
```

### b. keymap.c
Add the SWITCHER keycode (or its alias SWTCH) to your `keymap.c`, wherever you want your Switcher key to be.  For example, let's say part of your keymap looks like this:

```
[0] = LAYOUT_MY_KEYBOARD(
     KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,     KC_H,    KC_J,    KC_K,    KC_L,    KC_ENT,
     KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,     KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,

                                MO(1),   KC_BSPC,  KC_SPC,  MO(2)
  ),
```
If you wanted to turn your forward slash key into a Switcher key, you would simply replace KC_SLSH (on the bottom of the far right column) with SWTCH, like this:

```
[0] = LAYOUT_MY_KEYBOARD(
     KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,     KC_H,    KC_J,    KC_K,    KC_L,    KC_ENT,
     KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,     KC_N,    KC_M,    KC_COMM, KC_DOT,  SWTCH,

                                MO(1),   KC_BSPC,  KC_SPC,  MO(2)
  ),
```

### c. config.h
If you're using the built-in Mac app switcher, add the following line to `config.h`.  This automatically sets up Cmd as your Hold Key and Tab as your Tap Key, and also enables all the MacOS Niceties.

```
#define SWITCHER_MACOS_APP_SWITCHER
```

For any other app switcher, you need to set the Hold and Tap keys manually.  If you're using Alt-Tab on Windows, do that as follows:

```
#define SWITCHER_VIRTUAL_HOLD_KEY KC_LALT
#define SWITCHER_VIRTUAL_TAP_KEY KC_TAB
```

If you would like Switcher to automatically select the highlighted app a short while after your last keypress (without waiting for you to press a non-Switcher key), you can set an Idle Timeout in milliseconds.  For a one second timeout:

```
#define SWITCHER_IDLE_TIMEOUT 1000
```

## Optional Additional Setup

### 1. Secondary Keys
#### a. *config.h*
If you wish to use secondary keys, you need to first define `SWITCHER_ENABLE_SECONDARY_KEYS` in your `config.h`, as follows:

```
#define SWITCHER_ENABLE_SECONDARY_KEYS
```

#### b. *keymap.c*
Then, use the `SWITCHER_SECONDARY_KEYS` macro in your `keymap.c`.  For example, to use the arrow keys plus Q and H (which respectively quit and hide apps on MacOS), you would add the below.  Note that the the first column is the key you want to press and the second column is the key that Switcher will send when you do so.

```
SWITCHER_SECONDARY_KEYS(
    {KC_LEFT, KC_LEFT},     // 'left' functions as usual
    {KC_RIGHT, KC_RIGHT},   // 'right' functions as usual  
    {KC_UP, KC_UP},         // 'up' functions as usual
    {KC_DOWN, KC_DOWN},     // 'down' functions as usual
    {KC_Q, KC_Q},           // 'Q' functions as usual
    {KC_H, KC_H},           // 'H' functions as usual
);

```

Alternatively, if you wanted to use Vim keys for navigation, and remap Quit and Hide to the home row, you could do this instead:

```
SWITCHER_SECONDARY_KEYS(
    {KC_H, KC_LEFT},        // 'H' sends 'left'
    {KC_J, KC_DOWN},        // 'J' sends 'down'
    {KC_K, KC_UP},          // 'K' sends 'up'
    {KC_L, KC_RIGHT},       // 'L' sends 'right'
    {KC_A, KC_Q},           // 'A' sends 'Q' to quit app
    {KC_F, KC_H},           // 'F' sends 'H' to hide app
);

```
You can also use compound keycodes.  On my own Navigation layer, left index finger is 'Paste' (Cmd-V).  While app switching, I remap this to 'Hide':

```

SWITCHER_SECONDARY_KEYS(
    {G(KC_V), KC_H},           // 'Cmd-V' sends H to hide app
                               // (In QMK, G() stands for GUI, another name for 'Cmd'.)
);

```

### 2. Switcher Macros

To send a custom macro immediately after you press the Switcher key, use the `SWITCHER_CUSTOM` keycode (or its alias, `SWTCH_C`) as a key in your keymap. Then in `config.h` specify the sequence of keycodes you want to send when the key is pressed.

We could create a key on MacOS that hides all windows of the last used app (rather than of the currently used app):

```
/**
* KC_H hides the last used app (which is initially highlighted by default)
* KC_DOT exits the switcher with no further changes.
**/
#define SWITCHER_CUSTOM_MACRO {KC_H, KC_DOT}
```

Alternatively, you could have a Mac key that quickly switches to the 2025_main.xls file (or really, to any window of the current app whose name starts with the number '2').

```
/**
* KC_LEFT points to the current app
* KC_UP goes into Exposé
* KC_2 highlights the most recent window whose name begins with the number two
* SWITCHER_OPEN_ITEM is a special keycode that opens the highlighted app / window and closes the switcher
**/
#define SWITCHER_CUSTOM_MACRO {KC_LEFT, KC_UP, KC_2, SWITCHER_OPEN_ITEM}
```

#### 2.1 Preconfigured Exposé Key

> [!NOTE]
> Exposé, aka Application Windows, is not the same as Mission Control.  Mission Control shows you all open windows simultaneously and can only be navigated with the trackpad / mouse.  Exposé shows you the windows of one app at a time and can be navigated with the keyboard.

If you're on MacOS and you'd like a key that opens Exposé but with all the Switcher trimmings, you can add the `SWITCHER_EXPOSE` keycode (or its alias `SWTCH_E`) to your keymap in `keymap.c`.  This opens the app switcher and then sends a pre-configured macro to immediately dive into Exposé.

If you do this, you'll likely also want to set up backtick (`) as a secondary key so that you can cycle through the windows of different apps:

```
SWITCHER_SECONDARY_KEYS(
    {KC_LEFT, KC_LEFT},     // 'left' functions as usual
    {KC_RIGHT, KC_RIGHT},   // 'right' functions as usual  
    {KC_UP, KC_UP},         // 'up' functions as usual
    {KC_DOWN, KC_DOWN},     // 'down' functions as usual
    {KC_GRAVE, KC_GRAVE},   // 'backtick' functions as usual; cycles through apps in exposé
);

```

#### 2.2 Using Multiple Switcher Keys

If you like, you can use all three Primary keycodes (`SWITCHER`, `SWITCHER_EXPOSE`, `SWITCHER_CUSTOM`) simultaneously on three separate keys.  All three must use the same piece of switcher software as you can only specify one `SWITCHER_VIRTUAL_HOLD_KEY` and `SWITCHER_VIRTUAL_TAP_KEY`, but macros allow each to have its own use.

#### 2.3 Even More Custom Switcher Keys
You can go crazy with even more custom Switcher keys with a little custom code in your `keymap.c`:

**Step 1:** Add a custom keycode `MY_SWITCHER_KEYCODE` (or any other keycode):

```
enum custom_keycodes {
  MY_SWITCHER_KEYCODE = SAFE_RANGE,
  // Other custom keys...
};
```

**Step 2:** Assign your new `MY_SWITCHER_KEYCODE` to a key within your keymap.

**Step 3:** Add the below callback to tell Switcher that this is a Switcher keycode:

```
bool is_switcher_keycode_user(uint16_t keycode) {
	return keycode == MY_SWITCHER_KEYCODE;
}
```

**Step 4:** Add another callback to set up the initial sequence for that specific keycode:

```
void switcher_send_macros_user(uint16_t keycode) {
    if (keycode == MY_SWITCHER_KEYCODE) {
        switcher_send_keycode(KC_RIGHT);
        switcher_send_keycode(SWITCHER_OPEN_ITEM);
    }
}
```




### 3. Other Options
#### 3.1. `SWITCHER_PRESELECT_CURRENT_APP`

Most switcher software opens with the previously used app already highlighted.  If you'd like to instead start with the current app highlighted, you can define this option in your `config.h`.  This is useful if you use a single key for both app switching and entering Exposé.

```
#define SWITCHER_PRESELECT_CURRENT_APP
```
#### 3.2. `SWITCHER_FORWARD_ENDING_KEYCODE`

By default, **ending keycodes** – i.e. any keys pressed to select an app in your switching software – are discarded as soon as Switcher exits.  If you want ending keycodes to propogate onwards to the newly opened app instead, then add the following to your `config.h`:

```
#define SWITCHER_FORWARD_ENDING_KEYCODE
```


## Troubleshooting
### Boot Durations
If you experience lost keystrokes or misbehaving initial sequences, you probably need to adjust your boot durations.

Switcher runs exclusively on your keyboard.  There is no way it can know when your switching software has finished loading and is ready to receive keystrokes.  Instead, Switcher takes a guess based on the typical case: any automated initial sequence, as well as any quick additional keystrokes you made after hitting the Switcher key, are released after a delay of exactly 180 milliseconds.

As switching software load times vary widely depending on your hardware and software, you may need to adjust the boot duration in your config.h:

```
#define SWITCHER_BOOT_DURATION 300 // default is 180ms
```

If you're using the MacOS app switcher, there's a separate boot duration for Exposé, which usually takes longer to load.  Adjust it in the same way:

```
#define SWITCHER_EXPOSE_BOOT_DURATION 600 // default is 400ms
```

If your boot duration is too short, keystrokes will be lost, leading to unexpected behaviour.  If your boot duration is too long, keystrokes will always be sent and correctly sequenced, but you may see the app switcher briefly appear in its default state before the keystrokes are applied.  In the event of issues, you should err on the side of a longer boot duration.

### Cache Size
Switcher's key cache is limited to 8 keystrokes.  This is for any initial sequence **plus** any keystrokes you enter manually before the boot duration is complete.  Keys received while the cache is full are silently discarded.  The cache size can be increased in your `config.h` as shown below, but be mindful of memory usage, especially if you have an older keyboard.  Before 2024, even expensive keyboards often came with AVR chips and only 2.5KB of RAM!

```
#define SWITCHER_SECONDARY_KEY_CACHE_SIZE 12 // default is 8
```

## Full Examples

### 1. Switcher Key on Nav Layer
Anatoly uses Windows.  His Switcher key is on a navigation layer that's accessed with a momentary layer switch.

#### a. keymap.json

```
{
  "modules": ["dave-thompson/switcher"]
}
```

#### b. keymap.c
Anatoly's navigation layer looks like this:

```
[2] = LAYOUT_MY_KEYBOARD(
     _______, _______, _______, _______, _______,  _______, _______, KC_UP,   _______,  _______,
     _______, _______, _______, _______, _______,  _______, KC_LEFT, KC_DOWN, KC_RIGHT, SWTCH,
     _______, _______, _______, _______, _______,  _______, _______, _______, _______,  _______,

                                _______, _______,  _______, _______,
  ),
```
To open Switcher, Anatoly holds down his navigation layer key and taps 'SWTCH'.  Since his arrow keys are on the same layer, he'll use those to navigate to the desired window.  As soon as he releases the layer key, the highlighted window will open automatically.

```
SWITCHER_SECONDARY_KEYS(
    {KC_LEFT, KC_LEFT},        // 'left' works as usual
    {KC_RIGHT, KC_RIGHT},      // 'right' works as usual
    {KC_DOWN, KC_DOWN},        // 'down' works as usual
    {KC_UP, KC_UP},            // 'up' works as usual
);
```

#### c. config.h

```
#define SWITCHER_VIRTUAL_HOLD_KEY KC_LALT
#define SWITCHER_VIRTUAL_TAP_KEY KC_TAB
#define SWITCHER_ENABLE_SECONDARY_KEYS

```

### 2. Exposé Key with Vim Bindings
Jenny has a Mac and uses Exposé rather than the app switcher. Her Exposé key immediately shows her open windows of the current app, and she cycles through the windows of other apps by tapping 'F' with her left index finger.

She uses Vim key bindings to navigate between windows and hits the Enter key to switch to the selected window.

#### a. keymap.json

```
{
  "modules": ["dave-thompson/switcher"]
}
```

#### b. keymap.c
Jenny has Exposé on her left thumb:

```
[0] = LAYOUT_MY_KEYBOARD(
     KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,     KC_H,    KC_J,    KC_K,    KC_L,    KC_COLN,
     KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,     KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_BSPC,

                                MO(1),   SWTCH_E,  KC_SPC,  KC_ENT
  ),
```


```
SWITCHER_SECONDARY_KEYS(
    {KC_H, KC_LEFT},        // 'H' sends 'left'
    {KC_J, KC_DOWN},        // 'J' sends 'down'
    {KC_K, KC_UP},          // 'K' sends 'up'
    {KC_L, KC_RIGHT},       // 'L' sends 'right'
    {KC_F, KC_GRAVE},       // 'F' sends 'backtick' to cycle Exposé through different apps
);
```

#### c. config.h

```
#define SWITCHER_MACOS_APP_SWITCHER
#define SWITCHER_ENABLE_SECONDARY_KEYS

```

### 3. Same Three Files
Sumit prepares executive summaries of the latest analytics data.  His fingers are lightning fast and he always works on the same three files: 'Summary', 'Data' and 'Analysis'.  He has three Switcher keys, each of which switches directly to a specific file.

#### a. keymap.json

```
{
  "modules": ["dave-thompson/switcher"]
}
```

#### b. keymap.c

Since Sumit uses three custom Switcher keys, he defines three custom keycodes:

```
enum custom_keycodes {
  S_SUMM = SAFE_RANGE,
  S_DATA,
  S_ANLY,
};
```

He adds those keycodes to his thumb cluster, so he can switch between files as quickly as possible:

```
[0] = LAYOUT_MY_KEYBOARD(
     KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,     KC_H,    KC_J,    KC_K,    KC_L,    KC_COLN,
     KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,     KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,

                      S_SUMM,  S_DATA,  S_ANLY,    KC_TAB,  KC_SPC,  KC_ENT,  
  ),
```

He adds the following two callbacks, so Switcher knows how to process the three different keys:

```
bool is_switcher_keycode_user(uint16_t keycode) {
    return keycode == S_SUMM
        || keycode == S_DATA
        || keycode == S_ANLY;
}

void switcher_send_macros_user(uint16_t keycode) {
    if (keycode == S_SUMM) {
        switcher_send_keycode(KC_LEFT);                // Select current app
        switcher_send_keycode(KC_UP);                  // Enter Exposé
        switcher_send_keycode(KC_S);                   // Select first window whose name begins with 'S'
        switcher_send_keycode(SWITCHER_OPEN_ITEM);     // Open that window 
    }
    if (keycode == S_DATA) {
        switcher_send_keycode(KC_LEFT);                // Select current app
        switcher_send_keycode(KC_UP);                  // Enter Exposé
        switcher_send_keycode(KC_D);                   // Select first window whose name begins with 'D'
        switcher_send_keycode(SWITCHER_OPEN_ITEM);     // Open that window 
    }
    if (keycode == S_ANLY) {
        switcher_send_keycode(KC_LEFT);                // Select current app
        switcher_send_keycode(KC_UP);                  // Enter Exposé
        switcher_send_keycode(KC_A);                   // Select first window whose name begins with 'A'
        switcher_send_keycode(SWITCHER_OPEN_ITEM);     // Open that window 
    }
}
```

#### c. config.h

```
#define SWITCHER_MACOS_APP_SWITCHER

```

## Appendix A: Full List of Keycodes, Parameters and Options

### Keycodes
<table>
<tr><td><b>Primary Keycode</b></td><td><b>Alias</td></b></tr>
<tr><td><tt>SWITCHER</tt></td><td><tt>SWTCH</tt></td></tr>
<tr><td><tt>SWITCHER_CUSTOM</tt></td><td><tt>SWTCH_C</tt></td></tr>
<tr><td><tt>SWITCHER_EXPOSE</tt></td><td><tt>SWTCH_E</tt></td></tr>
</table>

In addition to the primary keycodes above, there is also special secondary keycode `SWITCHER_OPEN_ITEM`. This should never be applied to a key and is only used when specifying custom automated sequences.

### Config Parameters
#### In config.h

See the Setup and Troubleshooting sections above for example usage of each of the following parameters.


<table>
<tr><td><b>Parameter</b></td><td><b>Effect</b></td></tr>
<tr><td><tt>SWITCHER_VIRTUAL_HOLD_KEY</tt></td><td>Defines the Hold key, usually Cmd or Alt, simulated by Switcher.</td></tr>
<tr><td><tt>SWITCHER_VIRTUAL_TAP_KEY</tt></td><td>Defines the Tap key, usually Tab, simulated by Switcher.</td></tr>
<tr><td><tt>SWITCHER_MACOS_APP_SWITCHER</tt></td><td>Turns on all features specific to the built-in MacOS app switcher.  Defaults the virtual Hold and Tap keys to Cmd and Tab; these defaults may be overwritten using the parameters above.</td></tr>
<tr><td><tt>SWITCHER_IDLE_TIMEOUT</tt></td><td>Applies an idle timeout from the time of the user's last keypress.  When the timeout expires, Switcher will automatically select the highlighted item and exit.</td></tr>
<tr><td><tt>SWITCHER_ENABLE_SECONDARY_KEYS</tt></td><td>Enables secondary key functionality.  Must be accompanied by a mapping of secondary keys in keymap.c.</td></tr>
<tr><td><tt> SWITCHER_PRESELECT_CURRENT_APP </tt></td><td>Automatically sends Shift-TapKey when opening the switching software, to highlight the current app rather than the previous one.</td></tr>
<tr><td><tt> SWITCHER_FORWARD_ENDING_KEYCODE </tt></td><td>Forwards any ending keycode to the newly selected app.  (By default, ending keycodes are discarded when Switcher exits.)</td></tr>
<tr><td><tt>SWITCHER_BOOT_DURATION</tt></td><td>Alters the number of milliseconds Switcher waits for your switching software to appear before sending any keycodes (default: 180).</td></tr>
<tr><td><tt>SWITCHER_EXPOSE_BOOT_DURATION</tt></td><td>Alters the number of milliseconds Switcher waits for MacOS Exposé to appear before sending any keycodes (default: 400).</td></tr>
<tr><td><tt>SWITCHER_SECONDARY_KEY_CACHE_SIZE</tt></td><td>Redefines the size of the secondary key cache (default: 8).</td></tr>
<tr><td><tt>SWITCHER_CUSTOM_MACRO</tt></td><td>Defines a list of initial keycodes to be sent automatically when pressing a SWITCHER_CUSTOM key.</td></tr>
</table>

#### In keymap.c

<table>
<tr><td><b>Macro</b></td><td><b>Effect</b></td></tr>
<tr><td><tt>SWITCHER_SECONDARY_KEYS</tt></td><td>Defines any secondary keys, as well as any remappings between keys pressed on the keyboard and virtual secondary keys sent by it.  Must be accompanied by the definition of SWITCHER_ENABLE_SECONDARY_KEYS in config.h.</td></tr>
</table>

### MacOS App Switcher: Known Features for use via Secondary Keys

The MacOS app switcher has several undocumented features available for use via secondary keys:

<table>
<tr><td><b>Key</b></td><td><b>Keycode</b></td><td><b>Effect</b></td></tr>
<tr><td><tt>⬅</tt></td><td><tt>KC_LEFT</tt></td><td>Navigate left</td></tr>
<tr><td><tt>⮕</tt></td><td><tt>KC_RIGHT</tt></td><td>Navigate right</td></tr>
<tr><td><tt>⬆</tt></td><td><tt>KC_UP</tt></td><td>Enter Exposé / Exposé: Navigate up</td></tr>
<tr><td><tt>⬇</tt></td><td><tt>KC_DOWN</tt></td><td>Enter Exposé / Exposé: Navigate down</td></tr>
<tr><td><tt>Q</tt></td><td><tt>KC_Q</tt></td><td>Quit highlighted app</td></tr>
<tr><td><tt>H</tt></td><td><tt>KC_H</tt></td><td>Hide highlighted app</td></tr>
<tr><td><tt>`</tt></td><td><tt>KC_GRAVE</tt></td><td>Exposé: View windows of next app</td></tr>
<tr><td><tt>A-Z, 0-9</tt></td><td><tt>KC_A, KC_0, etc.</tt></td><td>Exposé: Highlight window with matching name</td></tr>
<tr><td><tt>. / ESC</tt></td><td><tt>KC_DOT / KC_ESC</tt></td><td>Close app switcher without switching app</td></tr>
</table>

## Appendix B: Resource Requirements

### Firmware Size

Switcher adds 300 to 900 bytes to your firmware size, dependent on its configuration.

### RAM Usage

Switcher uses ~20 bytes of RAM when minimally configured, plus an additional ~25 bytes for a typical secondary key setup.

