/* +----------------------------------+ */
/* |              BIP39               | */
/* |             bip39.h              | */
/* |   (c)copyright nitram147 2021    | */
/* +----------------------------------+ */

#ifndef BIP39_H
#define BIP39_H

#include <stdint.h>//required for uint definitions

#define BIP_39_WORDS_COUNT 2048

#define BIP39_INVALID_WORD_INDEX 0xffff

#define BIP39_LONGEST_WORD_CHARS_COUNT 8

//todo move me out of header file (in case that we don't need this #defines to be available else where)
#define BIP39_STRCMP_EQ 0
#define BIP39_STRCMP_LT 1
#define BIP39_STRCMP_GT 2


//           |||
//todo fixme vvv !!!!
uint8_t bip39_strlen(const char *str);
uint8_t bip39_strcmp(const char *str1, const char *str2);


//todo rename "new_" (optimalized) versions to other name, or rename non-optimalized function to some name

//todo write short comment explaining functionality of functions below


// Calculate required memory size (count of words) for encoding of n-bytes
// n - count of bytes to encode
// include_padding:
//     if true - encode all n-bytes, in case that padding is needed (((n*8)%11) != 0) include this fact in the calculation 
//     if else - encode only bits which fits precisely into X words (where X = (n*8/11) <-- use integer division), drop remaining (if any)
uint8_t bip39_required_memory_size_encoding(const uint8_t n, const uint8_t include_padding);

// Calculate required memory size (count of bytes) for decoding of n-words
// n - count of words to decode from
// extract_padding:
//     if true - decode all n-words, in case that padding is needed (((n*11)%8) != 0) include this fact in the calculation 
//     if else - decode only bits which fits precisely into X bytes (where X = (n*11/8) <-- use integer division), drop remaining (if any)
uint8_t bip39_required_memory_size_decoding(const uint8_t n, const uint8_t extract_padding);

void bip39_base2048_encode(const uint8_t *data_in, const uint8_t n, const uint8_t include_padding, uint16_t *data_out);

void bip39_base2048_decode(const uint16_t *data_in, const uint8_t n, const uint8_t extract_padding, uint8_t *data_out);

const char *bip39_get_word(const uint16_t index);

uint16_t bip39_get_index(const char *word);

uint16_t new_bip39_get_index(const char *word);

uint16_t new_bip39_hint_index(const char *word);

uint16_t bip39_hint_index(const char *word);

uint16_t new_bip39_hint_index_n(const char *word, uint8_t *n);

uint8_t bip39_parse_string_into_words(const char *str, uint16_t *words);

uint8_t bip39_words_to_string(const uint16_t *words, const uint8_t words_count, char *str);

uint8_t bip39_validate_checksum(const uint16_t *words, const uint8_t words_count);

#endif
