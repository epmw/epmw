#include <FreeRTOS.h>

#include "string.h"

#include "../../config.h"
#include "../../display.h"
#include "../../buttons.h"

#include "ui_seed_enter_screen.h"
#include "ui_select_from_options_screen.h"
#include "../ui_common.h"
#include "../components/ui_button.h"

#define UI_SEED_KEYBOARD_KEYS_COUNT 10
#define UI_SEED_WORD_MAX_LENGTH 8
#define UI_SEED_TIME_TO_NEXT_CHAR_MS 700

typedef struct{
	ui_button_t keys[UI_SEED_KEYBOARD_KEYS_COUNT];
	//todo comment the usecase
	char assigned_chars[UI_SEED_KEYBOARD_KEYS_COUNT][3];
	//todo comment this "bitfield"
	uint32_t chars_enabled;
} ui_seed_keyboard_t;

//todo comment usecase
static inline uint8_t get_x_for_key(const uint8_t idx){
	if(idx == 9) return 18 + 1 * 29;
	return 18 + (idx % 3) * 29;
}

//todo comment usecase
static inline uint8_t get_y_for_key(const uint8_t idx){
	return 9 + (idx / 3) * 13;
}

static void init_seed_keyboard(ui_seed_keyboard_t *seed_keyboard){

	memset(seed_keyboard, 0, sizeof(ui_seed_keyboard_t));

	const uint8_t alphabet_size = 'z' - 'a' + 1;

	for(uint8_t i = 0; i < ((UI_SEED_KEYBOARD_KEYS_COUNT-1)*3); ++i){
		if(i < alphabet_size){
			seed_keyboard->assigned_chars[i/3][i%3] = 'a' + i;
			seed_keyboard->chars_enabled |= (1 << i);
		}else{
			seed_keyboard->assigned_chars[i/3][i%3] = ' ';
		}
	}

	seed_keyboard->assigned_chars[UI_SEED_KEYBOARD_KEYS_COUNT-1][0] = '<';
	seed_keyboard->assigned_chars[UI_SEED_KEYBOARD_KEYS_COUNT-1][1] = '-';
	seed_keyboard->assigned_chars[UI_SEED_KEYBOARD_KEYS_COUNT-1][2] = '-';

	for(uint8_t i=0; i<UI_SEED_KEYBOARD_KEYS_COUNT; ++i){

		char text[4] = {
			seed_keyboard->assigned_chars[i][0],
			seed_keyboard->assigned_chars[i][1],
			seed_keyboard->assigned_chars[i][2],
			0x00
		};

		ui_button_init_button(
			&(seed_keyboard->keys[i]),
			text,
			get_x_for_key(i),
			get_y_for_key(i)
		);

		ui_button_set_border_on(&(seed_keyboard->keys[i]), 1);

	}

	ui_button_set_active_state(&(seed_keyboard->keys[0]), 1);

}

//todo remove all magic constants !!!
uint16_t retrieve_seed_word_from_user(){

	display_clear();

	ui_seed_keyboard_t seed_keyboard;

	display_puts(0, 0, "Wrd(1):");
	init_seed_keyboard(&seed_keyboard);

	ui_button_t left_btn, right_btn;
	ui_button_init_button(&left_btn, "<>", 10, 49);
	ui_button_set_border_on(&left_btn, 1);
	ui_button_init_button(&right_btn, "OK", 90, 49);
	ui_button_set_border_on(&right_btn, 1);

	uint8_t active_idx = 0;
	uint8_t current_word_char_idx = 0;
	uint8_t idx_within_key_pad = 0;

	uint8_t tmp_left_btn_pressed = 0;

	while(1){

		if(tmp_left_btn_pressed || ui_wait_and_get_pressed_button() == LEFT_BUTTON){

			tmp_left_btn_pressed = 0;

			ui_button_set_active_state(&left_btn, 1);
			ui_button_set_active_state(&(seed_keyboard.keys[active_idx]), 0);

			active_idx = ((++active_idx) > (UI_SEED_KEYBOARD_KEYS_COUNT - 1)) ? 0 : active_idx;

			ui_button_set_active_state(&(seed_keyboard.keys[active_idx]), 1);
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&left_btn, 0);

		}else{

			ui_button_set_active_state(&right_btn, 1);

			if(active_idx != (UI_SEED_KEYBOARD_KEYS_COUNT-1)){

				char text[2] = {seed_keyboard.assigned_chars[active_idx][idx_within_key_pad++], 0x00};
				display_puts(7*8 + current_word_char_idx * 8, 0, text);
				if(idx_within_key_pad > 2) idx_within_key_pad = 0;

				display_draw_hline(
					7*8 + current_word_char_idx * 8, 7,
					7*8 + current_word_char_idx * 8 + 8, 1
				);

				display_buffer_display();

				ui_wait_until_all_buttons_are_released();
				ui_button_set_active_state(&right_btn, 0);

				while(1){

					uint8_t break_while = 0;
					uint8_t tmp_btn_press = ui_wait_and_get_pressed_button_with_timeout(UI_SEED_TIME_TO_NEXT_CHAR_MS);

					switch(tmp_btn_press){

						//case fallthrough on purpose !!!
						case LEFT_BUTTON:
							tmp_left_btn_pressed = 1;

						case BUTTON_TIMEOUT:
							display_draw_hline(
								7*8 + current_word_char_idx * 8, 7,
								7*8 + current_word_char_idx * 8 + 8, 0
							);
							idx_within_key_pad = (idx_within_key_pad) ? (idx_within_key_pad - 1) : 2;
							text[0] = seed_keyboard.assigned_chars[active_idx][idx_within_key_pad];
							display_puts(7*8 + current_word_char_idx * 8, 0, text);

							display_buffer_display();
							if((++current_word_char_idx) >= UI_SEED_WORD_MAX_LENGTH){
								--current_word_char_idx;
							}
							idx_within_key_pad = 0;
							break_while = 1;
							break;

						default: break;
					}

					if(break_while) break;

					ui_button_set_active_state(&right_btn, 1);
					
					text[0] = seed_keyboard.assigned_chars[active_idx][idx_within_key_pad++];
					display_puts(7*8 + current_word_char_idx * 8, 0, text);
					display_draw_hline(
						7*8 + current_word_char_idx * 8, 7,
						7*8 + current_word_char_idx * 8 + 8, 1
					);
					display_buffer_display();
					
					if(idx_within_key_pad > 2) idx_within_key_pad = 0;

					ui_wait_until_all_buttons_are_released();
					ui_button_set_active_state(&right_btn, 0);						
				}

				continue;

			}else{
				if(current_word_char_idx) --current_word_char_idx;
				display_puts(7*8 + current_word_char_idx * 8, 0, " ");
				ui_wait_until_all_buttons_are_released();
				ui_button_set_active_state(&right_btn, 0);
			}
		}

	}

	return 0;
}
