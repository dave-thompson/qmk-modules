# Lumberjack

<table>
<tr><td><b>Module</b></td><td><tt>dave-thompson/lumberjack</tt></td></tr>
<tr><td><b>Version</b></td><td>2025-08-20</td></tr>
<tr><td><b>Maintainer</b></td><td>Dave Thompson (@dave-thompson)</td></tr>
<tr><td><b>License</b></td><td><a href="../LICENSE.txt">GNU GPLv3</a></td></tr>
</table>

## Pretty Key Log for Easier Debugging

Sometimes, your keyboard doesn't do what you expect, and figuring out **why on earth not** can be tricky.

[Lumberjack](https://github.com/dave-thompson/qmk-modules/lumberjack) logs every key press to the console, with deltas and hold times, so you can see what's going on.

## Features

![](doc/lumberjack.png)
***The QMK Console:***  *Lumberjack records every key press to the console so you can see exactly which keys you pressed and when*

Every key press is shown with four pieces of information:

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

### b. Install QMK Toolbox
Lumberjack logs your keystrokes to the QMK console.  If you don't have a QMK Console already, your easiest option is to install and use [QMK Toolbox](https://qmk.fm/toolbox).





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

## Troubleshooting
### My Keycodes are Scrambled!
If your keycodes look something like `0x320B` then, well... that's just what a keycode looks like!  In fact, your keyboard likes them that way.

If you add `KEYCODE_STRING_ENABLE = yes` to your `rules.mk` then they will become more human readable and look like this: `RSFT_T(KC_H)`.  (In case `RSFT_T(KC_H)` still seems somewhat cryptic, it's a Mod-Tap key that resolves to either "Right Shift" or "H".)

### My Firmware is Too Big to Compile
Unfortunately, older keyboards have very little memory, and pretty logging takes up a surprising amount of it.  You have several options to free up space:

1. Follow the instructions on QMK's [Squeezing AVR](https://docs.qmk.fm/squeezing_avr) page, which will free up a lot.  (However, do NOT follow their suggestion to enable `AVR_USE_MINIMAL_PRINTF`, as it is not compatible with Lumberjack.)
1. Remove `KEYCODE_STRING_ENABLE = yes` from your rules.mk file.  This will make your keycodes much harder to read, but will free up quite a bit of space too.
1. Temporarily comment out parts of your keymap, like LED functionality, that you can live without until you're done debugging.

### Some Deltas are Negative
Yes, this happens!  Due to the way that matrix scanning works, QMK's key event sequence can be slightly non-chronological.

### Some Deltas are Missing
The timers measure up to a maximum of 60 seconds between keystrokes.  Deltas greater than this are not reported.

## Appendix A: Resource Requirements

### Firmware Size
The Console is ~1,200 bytes, Lumberjack is ~1,900 bytes and Keycode String is ~1,950bytes, for a total of ~5,050 bytes.


### RAM Usage
Lumberjack uses ~70 bytes of RAM.  You may reduce this by lowering the number of keys which Lumberjack tracks simultaneously.  If you're confident you'll never have more than five keys pressed at the same time, you could add the below line to `config.h`.  Each simultaneously tracked key costs four bytes of RAM.

```
#define LUMBERJACK_MAX_TRACKED_KEYS 5 // default is 10
```

<p align="right">
<i>Lumberjack: He's good with logs</i>
</p>
