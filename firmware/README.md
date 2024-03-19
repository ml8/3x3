# Firmware

You can download the firmware for the macropad from the releases in this
repository. Use the [QMK toolbox](https://github.com/qmk/qmk_toolbox/releases)
to flash the firmware.

## Flashing

I suggest using the standalone QMK toolbox app to flash the firmware. 

1. Install QMK Toolbox.
2. Download (or build) the firmware version you want.
3. Open QMK Toolbox, open the firmware file you downloaded or built, then check
   "auto flash."
4. Unplug the macropad, then hold down the top-left key while plugging it in.
   The macropad should boot into DFU mode and QMK Toolbox should begin flashing
   it.
5. When the process is complete (~10 seconds or so), you can just use the
   macropad normally.

## Modifying/building

If you want to build it yourself and modify it, you'll need to install
[QMK](https://qmk.fm). Then, you can copy or simlink the `keyboards/ml8`
directory into the QMK repo's `keyboards` directory and build normally.

```
[qmk_firmware/] $ make ml8/ml8_9:via
```

Firmware readme [here](keyboards/ml8/ml8_9/readme.md).
