#include "oled_handlers.h"

#include <string.h>

#include "base.h"
#include "config.h"
#include "persistence.h"

#include "action_layer.h"
#include "debug.h"
#include "oled_driver.h"
#include "print.h"

// clang-format off
oled_text_t g_sys_layer_text[LAYER_COUNT] = {
  "Media\nPrev | Play | Next \nStop | Mute | ^\n  <  |   >  | v",
  "Zoom\nTalk | Mic  | Video \nFull | Quit | Enter\nHand |      |",
  "Numpad\n 1   |  2   | 3\n 4   |  5   | 6\n 7   |  8   | 9",
  "Undefined"
};
// clang-format on

uint8_t     g_last_layer = LAYER_COUNT; // Not a valid layer
oled_text_t g_layer_text[LAYER_COUNT];
bool        g_oled_on = true;

oled_text_t *system_layer_labels(void) {
    return g_sys_layer_text;
}
oled_text_t *user_layer_labels(void) {
    return g_layer_text;
}

// Update the text for the given layer.
void oled_layer_update(uint8_t layer, char *data, uint8_t length) {
    // validate that the layer is in scope
    if (layer >= '0') {
        layer -= '0';
    }

    dprintf("layer: %d\n", layer);
    if (layer < 0 || layer >= LAYER_COUNT) {
        dprint("invalid layer\n");
        return;
    }

    dprintf("updating layer text with string:\n%s\n", data);
    strncpy(g_layer_text[layer], data, sizeof(oled_text_t));
    g_layer_text[layer][strnlen(data, sizeof(oled_text_t))] = '\0';
    persist_user_layer_labels();
#if defined(OLED_ENABLE)
    if (layer == get_highest_layer(layer_state)) {
        // only need to update if it's the current layer.
        oled_update(layer, true);
    }
#endif
}

// To reduce firmware size.
#if defined(OLED_ENABLE)

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_180;
}

// Update OLED display. Update NOW if force_dirty is true.
void oled_update(uint8_t layer, bool force_dirty) {
    if (layer < 0 || layer >= LAYER_COUNT) {
        dprintf("Request to render invalid layer\n");
        return;
    }
    const char *txt = g_layer_text[layer];
    oled_clear();
    oled_set_cursor(0, 0);
    oled_write(txt, false);
    if (force_dirty) {
        oled_render();
    }
}

// Periodically update OLED display. Only redraw if the layer has changed.
bool oled_task_user(void) {
    if (!is_post_init() || !g_oled_on) {
        return true;
    }
    // render status
    uint8_t curr = get_highest_layer(layer_state);
    if (curr == g_last_layer) {
        return true;
    }
    oled_clear();
    g_last_layer = curr;

    if (curr < 0 || curr >= LAYER_COUNT) {
        // don't write anything
        dprint("oled; invalid layer...\n");
        return true;
    }
    oled_update(curr, false);
    return true;
}
#endif

// Called from hid protocol.
void set_oled_state(bool on) {
    g_oled_on = on;
#if defined(OLED_ENABLE)
    if (!on) {
        // clear oled
        oled_clear();
        oled_render();
    } else {
        oled_update(get_highest_layer(layer_state), true);
    }
#endif
}
