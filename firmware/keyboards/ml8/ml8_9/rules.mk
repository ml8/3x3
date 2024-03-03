LTO_ENABLE = yes

ENCODER_ENABLE = yes
OLED_ENABLE = yes
EXTRAFLAGS+=-flto
EXTRAKEY_ENABLE = yes

# enable debugging
CONSOLE_ENABLE = yes
DEBUG_EEPROM_OUTPUT = yes

# enable external eeprom
EEPROM_DRIVER = i2c

# disable unused features
AUDIO_ENABLE   = no
MOUSEKEY_ENABLE = no
SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no
MAGIC_ENABLE = no
AUDIO_ENABLE = no

AVR_USE_MINIMAL_PRINTF = yes

# include all sources
SRC += base.c hid_handlers.c oled_handlers.c persistence.c
