# CLI programming tool

Requires golang to be installed.

The programming tool uses device id to distinguish between other QMK keyboards
that may be attached to the same machine. If you only have one QMK keyboard, you
can leave off the device string.

Compile:

```
$ make
```

To program layer text:

```
$ ./kbp -device ::6d6c::: -layer [0-3] -text "TEXT"
```

You can use `\n` for new lines. All text will be written to a single wrapped
line, otherwise.

To reset the layer text:

```
$ ./kbp -device ::6d6c::: -reset
```

Other commands (turning off the OLED, etc.) can be found by using the `-help`
flag.
