#ifndef UI_SEED_ENTER_SCREEN_H
#define UI_SEED_ENTER_SCREEN_H

#include <stdint.h>

//todo may be move to c file once not needed anymore there
typedef uint32_t (*retrieve_string_based_info_funct_ptr_t)(const char *);

uint8_t ui_retrieve_mnemonic_seed_from_user(uint16_t *words);

#endif
