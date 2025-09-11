# @dave-thompson's QMK community modules


![](doc/banner.png)
**Lumberjack**, one of the library's modules, shows you every key you pressed and when.  It helps you debug home-row-mods and other timing-dependent features.</br></br>

| Module                                    | Description                                           |
|-------------------------------------------|-------------------------------------------------------|
| [Lumberjack](./lumberjack/)    | **See which keys you pressed and when.**  Helps you debug timing-dependent features.</br></br>                    |
| [Switcher](./switcher/)        | **Supercharge your app switcher.**  Send Cmd-Tab or Alt-Tab with a single keypress, map navigation and power features to the home row, and a whole lot more.</br></br>                |

## License

This repo uses the GNU GPLv3 except where otherwise indicated. See the
[LICENSE file](LICENSE.txt) for details.

## Prerequisites

Your keyboard must support **Community Modules**, meaning it must be using **QMK Firmware 0.28.0** (released 2025-02-27) or later.  If (and only if) your qmk_firmware folder has a 'modules' folder inside it, then you're good to go.

## Installation

Download the code to your computer using one of the two options below, then follow the README instructions for the specific module you'd like to install.

### Option 1

Open a terminal and run the following three commands.  Change `/path/to/qmk_firmware` to the path of your `qmk_firmware` folder (or if you're using [external userspace](https://docs.qmk.fm/newbs_external_userspace), to the path of your `qmk_userspace` folder).

```sh
cd /path/to/qmk_firmware
git submodule add https://github.com/dave-thompson/qmk-modules.git modules/dave-thompson
git submodule update --init --recursive
```

### Option 2

Download [qmk-modules-main](https://github.com/dave-thompson/qmk-modules/archive/refs/heads/main.zip) and unzip it.  Rename the resulting `qmk-modules-main` folder to `dave-thompson` and put it in your `modules` folder.

Whichever option you choose, your directory structure should then look like this:

    <qmk_firmware or qmk_userspace>
    └── modules
        └── dave-thompson
            ├── doc
            ├── lumberjack
            ├── switcher
            └── ...