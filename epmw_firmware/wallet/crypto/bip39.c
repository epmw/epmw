/* +----------------------------------+ */
/* |              BIP39               | */
/* |             bip39.c              | */
/* |   (c)copyright nitram147 2021    | */
/* +----------------------------------+ */

#include <stddef.h> //required for NULL definition

#include "bip39.h"
#include "bip39_wordlist.h"

//todo hint add no of hint 0, 1, 2 (returns first, second, third hinted word with corresponding prefix)

//todo rename required memory size encoding to base2048_encoding....

//todo decide how to handle more than 185 words during decoding, either change back memcalcfunction to uint16_t
//    or introduce limit for decoding function
//    also protect memcalcfunctdecoding from >185 value of n argument!!!


// Calculate required memory size (count of words) for encoding of n-bytes
// n - count of bytes to encode
// include_padding:
//     if true - encode all n-bytes, in case that padding is needed (((n*8)%11) != 0) include this fact in the calculation 
//     if else - encode only bits which fits precisely into X words (where X = (n*8/11) <-- use integer division), drop remaining (if any)
uint8_t bip39_required_memory_size_encoding(const uint8_t n, const uint8_t include_padding){
	if(!((n*8)%11)) return (n*8/11);
	else return ((n*8/11)+((include_padding) ? 1 : 0));
}

// Calculate required memory size (count of bytes) for decoding of n-words
// n - count of words to decode from
// extract_padding:
//     if true - decode all n-words, in case that padding is needed (((n*11)%8) != 0) include this fact in the calculation 
//     if else - decode only bits which fits precisely into X bytes (where X = (n*11/8) <-- use integer division), drop remaining (if any)
uint8_t bip39_required_memory_size_decoding(const uint8_t n, const uint8_t extract_padding){
	if(!((n*11)%8)) return (n*11/8);
	else return ((n*11/8)+((extract_padding) ? 1 : 0));
}

//todo make comments for encode/decode functions !!!
void bip39_base2048_encode(const uint8_t *data_in, const uint8_t n, const uint8_t include_padding, uint16_t *data_out){
	uint16_t word = 0;
	uint8_t word_index = 0;
	uint8_t loaded_bits = 0;
	uint8_t actual_byte;

	for(uint8_t i=0; i<n; ++i){
		actual_byte = data_in[i];
		for(uint8_t bi=7; bi>=0; --bi){
			word = (word << 1) | ((actual_byte >> bi) & 1);
			++loaded_bits;
			if(loaded_bits == 11){
				data_out[word_index++] = word;
				word = 0;
				loaded_bits = 0;
			}
			if(!bi) break; //uint so we can't count to negative
		}
	}

	if(include_padding && loaded_bits){
		while((loaded_bits++) != 11) word = (word << 1);
		data_out[word_index++] = word;
	}
}

void bip39_base2048_decode(const uint16_t *data_in, const uint8_t n, const uint8_t extract_padding, uint8_t *data_out){
	uint8_t byte = 0;
	uint8_t byte_index = 0;
	uint8_t loaded_bits = 0;
	uint16_t actual_word;

	for(uint8_t i=0; i<n; ++i){
		actual_word = data_in[i];
		for(uint8_t wi=10; wi>=0; --wi){
			byte = (byte << 1) | ((actual_word >> wi) & 1);
			++loaded_bits;
			if(loaded_bits == 8){
				data_out[byte_index++] = byte;
				byte = 0;
				loaded_bits = 0;
			}
			if(!wi) break; //uint so we can't count to negative
		}
	}

	if(extract_padding && loaded_bits){
		data_out[byte_index] = (byte << (8 - loaded_bits));
	}
}

//TODO FIX ME ...
//static uint8_t bip39_strlen(const char *str){
uint8_t bip39_strlen(const char *str){
	uint8_t len=0;
	while(*(str++)) ++len;
	return len;
}

static uint8_t bip39_strequal(const char *str1, const char *str2){
	while(*str1 == *str2 && (*str1)){
		++str1;
		++str2;
	}
	return (*str1 == *str2);
}

