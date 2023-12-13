#include <FreeRTOS.h>

#include "../../config.h"
#include "../../display.h"
#include "../../buttons.h"

#include "ui_select_from_options_screen.h"
#include "../ui_common.h"
#include "../components/ui_button.h"

//returns count of options or zero
static uint8_t check_options_correctness(const ui_option_type_t options_type, const void *options_struct){
	
	if(options_type != UI_OPTION_TYPE_STRING && options_type != UI_OPTION_TYPE_UINT8_T) return 0;

	uint8_t options_count;

	switch(options_type){

		case UI_OPTION_TYPE_STRING:
			options_count = ((ui_string_options_t *)options_struct)->count_of_options;
			break;

		case UI_OPTION_TYPE_UINT8_T:
			options_count = ((ui_uint8_t_options_t *)options_struct)->count_of_options;
			break;

		default: break;
	}

	return options_count;
}

static void display_options_value(
	const ui_option_type_t options_type, const void *options_struct, const uint8_t option_idx,
	const uint8_t option_start_x, const uint8_t option_start_y
){

	uint8_t options_count = check_options_correctness(options_type, options_struct);
	
	if(!options_count) return;
	if(option_idx >= options_count) return;

	display_puts(option_start_x, option_start_y, "               ");

	switch(options_type){

		case UI_OPTION_TYPE_STRING:

			display_puts(
				option_start_x,
				option_start_y,
				((ui_string_options_t *)options_struct)->options[option_idx]
			);
			break;

		case UI_OPTION_TYPE_UINT8_T:
			{
				char tmp_bfr[4] = {0};
				ui_uint32_to_str(
					((ui_uint8_t_options_t *)options_struct)->options[option_idx],
					tmp_bfr
				);
				display_puts(option_start_x, option_start_y, tmp_bfr);
			}
			break;

		default: break;
	}

}

//todo comment for usage and screen_text requirements
uint8_t ui_select_from_options_screen(const ui_option_type_t options_type, const void *options_struct, const char *screen_text){

	uint8_t options_count = check_options_correctness(options_type, options_struct);
	if(!options_count) return UI_SELECT_FROM_OPTIONS_SCREEN_FAILURE_RETURN;
	
	uint8_t screen_text_lines_count = ui_count_lines_in_string(screen_text);

	const uint8_t option_start_x = 0 + (ui_get_length_of_line_in_string(screen_text, screen_text_lines_count - 1)) * 8 + 8;
	const uint8_t option_start_y = 0 + (screen_text_lines_count - 1) * 8;

	ui_button_t left_btn, right_btn;

	uint8_t current_option_idx = 0;

	while(1){

		display_clear();
	
		display_puts(0, 0, screen_text);

		ui_button_init_button(&left_btn, "<>", 10, 49);
		ui_button_set_border_on(&left_btn, 1);
		ui_button_init_button(&right_btn, "OK", 90, 49);
		ui_button_set_border_on(&right_btn, 1);

		while(1){

			display_options_value(
				options_type, options_struct, current_option_idx,
				option_start_x, option_start_y
			);

			display_buffer_display();

			if(ui_wait_and_get_pressed_button() == LEFT_BUTTON){
				ui_button_set_active_state(&left_btn, 1);
				current_option_idx = ((++current_option_idx) >= options_count) ? 0 : current_option_idx;
				ui_wait_until_all_buttons_are_released();
				ui_button_set_active_state(&left_btn, 0);
			}else{
				ui_button_set_active_state(&right_btn, 1);
				ui_wait_until_all_buttons_are_released();
				ui_button_set_active_state(&right_btn, 0);
				break;
			}

		}

		display_clear();
		display_puts(0, 0, "Selected value:\n");
		display_options_value(options_type, options_struct, current_option_idx, 0, 8);
		ui_button_init_button(&left_btn, "<-", 10, 49);
		ui_button_set_border_on(&left_btn, 1);
		ui_button_init_button(&right_btn, "OK", 90, 49);
		ui_button_set_border_on(&right_btn, 1);

		if(ui_wait_and_get_pressed_button() == LEFT_BUTTON){
			ui_button_set_active_state(&left_btn, 1);
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&left_btn, 0);
		}else{
			ui_button_set_active_state(&right_btn, 1);
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&right_btn, 0);
			break;
		}

	}

	return current_option_idx;	

}
