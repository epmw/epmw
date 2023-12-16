#include <FreeRTOS.h>

#include "string.h"

#include "../../config.h"
#include "../../display.h"
#include "../../buttons.h"

#include "../../wallet/crypto/bip39.h"

#include "ui_seed_enter_screen.h"
#include "ui_select_from_options_screen.h"
#include "../ui_common.h"
#include "../components/ui_button.h"

#define UI_SEED_KEYBOARD_KEYS_COUNT 10
#define UI_SEED_WORD_MAX_LENGTH 8
#define UI_SEED_TIME_TO_NEXT_CHAR_MS 450

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

static void render_seed_keyboard(ui_seed_keyboard_t *seed_keyboard, const uint8_t active_idx){
	for(uint8_t i=0; i<UI_SEED_KEYBOARD_KEYS_COUNT; ++i){

		char text[4] = {
			(seed_keyboard->chars_enabled & ((uint32_t)1 << (i*3+0))) ? seed_keyboard->assigned_chars[i][0] : ' ',
			(seed_keyboard->chars_enabled & ((uint32_t)1 << (i*3+1))) ? seed_keyboard->assigned_chars[i][1] : ' ',
			(seed_keyboard->chars_enabled & ((uint32_t)1 << (i*3+2))) ? seed_keyboard->assigned_chars[i][2] : ' ',
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

	ui_button_set_active_state(&(seed_keyboard->keys[active_idx]), 1);
}

static void init_seed_keyboard(
	ui_seed_keyboard_t *seed_keyboard,
	const retrieve_string_based_info_funct_ptr_t get_enabled_chars
){

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

	seed_keyboard->assigned_chars[UI_SEED_KEYBOARD_KEYS_COUNT-2][2] = '_';

	seed_keyboard->assigned_chars[UI_SEED_KEYBOARD_KEYS_COUNT-1][0] = '<';
	seed_keyboard->assigned_chars[UI_SEED_KEYBOARD_KEYS_COUNT-1][1] = '-';
	seed_keyboard->assigned_chars[UI_SEED_KEYBOARD_KEYS_COUNT-1][2] = '-';

	seed_keyboard->chars_enabled |= (7 << ((UI_SEED_KEYBOARD_KEYS_COUNT-1) * 3));

	if(get_enabled_chars){
		seed_keyboard->chars_enabled = get_enabled_chars("");
	}

	render_seed_keyboard(seed_keyboard, 0);
}

static char get_pressed_char(
	const ui_seed_keyboard_t *seed_keyboard,
	const uint8_t active_idx,
	uint8_t idx_within_key_pad
){
	const uint8_t tmp_en_cnt = (
		((seed_keyboard->chars_enabled & (1 << (active_idx*3+0))) ? 1 : 0) +
		((seed_keyboard->chars_enabled & (1 << (active_idx*3+1))) ? 1 : 0) +
		((seed_keyboard->chars_enabled & (1 << (active_idx*3+2))) ? 1 : 0)
	);
	if(!tmp_en_cnt) return ' ';
	idx_within_key_pad %= tmp_en_cnt;
	idx_within_key_pad += 1;
	for(uint8_t i=0; i<3; ++i){
		if(seed_keyboard->chars_enabled & (1 << (active_idx*3+i))){
			--idx_within_key_pad;
			if(!idx_within_key_pad){
				return seed_keyboard->assigned_chars[active_idx][i];
			}
		}
	}
	//this point should never be reached!!!
	return ' ';
}

//todo comment this ccwci - clamp_current_word_char_idx
static inline uint8_t ccwci(const uint8_t current_word_char_idx){
	return (current_word_char_idx >= UI_SEED_WORD_MAX_LENGTH) ? (current_word_char_idx - 1)  : current_word_char_idx;
}

//todo remove all magic constants !!!
//todo refactor this function for generic keyboard - it's already coded in generic way!!!
static uint16_t retrieve_seed_word_from_user(
	const uint8_t word_no,
	const retrieve_string_based_info_funct_ptr_t get_enabled_chars,
	const  retrieve_string_based_info_funct_ptr_t accept_string
){

	char loaded_text_buffer[UI_SEED_WORD_MAX_LENGTH+1] = {0};
	uint8_t loaded_chars_count = 0;

	display_clear();

	ui_seed_keyboard_t seed_keyboard;

	//reuse already alocated buffer for this purpose
	strcpy(loaded_text_buffer, "Wrd(");
	uint8_t word_no_str_len = ui_uint32_to_str(word_no, loaded_text_buffer + strlen("Wrd("));
	strcpy(loaded_text_buffer + strlen("Wrd(") + word_no_str_len, "):");
	display_puts(0, 0, loaded_text_buffer);

	init_seed_keyboard(&seed_keyboard, get_enabled_chars);

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

				char text[2];

				text[0] = get_pressed_char(&seed_keyboard, active_idx, idx_within_key_pad++);
				text[1] = 0x00;

				if(idx_within_key_pad > 2) idx_within_key_pad = 0;

				if(text[0] != ' '){
					display_puts((6 + word_no_str_len) * 8 + ccwci(current_word_char_idx) * 8, 0, text);
					display_draw_hline(
						(6 + word_no_str_len) * 8 + ccwci(current_word_char_idx) * 8, 7,
						(6 + word_no_str_len) * 8 + ccwci(current_word_char_idx) * 8 + 8, 1
					);
				}

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
								(6 + word_no_str_len) * 8 + ccwci(current_word_char_idx) * 8, 7,
								(6 + word_no_str_len) * 8 + ccwci(current_word_char_idx) * 8 + 8, 0
							);
							idx_within_key_pad = (idx_within_key_pad) ? (idx_within_key_pad - 1) : 2;
							text[0] = get_pressed_char(&seed_keyboard, active_idx, idx_within_key_pad);

							if(text[0] != ' '){
								
								loaded_text_buffer[loaded_chars_count++] = text[0];
								loaded_text_buffer[loaded_chars_count] = 0x00;
								
								if(text[0] != '_'){								
									display_puts((6 + word_no_str_len) * 8 + ccwci(current_word_char_idx) * 8, 0, text);
									display_buffer_display();
								}	

								if((++current_word_char_idx) > UI_SEED_WORD_MAX_LENGTH){
									--current_word_char_idx;
								}

								if(get_enabled_chars){
									seed_keyboard.chars_enabled = get_enabled_chars(loaded_text_buffer);
									render_seed_keyboard(&seed_keyboard, active_idx);
								}

								if(accept_string){
									if(accept_string(loaded_text_buffer)){
										if(text[0] == '_'){
											loaded_text_buffer[loaded_chars_count-1] = 0x00;
											return new_bip39_get_index(loaded_text_buffer);
										}else{
											return new_bip39_hint_index(loaded_text_buffer);
										}
									}
								}

							}

							idx_within_key_pad = 0;
							break_while = 1;
							break;

						default: break;
					}

					if(break_while) break;

					ui_button_set_active_state(&right_btn, 1);
					
					text[0] = get_pressed_char(&seed_keyboard, active_idx, idx_within_key_pad++);
					
					if(text[0] != ' '){
						display_puts((6 + word_no_str_len) * 8 + ccwci(current_word_char_idx) * 8, 0, text);
						display_draw_hline(
							(6 + word_no_str_len) * 8 + ccwci(current_word_char_idx) * 8, 7,
							(6 + word_no_str_len) * 8 + ccwci(current_word_char_idx) * 8 + 8, 1
						);
					}

					display_buffer_display();
					
					if(idx_within_key_pad > 2) idx_within_key_pad = 0;

					ui_wait_until_all_buttons_are_released();
					ui_button_set_active_state(&right_btn, 0);						
				}

				continue;

			//last key selected ("backspace")
			}else{
				if(loaded_chars_count){
					--loaded_chars_count;
					loaded_text_buffer[loaded_chars_count] = 0x00;
				}
				if(current_word_char_idx) --current_word_char_idx;
				display_puts((6 + word_no_str_len) * 8 + current_word_char_idx * 8, 0, " ");
				if(get_enabled_chars){
					seed_keyboard.chars_enabled = get_enabled_chars(loaded_text_buffer);
					render_seed_keyboard(&seed_keyboard, active_idx);
				}
				ui_wait_until_all_buttons_are_released();
				ui_button_set_active_state(&right_btn, 0);
			}
		}

	}

	return 0;
}