static uint8_t bip39_strnequal(const char *str1, const char *str2, const uint8_t n){
	uint8_t i=0;
	while(*str1 == *str2 && (*str1) && (i++)<n){
		if(i+1<=n){
			++str1;
			++str2;
		}
	}
	return (*str1 == *str2);
}

uint8_t bip39_strcmp(const char *str1, const char *str2){
	for(uint8_t i=0;;++i){
		if(*str1  == *str2){
			if(!(*str1)) return BIP39_STRCMP_EQ;
		}else{
			return (*str1 < *str2) ? BIP39_STRCMP_LT : BIP39_STRCMP_GT;	
		}
		++str1; ++str2;
	}
	return BIP39_STRCMP_EQ; //this should be never reached
}

static uint8_t bip39_strncmp(const char *str1, const char *str2, const uint8_t n){
	for(uint8_t i=0; i<n; ++i){
		if(*str1  == *str2){
			if(!(*str1)) return BIP39_STRCMP_EQ;
		}else{
			return (*str1 < *str2) ? BIP39_STRCMP_LT : BIP39_STRCMP_GT;	
		}
		++str1; ++str2;
	}
	return BIP39_STRCMP_EQ;
}

const char *bip39_get_word(const uint16_t index){
	if(index >= BIP_39_WORDS_COUNT) return NULL;
	return bip39_wordlist[index];
}

uint16_t bip39_get_index(const char *word){
	for(uint16_t i=0; i<BIP_39_WORDS_COUNT; ++i){
		if(bip39_strcmp(word, bip39_get_word(i)) == BIP39_STRCMP_EQ) return i;
	}
	return BIP39_INVALID_WORD_INDEX;
}

uint16_t new_bip39_get_index(const char *word){
	char c = *word;

	if(c < 'a' || c > 'z') return BIP39_INVALID_WORD_INDEX;

	c -= 'a';
	uint16_t left = bip39_wordlist_first_char_index_map[c], right = (bip39_wordlist_first_char_index_map[c+1] - 1);

	uint16_t middle;
	uint8_t cmp_res;
	while(left <= right && right != 0xffff){
		middle = ((right - left) / 2) + left;
		cmp_res = bip39_strcmp(word, bip39_get_word(middle));
		switch(cmp_res){
			case BIP39_STRCMP_EQ:
				return middle;
				break;
			case BIP39_STRCMP_LT:
				right = middle - 1;
				break;
			case BIP39_STRCMP_GT:
				left = middle + 1;
				break;
			default:
				break;
		}
	}
	return BIP39_INVALID_WORD_INDEX;
}

uint16_t new_bip39_hint_index(const char *word){
	char c = *word;
	
	if(c < 'a' || c > 'z') return BIP39_INVALID_WORD_INDEX;
	
	c -= 'a';
	uint16_t left = bip39_wordlist_first_char_index_map[c], right = (bip39_wordlist_first_char_index_map[c+1] - 1);

	uint16_t middle;
	uint8_t cmp_res;
	while(left <= right){
		middle = ((right - left) / 2) + left;
		cmp_res = bip39_strncmp(word, bip39_get_word(middle), bip39_strlen(word));
		switch(cmp_res){
			case BIP39_STRCMP_EQ:
				return middle;
				break;
			case BIP39_STRCMP_LT:
				right = middle - 1;
				break;
			case BIP39_STRCMP_GT:
				left = middle + 1;
				break;
			default:
				break;
		}
	}
	return BIP39_INVALID_WORD_INDEX;
}

//---------------------

uint16_t bip39_hint_index(const char *word){
	for(uint16_t i=0; i<BIP_39_WORDS_COUNT; ++i){
		if(bip39_strncmp(word, bip39_get_word(i), bip39_strlen(word)) == BIP39_STRCMP_EQ) return i;
	}
	return BIP39_INVALID_WORD_INDEX;
}

