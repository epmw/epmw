#ifndef WALLET_H
#define WALLET_H

#include <stdint.h>

uint8_t wallet_xpub_from_mnemonic(char *xpub_buffer, const char *mnemonic);

uint8_t wallet_address_from_mnemonic(char *address, const char *mnemonic, const uint32_t child_index);

uint8_t wallet_ergo_schnorr_sign(char *signed_msg_buffer, const uint8_t *msg, uint8_t *sk, uint8_t *random_bytes);

#endif
