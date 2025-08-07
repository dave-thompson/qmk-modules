# Switcher

<table>
<tr><td><b>Module</b></td><td><tt>dave-thompson/switcher</tt></td></tr>
<tr><td><b>Version</b></td><td>2025-08-04</td></tr>
<tr><td><b>Maintainer</b></td><td>Dave Thompson (@dave-thompson)</td></tr>
<tr><td><b>License</b></td><td><a href="../LICENSE.txt">GNU GPLv3</a></td></tr>
</table>

[Switcher](https://github.com/dave-thompson/qmk-modules/switcher) makes any app switcher work well on a QMK keyboard.

## Features

### Single Key App Switching

![](doc/mac_app_switcher.png)
***The MacOS App Switcher:***  *Usually you have to hold* ***Command*** *and repeatedly tap* ***Tab*** *to cycle through apps.  Switcher lets you do this with a single key.*


Just tap the special Switcher key to open the app switcher, then tap it again to cycle through the apps.  Switcher will simulate any Hold key and any Tap key you like.

### Selecting an App

By default, Switcher selects an app and exits as soon as you press or release any other key. This is ideal if your Switcher key is on a navigation layer, because Switcher will automatically exit as soon as you leave the layer.  If your Switcher key is already on the base layer, then just tap any other key.  Switcher can also send this exit key to the selected app if you'd like, so there are no wasted keystrokes.

Alternatively, you can specify an idle timeout.  In this case, Switcher will automatically select the highlighted app a short while after your last keypress.

### Secondary Key Presses
TODO: Replace with Expose graphic
TODO: Make all graphics jpg?
![](doc/mac_app_switcher.png)
***MacOS Exposé:***  *One of many hidden features of the MacOS App Switcher, accessed via a secondary key press, and from where you can use your keyboard to select individual app windows.*

Many task managers offer more than just switching between apps.  MacOS, for example, allows hiding an app (with 'H'), quitting an app (with 'Q'), or entering Exposé and viewing all open windows of an app (with '⬆'). You can give Switcher a list of any secondary keys you want to allow through to the task manager.

Often, these secondary keys are not conveniently placed on your keyboard, so Switcher also allows you to remap them to any other keys you like.

### Automated Initial Sequences
Sometimes you want a key that will not only open your task manager but do something more — perhaps open Exposé for the current app, which would normally take five actions: Cmd Down - Tab - Left - Up - Cmd Up.  With Switcher you can have a key that will do this (or any other task maanger action) all by itself.

### Caching
Most app switchers cache presses of the tap key (e.g. Tab) while they're booting, but do **not** cache presses of secondary keys.  If you're a quick typist, your choices are to slow down, to suffer lost keypresses, or probably both.

Switcher caches all your keypresses while the switching software is loading, so you can hammer away at the keyboard to your heart's content.

### MacOS Niceties
Switcher resolves some of the shortcomings of the MacOS's built-in app switcher. For example:

- App switcher and Exposé use same navigation keys.
- Opening Exposé automatically highlights first window.
- Windows open even when minimised.

## Basic Setup
Follow the steps below to set up Switcher.  (For a complete example, see my [personal keymap](https://github.com/dave-thompson/qmk-userspace).)

###1. keymap.json

Add switcher to your `keymap.json` file.  If you are already using other modules, then in most cases you should add Switcher to the **front** of the list, to prevent your app switcher keystrokes interfering with modules such as Sentence Case.

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

### 2. keymap.c
Add the SWITCHER keycode (or its alias SWTCH) to your `keymap.c`, in the desired location of your Switcher key.


### 2. config.h
If you're using the built-in Mac app switcher, add the following line to `config.h`.  This automatically sets up Cmd as your Hold Key and Tab as your Tap Key, and also enables all the MacOS Niceties.

```
#define SWITCHER_MACOS_APP_SWITCHER
```

For any other app switcher, you need to set the Hold and Tap keys manually.  If you're using Alt-Tab on Windows, do that as follows:

```
#define SWITCHER_VIRTUAL_HOLD_KEY KC_LALT
#define SWITCHER_VIRTUAL_TAP_KEY KC_TAB
```

If you would like Switcher to automatically select the highlighted app a short while after your last keypress (without waiting for you to press a non-Switcher key), then set an Idle Timeout in milliseconds.  For a one second timeout:

```
#define SWITCHER_IDLE_TIMEOUT 1000
```

## Additional Setup

###Secondary Key Presses
SWITCHER_ENABLE_SECONDARY_KEYS (default: disabled)

Example: home row for hide, quitting
Example: Vim keys for navigation



### Automated Initial Sequences (including Custom)
SWITCHER_EXPOSE keycode (alias: SWTCH_E) - if using, you may also want to set up ` as a secondary key to show the windows of different apps.

Custom initial sequences with SWITCHER_CUSTOM keycode (alias SWTCH_C)

SWITCHER_SELECT_ITEM keycode



### Advanced Use: Multiple Custom Switcher Keys
1) Define MY_SWITCHER_KEYCODE in your custom keycodes enum.

2) Assign MY_SWITCHER_KEYCODE to a key within your keymap.

3)

```
bool is_switcher_keycode_user(uint16_t keycode) {
	return (keycode == MY_SWITCHER_KEYCODE);
}
```

4)

```
void switcher_send_initial_keycodes_user(uint16_t keycode) {
    if (keycode == MY_SWITCHER_KEYCODE) {
        switcher_send_keycode(KC_RIGHT);
        switcher_send_keycode(SWITCHER_SELECT_ITEM);
    }
}
```

### Other Options
+ SWITCHER_SELECT_CURRENT_APP_NOT_PREVIOUS (default: selects previously used app)
+ SWITCHER_SWALLOW_ENDING_KEYCODE (default: sends ending keycode)


### MacOS App Switcher: List of All Possible Secondary Keys

##Troubleshooting
### General
If 
###Caching - Boot Durations
+ SWITCHER_BOOT_DURATION (optional, default: 180)
+ SWITCHER_EXPOSE_BOOT_DURATION (optional, default: 400)
Cache limited to 8 keystrokes; any keystrokes received while cache is full will be discarded; cache size can be increased using SWITCHER_SECONDARY_KEY_CACHE_SIZE, but be mindful of memory usage


##Appendix: Full Keycode & Config Parameter List