uint16_t new_bip39_hint_index_n(const char *word, uint8_t *n){
	uint8_t len = bip39_strlen(word);
	uint8_t dummy_n; if(n == NULL) n = &dummy_n;
	*n = 0;
	if(!len) return BIP39_INVALID_WORD_INDEX;

	char c = *word;	
	if(c < 'a' || c > 'z') return BIP39_INVALID_WORD_INDEX;

	c -= 'a';
	uint16_t left = bip39_wordlist_first_char_index_map[c], right = (bip39_wordlist_first_char_index_map[c+1] - 1);

	if(bip39_get_word(left)[0] != word[0]) return BIP39_INVALID_WORD_INDEX;
	
	if(len == 1){
		*n = (right-left+1);
		return left;
	}

	while(left != BIP_39_WORDS_COUNT && bip39_strncmp(word, bip39_get_word(left), len) != BIP39_STRCMP_EQ){
		++left;
	}

	if(left != BIP_39_WORDS_COUNT && bip39_strncmp(word, bip39_get_word(left), len) == BIP39_STRCMP_EQ){

		*n = 1;

		for(uint16_t nl = left+1; nl != BIP_39_WORDS_COUNT; ++nl){
			if(bip39_strncmp(word, bip39_get_word(nl), len) != BIP39_STRCMP_EQ) break;
			*n += 1;
		}

		return left;
	}

	return BIP39_INVALID_WORD_INDEX;
}

uint8_t bip39_parse_string_into_words(const char *str, uint16_t *words){
	char word[BIP39_LONGEST_WORD_CHARS_COUNT+1] = {0};
	uint8_t wci=0, wi=0;
	while(*str != '\0'){
		if(*str == ' ' || *str == '\t'){
			++str;
			if(wci){
				word[wci] = '\0';
				words[wi] = new_bip39_get_index(word);
				if(words[wi] == BIP39_INVALID_WORD_INDEX) return 0;
				++wi;
			}
			wci = 0;
			continue;
		}
		word[wci++] = *str;
		if(wci > BIP39_LONGEST_WORD_CHARS_COUNT) return 0;
		++str;
	}
	if(wci){
		word[wci] = '\0';
		words[wi] = new_bip39_get_index(word);
		if(words[wi] == BIP39_INVALID_WORD_INDEX) return 0;
		++wi;
	}
	return wi;
}

uint8_t bip39_words_to_string(const uint16_t *words, const uint8_t words_count, char *str){
	const char *wrd;
	for(uint8_t i=0; i<words_count; ++i){
		wrd = bip39_get_word(words[i]);
		if(wrd == NULL) return 0;
		while(*wrd != '\0'){
			*str = *wrd;
			++str; ++wrd;
		}
		*str = ' '; ++str;
	}
	if(words_count) --str;
	*str = '\0';
	return 1;
}

//todo implement hash function
#include "./external_libs/sha256.h"

void hash_data(const uint8_t *data, const uint8_t bytes_count, uint8_t *hash){
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, data, bytes_count);
	sha256_final(&ctx, hash);
	return;
}


uint8_t bip39_validate_checksum(const uint16_t *words, const uint8_t words_count){
	//standard contains examples from 12 up to 24 words, see https://github.com/bitcoin/bips/blob/master/bip-0039.mediawiki
	//however generators such as https://iancoleman.io/bip39/ generate from 3 up to 24 words
	//so we will allow also 3, 6 and 9 words variant
	if(words_count < 3 || words_count > 24 || (words_count % 3) != 0) return 0;
	uint8_t data[33] = {0};
	uint8_t hash[32] = {0};
	uint8_t checksum;
	bip39_base2048_decode(words, words_count, 1, data);
	hash_data(data, (bip39_required_memory_size_decoding(words_count, 1)-1), hash);
	//todo explain this bit operations magic
	checksum = hash[0] & (0x0100 - (1 << (8 - (words_count/3))));
	return (data[bip39_required_memory_size_decoding(words_count, 1)-1] == checksum);
}

//todo implement header file and finalize api
