#ifndef UI_PIN_SCREEN
#define UI_PIN_SCREEN

#include <stdint.h>
#include "../../config.h"

typedef struct{
	uint8_t length;
	uint8_t code[PIN_CODE_MAX_LENGTH];
} pin_code_t;

uint8_t compare_pin_codes(const pin_code_t pin_code, const pin_code_t pin_code_again);

pin_code_t ui_enter_pin_screen();

pin_code_t ui_setup_pin_screen();

#endif
