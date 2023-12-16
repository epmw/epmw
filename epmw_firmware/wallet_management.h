#ifndef WALLET_MANAGEMENT_H
#define WALLET_MANAGEMENT_H

#include <stdint.h>

#include "config.h"

#include "ui/screens/ui_pin_screen.h"

#define WALLET_INITIALIZED_MAGIC 0x12435687
#define WALLET_NOT_INITIALIZED_MAGIC 0x78653421

typedef struct __attribute__((aligned(4))){
	uint32_t initialized;
	pin_code_t pin_code;
	uint8_t pin_failed_attempts;
	//count of word in the generated/recovered mnemonic seed
	uint8_t wc;
	//this is not the most efficient saving format, however it is much more computation optimal
	//since we don't have to convert between raw entropy --> seed words each time when wallet
	//is powered up
	uint16_t words[MNEMONIC_MAX_LENGTH];
} wallet_data_t;

void wallet_management_init();

uint8_t wallet_is_initialized();

void wallet_set_as_initialized();

void wallet_management_set_mnemonic_seed(const uint8_t wc, const uint16_t *words);

uint8_t wallet_management_get_mnemonic_seed(uint16_t **words);

uint8_t wallet_management_check_pin_code(const pin_code_t pin_code);

void wallet_management_set_pin_code(const pin_code_t pin_code);

uint8_t wallet_management_get_pin_failed_attempts();

void wallet_management_set_pin_failed_attempts(const uint8_t pin_failed_attempts);

void wallet_delete();

#endif
