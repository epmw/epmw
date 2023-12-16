#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define PIN_CODE_MIN_LENGTH 4
#define PIN_CODE_MAX_LENGTH 8
#define PIN_CODE_MAX_RETRY_COUNT 16

//order in order in which they will be presented to the wallet user
#define VALID_MNEMONIC_LENGTHS {12, 15, 18, 24, 3, 6, 9}
#define MNEMONIC_MAX_LENGTH 24

#endif
