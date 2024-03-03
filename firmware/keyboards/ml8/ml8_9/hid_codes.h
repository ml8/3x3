#pragma once

#define HID_CODE_HEADER 0x6d6c

enum hid_commands {
    // Basic protocol definitions.
    HID_CMD_NOOP     = 0x00,
    HID_CMD_ERR      = 0x01,
    HID_CMD_ACK      = 0x02,
    HID_CMD_NACK     = 0x03,
    HID_CMD_CONT     = 0x04,
    HID_CMD_ABORT    = 0x05,
    HID_CMD_COMPLETE = 0x06,

    // Debug commands; hello and echo
    HID_CMD_HELLO = 0x30, // 0
    HID_CMD_ECHO  = 0x31,

    // Control commands
    HID_CMD_OLED_OFF = 0x40, // @
    HID_CMD_OLED_ON  = 0x41,

    // OLED programming commands
    HID_CMD_OLED_UPDATE = 0x50, // P
    HID_CMD_OLED_RESET  = 0x51,
};
