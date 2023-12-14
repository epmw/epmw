#ifndef WALLET_MANAGEMENT_H
#define WALLET_MANAGEMENT_H

#include <stdint.h>
#include "ui/screens/ui_pin_screen.h"

#define WALLET_INITIALIZED_MAGIC 0x12435687
#define WALLET_NOT_INITIALIZED_MAGIC 0x78653421

typedef struct __attribute__((aligned(4))){
	uint32_t initialized;
	pin_code_t pin_code;
	uint8_t pin_failed_attempts;
	uint8_t private_key[32]; //meaning entropy bytes used for mnemonic seed construction
} wallet_data_t;

void wallet_management_init();

uint8_t wallet_is_initialized();

void wallet_set_as_initialized();

void wallet_management_set_private_key(const uint8_t *private_key);

uint8_t wallet_management_check_pin_code(const pin_code_t pin_code);

void wallet_management_set_pin_code(const pin_code_t pin_code);

uint8_t wallet_management_get_pin_failed_attempts();

void wallet_management_set_pin_failed_attempts(const uint8_t pin_failed_attempts);

void wallet_delete();

#endif
