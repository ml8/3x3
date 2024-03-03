#include "persistence.h"

#include "config.h"
#include "oled_handlers.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "action_layer.h"
#include "debug.h"
#include "eeprom.h"
#include "print.h"
#include "via.h"

#define EEPROM_MAGIC_WORD 0xdead
#define PERSISTENCE_VERSION 666
#define EEPROM_OLED_VALID_CFG 0x55
#define EEPROM_OLED_INVALID_CFG 0xff

#define EEPROM_BASE_ADDR (void *)VIA_EEPROM_CUSTOM_CONFIG_ADDR
#define EEPROM_MAGIC_ADDR (void *)(0 + EEPROM_BASE_ADDR)
#define EEPROM_VERSION_ADDR (void *)(2 + EEPROM_BASE_ADDR)
#define EEPROM_OLED_RESTORE_ADDR (void *)(4 + EEPROM_BASE_ADDR)
#define EEPROM_OLED_CFG_ADDR (void *)(4 + (8 * sizeof(oled_text_t)) + EEPROM_BASE_ADDR)

struct oled_cfg {
    char valid; // whether the data is valid; set to 0x55
                // char data_start;
                // unused; start of data for layers. data is stored
                // contiguously with the layout d0 d1 d2 ... where
                // d0 is sizeof(oled_text_t) and contains the data for layer 0 (and so on).
};

// EEPROM layout (version 666)
//     +--------+ EEPROM_BASE_ADDR/EEPROM_MAGIC_ADDR
//     |  dead  |    - magic
//     +--------+ +2
//     |   1    |    - version
//     +--------+ +4
//     | layer0 |    - restore data (for up to 8 layers)
//     |  ...   |
//     |  ...   |
//     | layer1 |
//     |  ...   |
//     |  ...   |
//     |  ...   |
//     +--------+ +4 + 8*sizeof(oled_text_t)
//     |   55   |     - valid oled text
//     | layer0 |     - layer 0 data
//     |  ...   |
//     |  ...   |
//     | layer1 |     - layer 1 data
//     |  ...   |
//     |  ...   |
//     |        |
//     |        |
//     +--------+
//

#if defined(EEPROM_CFG)
bool eeprom_is_init(void) {
    uint16_t magic;
    eeprom_read_block(&magic, EEPROM_MAGIC_ADDR, sizeof(magic));
    dprintf("eeprom magic %d\n", magic);
    return magic == EEPROM_MAGIC_WORD;
}

uint16_t eeprom_version(void) {
    uint16_t v;
    eeprom_read_block(&v, EEPROM_VERSION_ADDR, sizeof(v));
    dprintf("eeprom persistence version %d\n", v);
    return v;
}

