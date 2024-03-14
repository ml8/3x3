# 3x3 Macropad: PCB, Firmware, STL, Programmer

Self-designed, hacked-together 3x3 macropad.

![ml8\_9.jpg](ml8_9.jpg)

Features:

* 4 layers configurable with [via](https://www.caniusevia.com/).
* OLED display to display per-layer text (programmable with tool in
  [`cli/`](cli/README.md)).
* Rotary encoder with volume control and layer select button.
* 256Kb EEPROM (24LC256).

Project layout:

* [`cli/`](cli/README.md) - tool to program keyboard (primarily to change OLED text)
* [`firmware/`](firmware/README.md) - [QMK](https://qmk.fm/)-based firmware (TODO: merge into QMK)
* [`via/`](via/README.md) - Via json config (TODO: merge into via) that can be
  used for programming (once merged into via, this should no longer be
  necessary).
* [`hardware/`](hardware/README.md) Gerber file for PCB and STL/3mf for
  enclosure.
