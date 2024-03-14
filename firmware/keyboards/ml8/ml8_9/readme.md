# ml8/ml8\_9

![ml8/ml8\_9](https://github.com/ml8/3x3/blob/main/ml8_9.jpg)

*Firmware for 3x3 macropad with rotary encoder and OLED. Supports via and
reprogramming OLED text via hid programming.*

* Keyboard Maintainer: [Marion Lang](https://github.com/ml8)
* Hardware Supported: [ml8\_9](https://github.com/ml8/3x3)
* Hardware Availability: [open source](https://github.com/ml8/3x3)
* OLED display programmer: [repo](https://github.com/ml8/3x3)

Make example for this keyboard (after setting up your build environment):

    make ml8/ml8_9:default

Flashing example for this keyboard:

    make ml8/ml8_9:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (top left key in 3x3 matrix) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the top of the PCB
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
