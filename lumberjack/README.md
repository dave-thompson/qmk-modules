# Lumberjack

<table>
<tr><td><b>Module</b></td><td><tt>dave-thompson/lumberjack</tt></td></tr>
<tr><td><b>Version</b></td><td>2025-09-04</td></tr>
<tr><td><b>Maintainer</b></td><td>Dave Thompson (@dave-thompson)</td></tr>
<tr><td><b>License</b></td><td><a href="../LICENSE.txt">GNU GPLv3</a></td></tr>
</table>

## Pretty Key Log for Easier Debugging

Your keyboard doesn't always do what you expect.  Working out **why not** can be tricky.

[Lumberjack](https://github.com/dave-thompson/qmk-modules/lumberjack) logs every key press to the console, with deltas and hold times, letting you see **which keys you pressed and when**, so you can figure out what's going on.

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

### a. Install a QMK Console

Lumberjack logs your keystrokes to a QMK console.  You have two console options:

- **Command Line QMK:** If you don't already have QMK installed on your machine, then follow steps 2 & 3 from the [QMK Setup Guide](https://docs.qmk.fm/newbs_getting_started) to install it.  Once QMK is installed, open a terminal window and type `qmk console` to open a QMK console in that window.

- **QMK Toolbox:**  If you don't want to install QMK or you'd rather avoid using the command line, you can instead install and use [QMK Toolbox](https://qmk.fm/toolbox). This is the easier option if you're unsure about things.

### b. keymap.json

Add Lumberjack to your `keymap.json` file.  If you're already using other modules, then add Lumberjack to the **front** of the list, so it can log your key presses before other modules intercept (and potentially change) them.  Lumberjack will never interfere with modules that come after it.

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

### c. rules.mk
In your rules.mk, you need to enable the console:

```makefile
CONSOLE_ENABLE = yes

```
Optionally, you can also turn on `KEYCODE_STRING_ENABLE`. This comes at the cost of significantly larger firmware, but is highly recommended if you have the space: it will make your logs much easier to read.

```makefile
KEYCODE_STRING_ENABLE = yes
```

### d. config.h

Finally, in your config.h, you need to tell Lumberjack to log the keys:

```c
#define LUMBERJACK_LOG_KEYS
```

### Disabling Lumberjack
If you're done with logging for the time being, you can disable Lumberjack (and recoup its firmware space) by commenting out the enabling lines. First, in `rules.mk`, comment out with # symbols:

```makefile
# CONSOLE_ENABLE = yes
# KEYCODE_STRING_ENABLE = yes
```

Then, in `config.h`, comment out with //:

```c
// #define LUMBERJACK_LOG_KEYS
```

## Optional Setup
### Coloured Output
![](doc/colored.png)
***Coloured Console:***  *If it's not too distracting, you might prefer a colourised output, which can be enabled in config.h.*

Lumberjack's optional coloured output assigns a unique colour to each keypress, making for easier identification of the UP and DOWN parts of each press, and faster spotting of keypresses that unexpectedly overlap.  It comes at the cost of extra firmware size and is only compatible with the command-line QMK console, not with QMK Toolbox.

Enable colours by adding the following to your `config.h`:

```c
#define LUMBERJACK_COLOR
```

## Troubleshooting
### My Keycodes are Scrambled!
If your keycodes look something like `0x320B` then, well... that's just what keycodes looks like!  In fact, your keyboard likes them that way.

If you add `KEYCODE_STRING_ENABLE = yes` to your `rules.mk`, then they will become more human readable and look like this: `RSFT_T(KC_H)`.  (`RSFT_T(KC_H)` is a Mod-Tap key that resolves to either "Right Shift" or "H".)

### My Logged Keycodes Don't Match What Was Typed
Lumberjack hooks into QMK early in its key processing architecture (in `pre_process_record`), with the aim to show you which physical keys you pressed and when.  But QMK does lots of processing after this to determine what keycodes to actually send to your computer.  Layer switches, mod-tap keys, combos, community modules like Sentence Case, and lots more will adjust the typed keycodes before they're sent on.

The hope is that, armed with the data Lumberjack gives you, you can identify which key sequences are causing problems.  Then it's up to you to work out why!  Or, if you're utterly stumped, at least you'll have some data to give to the good people of Reddit or Discord for them to help you out.

### My Firmware is Too Big to Compile
Unfortunately, older keyboards have very little memory, and pretty logging takes up a surprising amount of it.  You have several options to free up space:

1. Follow the instructions on QMK's [Squeezing AVR](https://docs.qmk.fm/squeezing_avr) page, which will free up a lot.
1. Remove `KEYCODE_STRING_ENABLE = yes` from your rules.mk file.  This will make your keycodes much harder to read, but will free ~1,850 bytes.
1. Temporarily comment out parts of your keymap, like LED functionality, that you can live without until you're done debugging.

### My Keycodes are Truncated
By default, Lumberjack truncates keycodes over 15 characters long to fit them in the first column of logged output.  You can increase the size of the first column by adding, e.g. `#define LUMBERJACK_KEYCODE_LENGTH 20` to your `config.h`.

### Some Deltas are Missing
The timers measure up to a maximum of 60 seconds between keystrokes.  Deltas greater than this are not reported.

### Some Key Presses "Not Tracked"
Lumberjack tracks up to 10 simultaneous key presses.  If you press 11 keys simultaneously, the 11th press will still be written to the console but instead of timing data you will see "Not Tracked" instead.  If you want to track more keys than you have fingers to press, you can do so by adding, e.g. `#define LUMBERJACK_MAX_TRACKED_KEYS 15` to your `config.h`.

### Colours Missing from Some Key Presses
To avoid being too busy, Lumberjack limits its palette to five colours.  The same colour will never be allocated to more than one simultaneous key press, meaning Lumberjack will only ever colour five keys at the same time.  Any additional simultaneous key presses will not be in colour.

## Appendix A: Full list of Parameters and Options

#### In rules.mk

<table>
<tr><td><b>Parameter</b></td><td><b>Effect</b></td></tr>
<tr><td><tt>CONSOLE_ENABLE = yes</tt></td><td>Mandatory. Enables use of the console.</td></tr>
<tr><td><tt>KEYCODE_STRING_ENABLE = yes</tt></td><td>Enables human-readable keycodes at the cost of larger firmware size.</td></tr>
</table>

#### In config.h

<table>
<tr><td><b>Parameter</b></td><td><b>Effect</b></td></tr>
<tr><td><tt>LUMBERJACK_LOG_KEYS</tt></td><td>Mandatory. Enables Lumberjack.</td></tr>
<tr><td><tt>LUMBERJACK_COLOR</tt></td><td>Enables coloured logging at the cost of larger firmware size.  Requires use of command-line console.</td></tr>
<tr><td><tt>LUMBERJACK_KEYCODE_LENGTH</tt></td><td>Adjusts the width of the first log column.  Keycodes longer than this length will be truncated.</td></tr>
<tr><td><tt>LUMBERJACK_MAX_TRACKED_KEYS</tt></td><td>Adjusts the maximum number of simultaneously tracked keypresses.  Additional simultaneous keypresses beyond the maximum are logged without hold times and with the message "Not Tracked".</td></tr>



</table>

## Appendix B: Resource Requirements

### Firmware Sizes
The Console is ~1,200 bytes, Lumberjack (Core) is ~1,100 bytes, Keycode String is ~1,850bytes and Lumberjack Colour is ~450bytes, for a total of ~4,600 bytes.

Lumberjack has a built-in lightweight utilities library (`lumberjack_utils.h/.c`) which provides string and keycode manipulation at a fraction of the size of standard C libraries.  The library may also be useful for other QMK logging applications.


### RAM Usage
Lumberjack uses ~115 bytes of static RAM (or ~160 with colours), plus ~60 bytes of stack.  You may reduce RAM usage by lowering the number of keys which Lumberjack tracks simultaneously.  Each simultaneously tracked key costs 11 bytes of RAM and Lumberjack tracks up to 10 simultaneous keys by default.

So if you're short on RAM and confident you'll never have more than, say, five keys pressed at the same time, add the following to `config.h`:

```c
#define LUMBERJACK_MAX_TRACKED_KEYS 5 // default is 10
```

## Appendix C: Running Tests

The `lumberjack_utils` and `lumberjack_colors` libraries comes with unit tests.  To run them, navigate to the `tests` directory in your terminal and enter `make test`.

<p align="right">
<i>Lumberjack: he likes logs</i>
</p>

