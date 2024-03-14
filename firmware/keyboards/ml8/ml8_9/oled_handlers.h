#pragma once

#include "base.h"

#include <stdbool.h>
#include <stdint.h>

// OLED supports 4 lines of 21 chars (+4 for newlines and final null terminator)
typedef char oled_text_t[4 * 21 + 3];

// Update the text for a given layer of the OLED.
void oled_layer_update(uint8_t layer, char *data, uint8_t length);
// Update the display, optionally forcing it to be rendered.
void oled_update(uint8_t layer, bool force_dirty);
// Turn on/off the oled.
void set_oled_state(bool on);

oled_text_t *system_layer_labels(void);
oled_text_t *user_layer_labels(void);
