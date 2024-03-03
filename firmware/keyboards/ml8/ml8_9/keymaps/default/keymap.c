#include "base.h"

#include QMK_KEYBOARD_H

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {[0] = {ENCODER_CCW_CW(KC_VOLU, KC_VOLD)}};
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // clang-format off
    [L_MEDIA] = LAYOUT_ortho_3x4(
        KC_MEDIA_PREV_TRACK, KC_MEDIA_PLAY_PAUSE, KC_MEDIA_NEXT_TRACK, 
        KC_MEDIA_STOP, KC_AUDIO_MUTE, KC_UP,
        KC_LEFT,       KC_RIGHT,      KC_DOWN, 
        KC_CYCLE_LAYERS, KC_NO,      KC_NO
    ),
    [L_ZOOM] = LAYOUT_ortho_3x4(
        KC_SPACE,   SGUI(KC_A), SGUI(KC_V), 
        SGUI(KC_F), LGUI(KC_W), KC_ENTER, 
        LOPT(KC_Y), KC_NO,      KC_NO, 
        KC_CYCLE_LAYERS,  KC_NO,      KC_NO
    ),
    [L_NUM] = LAYOUT_ortho_3x4(
        KC_1,  KC_2,  KC_3, 
        KC_4,  KC_5,  KC_6, 
        KC_7,  KC_8,  KC_9, 
        KC_CYCLE_LAYERS, KC_NO, KC_NO
    ),
    [L_UNDEF] = LAYOUT_ortho_3x4(
        KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO,
        KC_CYCLE_LAYERS, KC_NO, KC_NO
    )
    // clang-format on
};
