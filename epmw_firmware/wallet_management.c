#include "wallet_management.h"
#include "flash_storage.h"

static uint8_t wallet_manager_initialized = 0;
static wallet_data_t wallet_data;

static inline void wallet_management_load_from_flash(){
	flash_load_data((uint8_t *)&wallet_data, sizeof(wallet_data));	
}

static inline void wallet_management_save_to_flash(){
	flash_save_data((uint8_t *)&wallet_data, sizeof(wallet_data));
}

static void wallet_set_initialized_state(const uint8_t state){
	wallet_data.initialized = state ? WALLET_INITIALIZED_MAGIC : WALLET_NOT_INITIALIZED_MAGIC;
	wallet_management_save_to_flash();
}

void wallet_management_init(){
	//todo - separate wallet data to init flag and private key parts
	//load private key into SRAM only when needed
	wallet_management_load_from_flash();
	wallet_manager_initialized = 1;
}

uint8_t wallet_is_initialized(){
	wallet_management_load_from_flash();
	return (wallet_data.initialized == WALLET_INITIALIZED_MAGIC) ? 1 : 0;
}

void wallet_set_as_initialized(){
	wallet_set_initialized_state(1);
}

void wallet_management_set_mnemonic_seed(const uint8_t wc, const uint16_t *words){
	if(wc % 3) return; //invalid words count
	wallet_data.wc = wc;
	for(uint8_t i=0; i<wc; ++i){
		wallet_data.words[i] = words[i];
	}
	wallet_management_save_to_flash();
}

uint8_t wallet_management_get_mnemonic_seed(uint16_t **words){
	if(!wallet_is_initialized()) return 0;
	*words = wallet_data.words;
	return wallet_data.wc;
}

uint8_t wallet_management_check_pin_code(const pin_code_t pin_code){
	return compare_pin_codes(wallet_data.pin_code, pin_code);
}

void wallet_management_set_pin_code(const pin_code_t pin_code){
	wallet_data.pin_code = pin_code;
	wallet_data.pin_failed_attempts = 0;
	wallet_management_save_to_flash();
}

uint8_t wallet_management_get_pin_failed_attempts(){
	return wallet_data.pin_failed_attempts;
}

void wallet_management_set_pin_failed_attempts(const uint8_t pin_failed_attempts){
	wallet_data.pin_failed_attempts = pin_failed_attempts;
	wallet_management_save_to_flash();
}

void wallet_delete(){
	for(uint8_t i = 0; i < MNEMONIC_MAX_LENGTH; ++i){
		wallet_data.words[i] = 0;
	}
	wallet_data.wc = 0;
	wallet_data.pin_code.length = 0;
	for(uint8_t i = 0; i < PIN_CODE_MAX_LENGTH; ++i){
		wallet_data.pin_code.code[i] = 0;
	}
	wallet_set_initialized_state(0);
}
