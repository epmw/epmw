#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

//this mnemonics is only used for test purposes, you can insert your own mnemonic here and compile it
//however in the upcoming versions, this wallet will be able to either generate new seed at its own
//or recover wallet from existing seed
#define TEST_MNEMONIC "try cook habit comfort jump tissue transfer jar people brown best slogan pigeon earn awful"

#define PIN_CODE_MIN_LENGTH 4
#define PIN_CODE_MAX_LENGTH 8
#define PIN_CODE_MAX_RETRY_COUNT 16

//order in order in which they will be presented to the wallet user
#define VALID_MNEMONIC_LENGTHS {12, 15, 18, 24, 3, 6, 9}
#define MNEMONIC_MAX_LENGTH 24

#endif
