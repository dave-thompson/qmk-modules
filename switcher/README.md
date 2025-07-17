# Switcher

<table>
<tr><td><b>Module</b></td><td><tt>dave-thompson/switcher</tt></td></tr>
<tr><td><b>Version</b></td><td>2025-07-17</td></tr>
<tr><td><b>Maintainer</b></td><td>Dave Thompson (@dave-thompson)</td></tr>
<tr><td><b>License</b></td><td><a href="../LICENSE.txt">GNU GPLv3</a></td></tr>
</table>

This is community module [Switcher](https://github.com/dave-thompson/qmk-modules/switcher).  Switcher is a fuller-featured re-imagining of @callum-oakley's [Swapper](https://github.com/callum-oakley/qmk_firmware/tree/master/users/callum).

It allows full use of MacOS's app switcher on a QMK mechanical keyboard, no matter how your keyboard is configured, and including all the things you never even knew the app switcher could do.

Indebted to @getreuer's [superbly documented examples](https://getreuer.info/posts/keyboards/index.html).

# Usage

##1. keymap.json

In `keymap.json`, add the following:

```json
{
    "modules": ["dave-thompson/switcher"]
}
```
If you don't already have a keymap.json file, simply create one with the above content in the same directory as your keymap.c file.

If you are also using other modules, you will usually want Switcher to be the first in the list.  As a minimum, it should go before any modules which process the same keycodes when Switcher is not active.  This ensures correct behaviour as follows:
- When Switcher is *not* active, it will simply allow keystrokes to cascade through to other modules.
- When Switcher *is* active (i.e. when the app switcher modal is open), it will intercept all keystrokes before they get to any other modules, thus correctly interpreting them in the app switcher context, rather than in the context of whatever applications you have underneath.

## 2. keymap.c

#TODO : Add setup instructions & example use cases
