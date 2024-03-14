#include "default_keymap.h"

#include "base.h"
#include "config.h"

#include <stdint.h>

#include QMK_KEYBOARD_H

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {[0] = {ENCODER_CCW_CW(KC_VOLU, KC_VOLD)}};
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // clang-format off
    [L_MEDIA] = LAYOUT_ortho_3x4(
        KC_MEDIA_PREV_TRACK, KC_MEDIA_PLAY_PAUSE, KC_MEDIA_NEXT_TRACK, 
        KC_MEDIA_STOP, KC_AUDIO_MUTE, KC_UP,
        KC_LEFT,       KC_RIGHT,      KC_DOWN, 
        KC_CYCLE_LAYERS, KC_NO,      KC_NO
    ),
    // For video calls:
    // push-to-talk  |  mute/unmute  |  video on/off
    // full screen   |  quit         |  confirm quit
    // raise/lower   |   x           |  x
    [L_ZOOM] = LAYOUT_ortho_3x4(
        KC_SPACE,   SGUI(KC_A), SGUI(KC_V), 
        SGUI(KC_F), LGUI(KC_W), KC_ENTER, 
        LOPT(KC_Y), KC_NO,      KC_NO, 
        KC_CYCLE_LAYERS,  KC_NO,      KC_NO
    ),
    [L_MEET] = LAYOUT_ortho_3x4(
        KC_SPACE,  LGUI(KC_D),  LGUI(KC_E), 
        LGUI(LCTL(KC_F)),  LGUI(KC_W),  KC_ENTER, 
        LGUI(LCTL(KC_H)),  KC_NO,  KC_NO, 
        KC_CYCLE_LAYERS, KC_NO, KC_NO
    ),
    [L_TEAMS] = LAYOUT_ortho_3x4(
        LOPT(KC_SPACE), SGUI(KC_M), SGUI(KC_O),
        LGUI(LCTL(KC_F)), SGUI(KC_H), KC_ENTER,
        SGUI(KC_K), KC_NO, KC_NO,
        KC_CYCLE_LAYERS, KC_NO, KC_NO
    )
    // clang-format on
};
