#pragma once

#include <stdbool.h>

#include "keycodes.h"

// Keycode for cycling between layers.
enum keycodes {
    KC_CYCLE_LAYERS = QK_USER,
};

bool is_post_init(void);
