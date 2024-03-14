#include "hid_handlers.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "hid_codes.h"
#include "oled_handlers.h"
#include "persistence.h"

#include "debug.h"
#include "print.h"
#include "raw_hid.h"

// Transfer state -- transfers via hid are chunked at 32 bytes
struct transfer_state {
    uint8_t     cur_operation; // inflight operation
    uint8_t     cur_layer;     // layer operation applies to
    uint8_t     buffer_offset; // next offset to write to
    oled_text_t buffer;        // buffer to use for chunked operations
} g_transfer_state;

// Clear transfer state
void reset_transfer_state(void) {
    g_transfer_state.buffer_offset = 0;
    g_transfer_state.cur_operation = HID_CMD_NOOP;
}

// Send a nack
void nack_hid_message(void) {
    dprintf("sending nack.\n");
    uint8_t buffer[32]; // 32-byte buffer required for sends
    buffer[0] = HID_CMD_NACK;
    raw_hid_send(buffer, 32);
}

// Send an ack
void ack_hid_message(uint8_t cmd) {
    dprintf("sending ack for cmd %d\n", cmd);
    uint8_t buffer[32]; // 32-byte buffer required for sends
    buffer[0] = HID_CMD_ACK;
    buffer[1] = cmd;
    raw_hid_send(buffer, 32);
}

// Complete an in-flight layer text update.
void complete_oled_layer_update(void) {
    g_transfer_state.buffer[g_transfer_state.buffer_offset] = '\0';
    oled_layer_update(g_transfer_state.cur_layer, g_transfer_state.buffer, g_transfer_state.buffer_offset);
    reset_transfer_state();
}

// Begin or continue an oled layer update.
// oled layer update messages have the following format: < L N data... > where
// L is the layer number and N is the length of the data that follows.
bool start_or_continue_oled_layer_update(uint8_t cmd, uint8_t *buffer) {
    uint8_t layer = buffer[0];
    uint8_t len   = buffer[1];
    dprintf("layer %d; text len %d\n", layer, len);

    if (len > 27) {
        // 32 bytes minus 5 byte header
        dprintf("message malformed; length of %d\n", len);
        return false;
    }

    if (layer < 0 || layer >= LAYER_COUNT) {
        dprintf("invalid layer number %d\n", layer);
        return false;
    }

    // validate command
    switch (cmd) {
        case HID_CMD_OLED_UPDATE:
            // a new transfer; consider previous aborted.
            dprintf("new transfer; considering any in-flight aborted.\n");
            reset_transfer_state();
            g_transfer_state.cur_operation = HID_CMD_OLED_UPDATE;
            break;

        case HID_CMD_CONT:
        case HID_CMD_COMPLETE:
            // an inflight transfer; validate that the transfer is known and
            // consistent.
            if (g_transfer_state.cur_layer != layer) {
                // layer changed mid-transfer!
                dprintf("layer has changed from %d to %d\n", g_transfer_state.cur_layer, layer);

                return false;
            }
            break;
        default:
            dprintf("unsupported command %d!\n", cmd);
            // TODO here and below: send HID_CMD_ERR
            return false;
    }

    if (cmd == HID_CMD_COMPLETE) {
        dprintf("completing update\n");
        // complete the update
        complete_oled_layer_update();
        reset_transfer_state();
        return true;
    }

    g_transfer_state.cur_layer = layer;

    if (len < 1) {
        // no data to buffer
        dprintf("empty message\n");
    } else if (len + g_transfer_state.buffer_offset > sizeof(g_transfer_state.buffer)) {
        // TODO move this check when cleaning up code.
        // too much data to buffer.
        dprintf("buffered data too large: %d\n", len + g_transfer_state.buffer_offset);
        return false;
    } else {
        // buffer data
        dprintf("buffering %d bytes...\n", len);
        strncpy(&g_transfer_state.buffer[g_transfer_state.buffer_offset], (char *)&buffer[2], len);
        g_transfer_state.buffer_offset += len;
        // null terminate for debug printing
        g_transfer_state.buffer[g_transfer_state.buffer_offset] = '\0';
        dprintf("current buffer: %s\n", g_transfer_state.buffer);
    }
    return true;
}

