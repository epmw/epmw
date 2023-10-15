#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

//use last page in flash to store data
#define FLASH_STORAGE_ADDRESS ((uint32_t)0x08010000)

void flash_save_data(const uint8_t *data, const uint8_t data_size);

void flash_load_data(uint8_t *data, const uint8_t data_size);

#endif
