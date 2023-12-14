#include <FreeRTOS.h>

#include "../../config.h"
#include "../../display.h"
#include "../../buttons.h"

#include "ui_pin_screen.h"
#include "ui_select_from_options_screen.h"
#include "../ui_common.h"
#include "../components/ui_button.h"

typedef struct{
	ui_button_t keys[10];
	//todo comment the usecase
	uint8_t assigned_values[10];
} ui_pin_key_pad_t;

//todo fix me
static uint8_t pseudo_random(){

	static uint8_t i = 0;
	i = ((++i) > 9) ? 0 : i;

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
		case 9: return 5;
		default: break;
	}

	return 0;
}

//todo fix me
static uint8_t pseudo_random2(){

	static uint8_t i = 0;
	i = ((++i) > 9) ? 0 : i;

	switch(i){
		case 0: return 7;
		case 1: return 1;
		case 2: return 3;
		case 3: return 9;
		case 4: return 2;
		case 5: return 8;
		case 6: return 5;
		case 7: return 4;
		case 8: return 0;
		case 9: return 6;
		default: break;
	}

	return 0;
}

//todo comment usecase
static inline uint8_t get_x_for_key(const uint8_t idx){
	if(idx == 9) return 44 + 1 * 13;
	return 44 + (idx % 3) * 13;
}

//todo comment usecase
static inline uint8_t get_y_for_key(const uint8_t idx){
	return 9 + (idx / 3) * 13;
}

static void init_pin_key_pad(ui_pin_key_pad_t *pin_key_pad){
	//todo fix me...
	for(uint8_t i=0; i<10; ++i){
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

static uint8_t retrieve_pin_length_from_user(){
	uint8_t pin_length_options_count = PIN_CODE_MAX_LENGTH - PIN_CODE_MIN_LENGTH + 1;
	uint8_t pin_length_options[pin_length_options_count];

	for(uint8_t i=0; i < pin_length_options_count; ++i){
		pin_length_options[i] = PIN_CODE_MIN_LENGTH + i;
	}

	ui_uint8_t_options_t options_struct = {
		.count_of_options = pin_length_options_count,
		.options = pin_length_options
	};

	//temporary saved retrieved index into this variable
	uint8_t selected_pin_length = ui_select_from_options_screen(
		UI_OPTION_TYPE_UINT8_T, (void *)(&options_struct),
		"Select pin code\nlength:"
	);

	if(selected_pin_length == UI_SELECT_FROM_OPTIONS_SCREEN_FAILURE_RETURN) return 0;

	//fix variable value to fit its name
	selected_pin_length = pin_length_options[selected_pin_length];

	return selected_pin_length;
}

static uint8_t retrieve_pin_from_user(const uint8_t selected_pin_length, uint8_t *retrieved_pin){

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
			active_idx = ((++active_idx) > 9) ? 0 : active_idx;
			ui_button_set_active_state(&(pin_key_pad.keys[active_idx]), 1);
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&left_btn, 0);

		}else{
			ui_button_set_active_state(&right_btn, 1);
			retrieved_pin[current_pin_idx] = pin_key_pad.assigned_values[active_idx];
			char text[2] = {'0' + pin_key_pad.assigned_values[active_idx], 0x00};
			display_puts(40 + current_pin_idx * 8, 0, text);
			++current_pin_idx;
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&right_btn, 0);
			if(current_pin_idx > (selected_pin_length - 1)){
				break;
			}
		}

	}

	return selected_pin_length;
}

uint8_t compare_pin_codes(const pin_code_t pin_code, const pin_code_t pin_code_again){

	if(pin_code.length != pin_code_again.length) return 0;

	for(uint8_t i = 0; i < pin_code.length; ++i){
		if(pin_code.code[i] != pin_code_again.code[i]) return 0;
	}

	return 1;
}

pin_code_t ui_enter_pin_screen(){

	pin_code_t pin_code = {
		.length = 0
	};

	pin_code.length = retrieve_pin_length_from_user();
	if(!(pin_code.length)) return pin_code;

	retrieve_pin_from_user(pin_code.length, pin_code.code);
	return pin_code;
}

pin_code_t ui_setup_pin_screen(){

	pin_code_t pin_code = {
		.length = 0
	};

	pin_code.length = retrieve_pin_length_from_user();
	if(!(pin_code.length)) return pin_code;	

	pin_code_t pin_code_again = {
		.length = pin_code.length
	};

	while(1){

		retrieve_pin_from_user(pin_code.length, pin_code.code);

		display_clear();

		display_puts(0, 0, "Enter pin once\nagain\n\nPress any key\nto continue");
		display_buffer_display();

		ui_wait_for_any_button_press();
		ui_wait_until_all_buttons_are_released();

		retrieve_pin_from_user(pin_code_again.length, pin_code_again.code);

		if(compare_pin_codes(pin_code, pin_code_again)) return pin_code;

		display_clear();

		display_puts(0, 0, "Entered pins\ndo not match!\n\nTry again!\nPress any key\nto continue");
		display_buffer_display();

		ui_wait_for_any_button_press();
		ui_wait_until_all_buttons_are_released();

	}

}

