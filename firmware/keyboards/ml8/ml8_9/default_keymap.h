#pragma once

#include "config.h"

#include <stdint.h>

#include QMK_KEYBOARD_H

// Layer names, for convenience.
enum layers { L_MEDIA, L_ZOOM, L_MEET, L_TEAMS };

extern const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2];
extern const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS];
