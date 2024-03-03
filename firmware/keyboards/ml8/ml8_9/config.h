#pragma once

// Reduce firmware size
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#define NO_ACTION_ONESHOT
#define NO_ACTION_TAPPING
#define NO_MUSIC_MODE
// Reduce firmware size but limit to 8 layers
#define LAYER_STATE_8BIT

// clang-format off
#define ENCODERS_PAD_A { B5 }
#define ENCODERS_PAD_B { B4 }
#define ENCODER_RESOLUTION 4
// clang-format on

// Enable external EEPROM
#define EEPROM_I2C_24LC256
// reserve 8k for our use
#define VIA_EEPROM_CUSTOM_CONFIG_SIZE (1 << 13)

// Layer config
#define LAYER_COUNT 4

// Enable storing configuration in eeprom
#define EEPROM_CFG
