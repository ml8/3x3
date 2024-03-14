#include "base.h"

#include "config.h"
#include "persistence.h"

#include <stdbool.h>
#include <stdint.h>

#include "action.h"
#include "action_layer.h"
#include "debug.h"
#include "print.h"
#include "wait.h"

bool g_post_init = 0; // true iff initialization is complete

bool is_post_init(void) {
    return g_post_init;
}

void init(void) {
#if defined(CONSOLE_ENABLE)
    debug_matrix = true;
    debug_enable = true;
    // when debug mode/console is active, wait a bit to connect.
    wait_ms(1000);
    dprint("hi 0v0\n");
#endif
    // initialize layer labels
    persistence_init();
    g_post_init = 1;
}

void keyboard_post_init_user(void) {
    if (!g_post_init) {
        init();
    }
}

// Move to next layer
void cycle_layer(void) {
    uint8_t curr = get_highest_layer(layer_state);
    if (curr < 0 || curr >= LAYER_COUNT) {
        return;
    }
    layer_move((curr + 1) % LAYER_COUNT);
}

// Listen for custom keycode
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_CYCLE_LAYERS:
            if (!record->event.pressed) {
                return false;
            }
            cycle_layer();
            return false;
        default:
            return true;
    }
}

uint16_t keycode_config(uint16_t keycode) {
    return keycode;
}

uint8_t mod_config(uint8_t mod) {
    return mod;
}
