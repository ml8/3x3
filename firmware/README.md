# Firmware

You can download the firmware for the macropad from the releases in this
repository. Use the [QMK toolbox](https://github.com/qmk/qmk_toolbox/releases)
to flash the firmware.

If you want to build it yourself and modify it, you'll need to install
[QMK](https://qmk.fm). Then, you can copy or simlink the `keyboards/ml8`
directory into the QMK repo's `keyboards` directory and build normally.

```
[qmk_firmware/] $ make ml8/ml8_9:via
```

Firmware readme [here](keyboards/ml8/ml8_9/readme.md).
