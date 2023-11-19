#include <FreeRTOS.h>

#include "../buttons.h"
#include "../display.h"

#include "ui_common.h"

void ui_uint32_to_str(uint32_t x, char *str){
	if(!x){
		*(str++) = '0';
		*str = 0x00;
		return;
	}
	char buffer[16];
	uint8_t index = 0;
	while(x){
		buffer[index++] = '0' + (x % 10);
		x /= 10;
	}
	while(index--){
		*(str++) = buffer[index];
	}
}

void ui_wait_for_any_button_press(){
	while((!button_get_state(LEFT_BUTTON)) && (!button_get_state(RIGHT_BUTTON))){
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void ui_wait_until_all_buttons_are_released(){
	while((button_get_state(LEFT_BUTTON)) || (button_get_state(RIGHT_BUTTON))){
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

uint8_t ui_wait_and_get_pressed_button(){
	uint8_t left_button_state = 0;
	uint8_t right_button_state = 0;
	while(
		(left_button_state = button_get_state(LEFT_BUTTON)) == 0 &&
		(right_button_state = button_get_state(RIGHT_BUTTON)) == 0
	){
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	return left_button_state ? LEFT_BUTTON : RIGHT_BUTTON;
}

//todo define also starting pixel location (x,y)
//todo check also whether provided icon has correct size - add size parameter!!!
void ui_print_icon_to_display(const uint8_t width, const uint8_t height, const uint8_t *icon){

	uint16_t index = 0;
	uint8_t current_bit = 0;

	for(uint8_t i=0; i<height; ++i){

		for(uint8_t j=0; j<width; ++j){

			display_set_pixel(j, i, icon[index] & (1 << (7 - (current_bit++))));

			if(current_bit >= 8){
				current_bit = 0;
				++index;
			}
		}
	}
}