static uint32_t get_next_possible_bip39_chars(const char *str){

	uint8_t tmp_len = strlen(str);

	//return whole alphabet without x, since there is not any word starting
	//with x in the alphabet
	if(!tmp_len){
		return 0b00111011011111111111111111111111 & (~((uint32_t)1 << ('x' - 'a')));
	}

	uint32_t result = 0b00111000000000000000000000000000;
	uint8_t n;
	uint16_t wi;

	if((wi = new_bip39_hint_index_n(str, &n)) == BIP39_INVALID_WORD_INDEX) return 0;

	for(uint8_t i=0; i<n; ++i){
		char c = bip39_get_word(wi+i)[tmp_len];
		if(c){
			result |= (1 << (c - 'a'));
		}else{
			result |= (1 << ('z' - 'a' + 1));
		}
	}

	return result;
}

static uint32_t accept_bip39_word(const char *str){

	uint8_t n;
	uint16_t wi;

	//this is needed for some special edges cases such as word "bar"
	//since there are "bar" and also "barrel" in the BIP39 wordlist, keyboard
	//hence return "bar_" to symbolize that the word "bar" was chosen
	if(str[strlen(str)-1] == '_'){

		if(strlen(str) > UI_SEED_WORD_MAX_LENGTH) return 0;

		char buffer[UI_SEED_WORD_MAX_LENGTH + 1];
		strcpy(buffer, str);

		//remove '_' char from the end
		buffer[strlen(buffer)-1] = 0x00;

		if((wi = new_bip39_get_index(buffer)) == BIP39_INVALID_WORD_INDEX) return 0;
		return 1;

	}

	if((wi = new_bip39_hint_index_n(str, &n)) == BIP39_INVALID_WORD_INDEX) return 0;
	if(n != 1) return 0;

	return 1;
}

