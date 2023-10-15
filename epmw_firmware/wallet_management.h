#ifndef WALLET_MANAGEMENT_H
#define WALLET_MANAGEMENT_H

#include <stdint.h>

#define WALLET_INITIALIZED_MAGIC 0x12435687
#define WALLET_NOT_INITIALIZED_MAGIC 0x78653421

typedef struct __attribute__((aligned(4))){
	uint32_t initialized;
	uint8_t private_key[32]; //meaning entropy bytes used for mnemonic seed construction
} wallet_data_t;

void wallet_management_init();

uint8_t wallet_is_initialized();

void wallet_set_initialized(const uint8_t state);

#endif