// Handle a hid command that may require multiple rounds.
void start_or_continue_hid_command(uint8_t cmd, uint8_t *buffer) {
    // Only chunked transfer is oled for now.
    if (!start_or_continue_oled_layer_update(cmd, buffer)) {
        dprintf("transfer failed\n");
        reset_transfer_state();
        nack_hid_message();
        return;
    }

    ack_hid_message(cmd);
    return;
}

// Echo a message
void hid_echo(uint8_t *buffer) {
    uint8_t snd[32];
    snd[0] = HID_CMD_ACK;
    // 2 byte header + 1 byte cmd + 1 byte ACK -> 28 bytes remaining.
    memcpy(&snd[1], buffer, 28);
    raw_hid_send(snd, 32);
}

// Say hi
void hid_hello(void) {
    uint8_t snd[32];
    snd[0] = HID_CMD_ACK;
    snd[1] = HID_CMD_HELLO;
    // 30 bytes remaining
    strncpy((char *)&snd[2], "hello world", 30);
    raw_hid_send(snd, 32);
}

// Given a hid command and associated data, dispatch to handler.
bool handle_hid_command(int16_t cmd, uint8_t *buffer) {
    bool oled_on = true;
    switch (cmd) {
        case HID_CMD_HELLO:
            hid_hello();
            break;

        case HID_CMD_ECHO:
            hid_echo(buffer);
            break;

        case HID_CMD_OLED_OFF:
            oled_on = false;
            // fallthrough intentional.
        case HID_CMD_OLED_ON:
            set_oled_state(oled_on);
            dprintf("Setting oled state to on = %d\n", oled_on);
            ack_hid_message(cmd);
            break;

        case HID_CMD_OLED_RESET:
            dprintf("resetting oled state\n");
            reset_layer_labels();
            ack_hid_message(cmd);
            break;

        case HID_CMD_OLED_UPDATE:
        case HID_CMD_COMPLETE:
        case HID_CMD_CONT:
            dprintf("starting or continuing OLED update; current command %d, inflight "
                    "operation %d\n",
                    cmd, g_transfer_state.cur_operation);
            start_or_continue_hid_command(cmd, buffer);
            break;

        default:
            // TODO: should this be passed to via to handle? i.e. return false.
            dprintf("unknown command %d\n", cmd);
            nack_hid_message();
            break;
    }

    return true;
}

// Returns -1 on error or parsed command
int16_t validate_hid_message(uint8_t *data) {
    uint16_t header = data[0] << 8 | data[1]; // fix message endianness
    dprintf("Header was: %d \n", header);
    dprintf("Data is: %d %d\n", data[0], data[1]);
    if (header != HID_CODE_HEADER) {
        return -1;
    }

    if (debug_enable) {
        dprintf("Got: ");
        for (int i = 0; i < 32; i++) {
            dprintf("%d ", data[i]);
        }
        dprintf("\n");
    }

    uint8_t cmd = data[2];
    dprintf("cmd %d\n", cmd);

    return cmd;
}

// Returns false iff via should handle the message.
// Message format is <m l C ...>, where C is a command from hid_codes.h
bool user_hid_receive(uint8_t *data, uint8_t length) {
    // length is meaningless here, it will always be a 32-byte frame.
    dprintf("received hid message.\n");
    int16_t cmd = validate_hid_message(data);
    if (cmd < 0) {
        // not for us.
        return false;
    }
    return handle_hid_command(cmd, &data[3]);
}

#if defined(VIA_ENABLE)
// When via is enabled, we get a callback from via.
bool via_command_kb(uint8_t *data, uint8_t length) {
    return user_hid_receive(data, length);
}
#else
// When via is disabled, we own hid messages.
void raw_hid_receive(uint8_t *data, uint8_t length) {
    user_hid_receive(data, length);
}
#endif
