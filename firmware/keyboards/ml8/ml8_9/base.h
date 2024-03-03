#pragma once

#include <stdbool.h>

#include "keycodes.h"

// Layer names, for convenience.
enum layers { L_MEDIA, L_ZOOM, L_NUM, L_UNDEF };
// Keycode for cycling between layers.
enum keycodes {
    KC_CYCLE_LAYERS = QK_USER,
};

bool is_post_init(void);
