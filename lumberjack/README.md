# Lumberjack

<table>
<tr><td><b>Module</b></td><td><tt>dave-thompson/lumberjack</tt></td></tr>
<tr><td><b>Version</b></td><td>2025-09-01</td></tr>
<tr><td><b>Maintainer</b></td><td>Dave Thompson (@dave-thompson)</td></tr>
<tr><td><b>License</b></td><td><a href="../LICENSE.txt">GNU GPLv3</a></td></tr>
</table>

## Pretty Key Log for Easier Debugging

Sometimes, your keyboard doesn't do what you expect, and figuring out **why on earth not** can be tricky.

[Lumberjack](https://github.com/dave-thompson/qmk-modules/lumberjack) logs every key press to the console, with deltas and hold times, so you can see what's going on.

## Overview

![](doc/lumberjack.png)
***The QMK Console:***  *Lumberjack records every key press to the console so you can see exactly which keys you pressed and when*

Every event is shown with four pieces of information:

- **Keycode:** The code for the pressed key.
- **Down / Up:** Whether the key was pressed DOWN or released UP.
- **Delta:** The amount of time since the previous logged event.
- **Hold:** The amount of time for which the key was held down.

## Quick Start
Follow the steps below to set up Lumberjack.

### a. keymap.json

Add Lumberjack to your `keymap.json` file.  If you are already using other modules, then in most cases you should add Lumberjack to the **front** of the list, so it can log your key presses before other modules intercept (and potentially swallow) them.  Lumberjack will never interfere with modules that come after it.

```json
{
  "modules": ["dave-thompson/lumberjack", "dave-thompson/switcher"]
}
```

If you don't yet have a `keymap.json`, create one with the below content in the same folder as your `keymap.c` file.

```json
{
    "modules": ["dave-thompson/lumberjack"]
}
```

### b. Install a QMK Console

Lumberjack logs your keystrokes to a QMK console.  You have two console options:

- **Command Line QMK:** Firstly, if you don't already have QMK installed on your machine, then follow steps 2 & 3 from the [QMK Setup Guide](https://docs.qmk.fm/newbs_getting_started) to install it.  Once QMK is installed, open a terminal window and type `qmk console` to open a QMK console in that window.

- **QMK Toolbox:**  If you don't want to install QMK or you'd rather avoid using the command line, you can instead install and use [QMK Toolbox](https://qmk.fm/toolbox). This is the easier option if you're unsure about things.





### c. rules.mk
In your rules.mk, you need to enable the console:

```
CONSOLE_ENABLE = yes

```
Optionally, you can also turn on `KEYCODE_STRING_ENABLE`. This comes at the cost of significantly larger firmware, but is highly recommended if you have the space: it will make your logs much easier to read.

```
KEYCODE_STRING_ENABLE = yes
```

### d. config.h

Finally, in your config.h, you need to tell Lumberjack to log the keys:

```
#define LUMBERJACK_LOG_KEYS
```

### Disabling Lumberjack
If you're done with logging for the time being, you can disable Lumberjack (and recoup its firmware space) by commenting out the enabling lines. First, in `rules.mk`, comment out with # symbols:

```
# CONSOLE_ENABLE = yes
# KEYCODE_STRING_ENABLE = yes
```

Then, in `config.h`, comment out with //:

```
// #define LUMBERJACK_LOG_KEYS
```

##Optional Setup
### Coloured Output
![](doc/colored.png)
***Coloured Console:***  *If it's not too distracting, you might prefer a colourised output, which can be enabled in config.h.*

Lumberjack's optional coloured output assigns a unique colour to each keypress, making for easier identification of the UP and DOWN parts of each press, and faster spotting of keypresses that unexpectedly overlap.  It comes at the cost of extra firmware size and is only compatible with the command-line QMK console, not with QMK Toolbox.

Enable coloured output by adding the following to your `config.h`:

```
#define LUMBERJACK_COLOR
```

## Troubleshooting
### My Keycodes are Scrambled!
If your keycodes look something like `0x320B` then, well... that's just what keycodes looks like!  In fact, your keyboard likes them that way.

If you add `KEYCODE_STRING_ENABLE = yes` to your `rules.mk`, then they will become more human readable and look like this: `RSFT_T(KC_H)`.  (`RSFT_T(KC_H)` is a Mod-Tap key that resolves to either "Right Shift" or "H".)

### My Keycodes are Truncated
By default, Lumberjack truncates keycodes over 15 characters long to fit them in the first column of logged output.  You can increase the size of the first column by adding, e.g. `#define LUMBERJACK_KEYCODE_LENGTH 20` to your `config.h`.

### My Firmware is Too Big to Compile
Unfortunately, older keyboards have very little memory, and pretty logging takes up a surprising amount of it.  You have several options to free up space:

1. Follow the instructions on QMK's [Squeezing AVR](https://docs.qmk.fm/squeezing_avr) page, which will free up a lot.
1. Remove `KEYCODE_STRING_ENABLE = yes` from your rules.mk file.  This will make your keycodes much harder to read, but will free ~1,850 bytes.
1. Temporarily comment out parts of your keymap, like LED functionality, that you can live without until you're done debugging.

### Some Deltas are Missing
The timers measure up to a maximum of 60 seconds between keystrokes.  Deltas greater than this are not reported.

### Some Keypresses are Reported as  "Not Tracked"
Lumberjack tracks up to 10 simultaneous keypresses.  If you press 11 keys simultaneously, the 11th keypress will be written to the console but instead of timing data you will see "Not Tracked" instead.

### Some Keypresses are not Coloured
To avoid being too busy, Lumberjack limits its palette to five colours.  The same colour will never be allocated to more than one simultaneous keypress, meaning Lumberjack will only ever colour five keypresses at the same time.  Any additional simultaneous keypresses will not be coloured.

## Appendix A: Resource Requirements

### Firmware Size
The Console is ~1,200 bytes, Lumberjack is ~950 bytes and Keycode String is ~1,850bytes, for a total of ~4,000 bytes.

Lumberjack has a built-in lightweight utilities library (`lumberjack_utils.h/.c`) which provides string and keycode manipulation at a fraction of the size of standard C libraries.  The library may also be useful for other QMK logging applications.


### RAM Usage
Lumberjack uses ~70 bytes of RAM.  You may reduce this by lowering the number of keys which Lumberjack tracks simultaneously.  Each simultaneously tracked key costs four bytes of RAM and Lumberjack tracks up to 10 simultaneous keys by default.

So if you're short on RAM and confident you'll never have more than, say, five keys pressed at the same time, add the following to `config.h`:

```
#define LUMBERJACK_MAX_TRACKED_KEYS 5 // default is 10
```

## Appendix B: Running Tests

The `lumberjack_utils` and `lumberjack_colors` libraries comes with unit tests.  To run them, navigate to the `tests` directory in your terminal and enter `make test`.

<p align="right">
<i>Lumberjack: he likes logs</i>
</p>

