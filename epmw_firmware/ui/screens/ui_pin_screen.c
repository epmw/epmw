#include <FreeRTOS.h>

#include "../../config.h"
#include "../../display.h"
#include "../../buttons.h"

#include "ui_pin_screen.h"
#include "../ui_common.h"
#include "../components/ui_button.h"

typedef struct{
	ui_button_t keys[9];
	//todo comment the usecase
	uint8_t assigned_values[9];
} ui_pin_key_pad_t;

//todo fix me
static uint8_t pseudo_random(){

	static uint8_t i = 0;
	i = ((++i) > 8) ? 0 : i;

	switch(i){
		case 0: return 9;
		case 1: return 0;
		case 2: return 7;
		case 3: return 3;
		case 4: return 1;
		case 5: return 2;
		case 6: return 6;
		case 7: return 4;
		case 8: return 8;
		default: break;
	}

	return 0;
}

//todo comment usecase
static inline uint8_t get_x_for_key(const uint8_t idx){
	return 44 + (idx % 3) * 13;
}

//todo comment usecase
static inline uint8_t get_y_for_key(const uint8_t idx){
	return 9 + (idx / 3) * 13;
}

static void init_pin_key_pad(ui_pin_key_pad_t *pin_key_pad){
	//todo fix me...
	for(uint8_t i=0; i<9; ++i){
		pin_key_pad->assigned_values[i] = pseudo_random();
		char text[2] = {'0' + pin_key_pad->assigned_values[i], 0x00};
		ui_button_init_button(
			&(pin_key_pad->keys[i]),
			text,
			get_x_for_key(i),
			get_y_for_key(i)
		);
		ui_button_set_border_on(&(pin_key_pad->keys[i]), 1);
	}
	ui_button_set_active_state(&(pin_key_pad->keys[0]), 1);
}

void ui_enter_pin_screen(){
	display_clear();
	ui_pin_key_pad_t pin_key_pad;
	display_puts(0, 0, "Pin: ");
	init_pin_key_pad(&pin_key_pad);

	ui_button_t left_btn, right_btn;
	ui_button_init_button(&left_btn, "<>", 10, 49);
	ui_button_set_border_on(&left_btn, 1);
	ui_button_init_button(&right_btn, "OK", 90, 49);
	ui_button_set_border_on(&right_btn, 1);

	uint8_t active_idx = 0;
	uint8_t current_pin_idx = 0;

	while(1){
		if(ui_wait_and_get_pressed_button() == LEFT_BUTTON){
			ui_button_set_active_state(&left_btn, 1);
			ui_button_set_active_state(&(pin_key_pad.keys[active_idx]), 0);
			active_idx = ((++active_idx) > 8) ? 0 : active_idx;
			ui_button_set_active_state(&(pin_key_pad.keys[active_idx]), 1);
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&left_btn, 0);
		}else{
			ui_button_set_active_state(&right_btn, 1);
			//todo save entered digit
			char text[2] = {'0' + pin_key_pad.assigned_values[active_idx], 0x00};
			display_puts(40 + current_pin_idx * 8, 0, text);
			++current_pin_idx;
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&right_btn, 0);
			if(current_pin_idx > (PIN_CODE_MAX_LENGTH-1)){
				break;
			}
		}
	}
}

void ui_setup_pin_screen(){

}