// Persist system layers. Read upon reset of layer text.
void eeprom_persist_system_layers(void) {
    // TODO -- config is not really useful/used so far, refactor
    // persistence/recovery for system and user layers.
    dprint("persisting system layer labels...\n");
    oled_text_t buffer;
    void       *p = EEPROM_OLED_RESTORE_ADDR;
    for (int i = 0; i < LAYER_COUNT; i++) {
        dprintf("\tlayer %d\n", i);
        strncpy(buffer, system_layer_labels()[i], sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';
        // write at p, then increment pointer offset
        eeprom_write_block(buffer, p, sizeof(buffer));
        p += sizeof(buffer);
    }
    dprint("done\n");
}

// Recover system layers.
void eeprom_restore_system_layers(void) {
    oled_text_t buffer;
    void       *p = EEPROM_OLED_RESTORE_ADDR;
    dprintf("restoring system layer labels...\n");
    for (int i = 0; i < LAYER_COUNT; i++) {
        dprintf("\tlayer %d\n", i);
        eeprom_read_block(buffer, p, sizeof(buffer));
        strncpy(user_layer_labels()[i], buffer, sizeof(oled_text_t));
        p += sizeof(buffer);
    }
    dprint("done\n");
}

// Persist user layers. Read at start.
void eeprom_persist_user_layers(void) {
    oled_text_t     buffer;
    struct oled_cfg config;
    void           *p = EEPROM_OLED_CFG_ADDR;

    dprint("persisting user layer labels...\n");
    dprint("\theader...\n");
    config.valid = EEPROM_OLED_VALID_CFG;
    eeprom_write_block(&config, p, sizeof(config));
    p += sizeof(struct oled_cfg);
    for (int i = 0; i < LAYER_COUNT; i++) {
        dprintf("\tlayer %d...\n", i);
        strncpy(buffer, user_layer_labels()[i], sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';
        eeprom_write_block(buffer, p, sizeof(buffer));
        p += sizeof(oled_text_t);
    }
    dprint("done\n");
}

// Clear persisted user layers. Destory valid marker.
void eeprom_clear_user_layers(void) {
    struct oled_cfg config;
    config.valid = EEPROM_OLED_INVALID_CFG;
    void *p      = EEPROM_OLED_CFG_ADDR;
    dprintf("voiding user layer labels\n");
    eeprom_write_block(&config, p, sizeof(config));
    dprintf("done\n");
}

// Restore user layers. Called on recovery.
void eeprom_restore_user_layers(void) {
    oled_text_t     buffer;
    struct oled_cfg config;
    void           *p = EEPROM_OLED_CFG_ADDR;
    dprint("looking for user layers in eeprom...\n");
    eeprom_read_block(&config, p, sizeof(config));
    dprintf("\tfound marker %d (wanted %d)\n", config.valid, EEPROM_OLED_VALID_CFG);
    if (config.valid != EEPROM_OLED_VALID_CFG) {
        dprintf("no layers to restore\n");
        return;
    }
    p += sizeof(struct oled_cfg);
    for (int i = 0; i < LAYER_COUNT; i++) {
        dprintf("\tlayer %d...\n", i);
        eeprom_read_block(buffer, p, sizeof(oled_text_t));
        strncpy(user_layer_labels()[i], buffer, sizeof(oled_text_t));
        p += sizeof(oled_text_t);
    }
    dprintf("done!\n");
}

// Initialize eeprom
void eeprom_config_init(uint16_t version) {
    uint16_t buff[2] = {EEPROM_MAGIC_WORD, version};
    dprintf("initializing eeprom\n");
    eeprom_write_block(&buff, EEPROM_MAGIC_ADDR, sizeof(buff));
    if (!eeprom_is_init()) {
        dprint("eeprom write failed\n");
    } else {
        dprint("eeprom initialized\n");
    }
    // persist system layers when initializing eeprom
    eeprom_persist_system_layers();
}
#endif

// Write user layer labels to persistence
void persist_user_layer_labels(void) {
#if defined(EEPROM_CFG)
    eeprom_persist_user_layers();
#else
    dprintf("no recovery medium; not persisting.\n");
#endif
}

void recover_from_in_mem_system_layer_labels(void) {
    for (int i = 0; i < LAYER_COUNT; i++) {
        strncpy(user_layer_labels()[i], system_layer_labels()[i], sizeof(oled_text_t));
    }
}

// Restore user layer labels from persistence
void restore_user_layer_labels(void) {
#if defined(EEPROM_CFG)
    eeprom_restore_user_layers();
    oled_update(get_highest_layer(layer_state), true);
#else
    dprintf("no recovery medium; using default.\n");
    recover_from_in_mem_system_layer_labels();
#endif
}

// Reset layer labels to initial values
void reset_layer_labels(void) {
    // reset to initial config.
#if defined(EEPROM_CFG)
    eeprom_restore_system_layers();
    oled_update(get_highest_layer(layer_state), true);
    eeprom_clear_user_layers();
#else
    dprintf("no recover medium; using default.\n");
    recover_from_in_mem_system_layer_labels();
#endif
}

void persistence_init(void) {
    recover_from_in_mem_system_layer_labels();

#if defined(EEPROM_CFG)
    // check if eeprom is initialized.
    if (eeprom_is_init()) {
        dprint("eeprom initialized!\n");
        uint16_t version = eeprom_version();
        if (version != PERSISTENCE_VERSION) {
            // TODO: migrate persistence.
            dprint("persistence mismatch!\n");
            eeprom_config_init(PERSISTENCE_VERSION);
        } else {
            eeprom_restore_user_layers();
        }
    } else {
        eeprom_config_init(PERSISTENCE_VERSION);
    }
#endif
}
