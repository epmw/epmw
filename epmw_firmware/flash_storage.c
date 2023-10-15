#include <libopencm3/stm32/flash.h>

#include "flash_storage.h"

void flash_save_data(const uint8_t *data, const uint8_t data_size) {
	flash_unlock();
	flash_erase_page(FLASH_STORAGE_ADDRESS);
	//this don't work - for F1 unfortunately
	// for(uint8_t i=0; i<data_size; ++i){
	// 	flash_program_byte(FLASH_STORAGE_ADDRESS + i, *(data++));
	// }
	//so this is the temporary solution
	for(uint8_t i=0; i<data_size; i += 4){
		uint32_t tmp = *((uint32_t *)(data+i));
		flash_program_word(FLASH_STORAGE_ADDRESS + i, tmp);
	}
	flash_lock();
}

void flash_load_data(uint8_t *data, const uint8_t data_size) {
	const uint8_t *flash_bytes = (const uint8_t *)FLASH_STORAGE_ADDRESS;
	//this don't work - for F1 unfortunately
	// for(uint8_t i=0; i<data_size; ++i){
	// 	*(data++) = *(flash_bytes++);
	// }
	//so this is the temporary solution
	for(uint8_t i=0; i<data_size; i += 4){
		data[i] = flash_bytes[i];
		data[i+1] = flash_bytes[i+1];
		data[i+2] = flash_bytes[i+2];
		data[i+3] = flash_bytes[i+3];
	}		
}