static uint8_t retrieve_mnemonic_length_from_user(){

	uint8_t mnemonic_length_options[] = VALID_MNEMONIC_LENGTHS;

	ui_uint8_t_options_t options_struct = {
		.count_of_options = sizeof(mnemonic_length_options) / sizeof(uint8_t),
		.options = mnemonic_length_options
	};

	uint8_t wc_idx = ui_select_from_options_screen(
		UI_OPTION_TYPE_UINT8_T, &options_struct, "Select count\nof seedwords:"
	);

	return mnemonic_length_options[wc_idx];
}

static char *get_nth_suffix(const uint8_t nth){
	switch(nth){
		case 1:
			return "st";
			break;
		case 2:
			return "nd";
			break;
		case 3:
			return "rd";
			break;
		default: break;
	}
	return "th";
}

static uint8_t confirm_chosen_word(const uint8_t word_no, const uint16_t word){

	display_clear();
	display_puts(0, 0, "Please confirm\nthat ");
	
	char buffer[3];
	uint8_t word_no_str_len = ui_uint32_to_str(word_no, buffer);
	display_puts(8 * strlen("that "), 8, buffer);

	display_puts(8 * (strlen("that ") + word_no_str_len), 8, get_nth_suffix(word_no));

	display_puts(8 * (strlen("that ") + word_no_str_len + 2), 7, " word");

	display_puts(0, 16, "is:\n");
	display_puts(0, 24, bip39_get_word(word));

	ui_button_t left_btn, right_btn;

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
		return 1;
	}

	return 0;
}

uint8_t ui_retrieve_mnemonic_seed_from_user(uint16_t *words){

	while(1){

		uint8_t wc = retrieve_mnemonic_length_from_user();

		uint8_t loaded_wc = 0;

		while(loaded_wc < wc){

			words[loaded_wc] = retrieve_seed_word_from_user(
				loaded_wc+1,
				get_next_possible_bip39_chars,
				accept_bip39_word
			);

			if(confirm_chosen_word(loaded_wc+1, words[loaded_wc])){
				++loaded_wc;
			}

		}

		if(bip39_validate_checksum(words, loaded_wc)){
			break;
		}

		display_clear();
		display_puts(
			0, 0,
			"Invalid mnemonic\nseed was typed\n\nPlease try again"
		);
		display_buffer_display();

		ui_wait_for_any_button_press();
		ui_wait_until_all_buttons_are_released();

	}

}
