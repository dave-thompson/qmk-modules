# Switcher

<table>
<tr><td><b>Module</b></td><td><tt>dave-thompson/switcher</tt></td></tr>
<tr><td><b>Version</b></td><td>2025-08-04</td></tr>
<tr><td><b>Maintainer</b></td><td>Dave Thompson (@dave-thompson)</td></tr>
<tr><td><b>License</b></td><td><a href="../LICENSE.txt">GNU GPLv3</a></td></tr>
</table>

This is community module [Switcher](https://github.com/dave-thompson/qmk-modules/switcher), which makes any app switcher - even the stock MacOS App Switcher - work well on a QMK keyboard.

# Features

## Single Key App Switching

## Secondary Key Presses

## Caching

## Three Ways to Select an App

## Mac OS Niceties

## Automated Initial Sequences

## Advanced Use: Multiple Custom Switcher Keys

# Setup

##1. keymap.json

In `keymap.json`, add the below.  If you don't have a keymap.json yet, just create one (with the below content) in the same directory as your keymap.c file.

```json
{
    "modules": ["dave-thompson/switcher"]
}
```

If you are already using other modules, then in most cases you should add Switcher to **the front of** the list, to prevent your app switcher keystrokes interfering with modules such as Sentence Case.
```json
{
  "modules": ["dave-thompson/switcher", "getreuer/sentence_case"]
}
```

## 2. keymap.c

#TODO : Add setup instructions & example use cases


