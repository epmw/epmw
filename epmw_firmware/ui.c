#include <FreeRTOS.h>
// #include <task.h>

#include "ui.h"
#include "ui/ui_common.h"
#include "ui/components/ui_icons.h"
#include "ui/components/ui_button.h"
#include "ui/screens/ui_pin_screen.h"
#include "ui/screens/ui_seed_enter_screen.h"

#include "buttons.h"
#include "display.h"
#include "qr_show.h"
#include "wallet/wallet.h"
#include "wallet_management.h"

#include "config.h"
const char *test_mnemonic = TEST_MNEMONIC;

static uint8_t ui_new_wallet_new_wallet(uint16_t *words){
	display_clear();
	display_puts(
		0, 0,
		"TODO yet to be\nimplemented\n\nPlease restart\nEPMW and chose\ndifferent option"
	);
	display_buffer_display();
	//keep yielding from this task
	while(1){
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	// ui_wait_for_any_button_press();
	// ui_wait_until_all_buttons_are_released();
	//todo fix me
	return 0; //just for now so compiler will be happy
}

static uint8_t ui_new_wallet_restore_wallet(uint16_t *words){
	display_clear();
	display_puts(0, 0, "In order to\nrestore wallet,\nplease enter\nyour mnemonic\nseed (words).");
	display_buffer_display();
	ui_wait_for_any_button_press();
	ui_wait_until_all_buttons_are_released();
	return ui_retrieve_mnemonic_seed_from_user(words);
}

static void ui_new_wallet_new_or_restore_selection(){
	display_clear();

	ui_button_t new_wallet_btn, restore_wallet_btn;
	ui_button_t left_btn, right_btn;

	ui_button_init_button(&new_wallet_btn, "  New wallet  ", 2, 5);
	ui_button_set_border_on(&new_wallet_btn, 1);
	ui_button_init_button(&restore_wallet_btn, "Restore wallet", 2, 30);

	ui_button_init_button(&left_btn, "<>", 30, 48);
	ui_button_set_border_on(&left_btn, 1);
	ui_button_init_button(&right_btn, "OK", 60, 48);
	ui_button_set_border_on(&right_btn, 1);
	
	display_buffer_display();

	uint8_t current_selection = 0;
	while(1){
		if(ui_wait_and_get_pressed_button() == LEFT_BUTTON){
			current_selection = (current_selection + 1) % 2;
			ui_button_set_active_state(&left_btn, 1);
			ui_button_set_border_on(&new_wallet_btn, (current_selection == 0 ) ? 1 : 0);
			ui_button_set_border_on(&restore_wallet_btn, current_selection);
			display_buffer_display();
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&left_btn, 0);
			display_buffer_display();
			continue;
		}
		ui_button_set_active_state(&right_btn, 1);
		display_buffer_display();
		break;
	}

	ui_wait_until_all_buttons_are_released();

	uint16_t words[MNEMONIC_MAX_LENGTH];
	uint8_t wc;

	if(current_selection){
		wc = ui_new_wallet_restore_wallet(words);
	}else{
		wc = ui_new_wallet_new_wallet(words);
	}

	display_clear();

	display_puts(0, 0, "Please, setup\nyour wallet's\npin code");
	display_buffer_display();
	ui_wait_for_any_button_press();
	ui_wait_until_all_buttons_are_released();

	pin_code_t pin_code = ui_setup_pin_screen();

	wallet_management_set_pin_code(pin_code);

	display_clear();
	display_puts(0, 0, "Wallet's pin\ncode was\nsetuped\nsuccessfuly!");
	display_buffer_display();
	ui_wait_for_any_button_press();
	ui_wait_until_all_buttons_are_released();

	wallet_set_as_initialized();
}

static void ui_wallet_initialization(){

	display_clear();
	display_puts(0, 0, "Wallet is not");
	display_puts(0, 8, "initialized");
	display_puts(0, 24, "Start init");
	display_puts(0, 32, "by pressing");
	display_puts(0, 40, "any button");
	display_buffer_display();

	ui_wait_for_any_button_press();
	ui_wait_until_all_buttons_are_released();

	ui_new_wallet_new_or_restore_selection();

	//todo pin selection...

	/*
	//todo show entropy state during entropy retrieval...
	display_puts(0, 0, "Entropy:");
	char buffer[17];
	uint32_to_str(1234, buffer);
	display_puts(0, 8, buffer);
	display_buffer_display();
	*/
}

static void ui_startup_screen_progress_bar_animation(const uint16_t ms_duration){
	for(uint8_t x=0; x<128; ++x){
		display_set_pixel(x, 63, 1);
		// todo fix function bellow
		// display_buffer_display_selection(x, 63, x, 63);
		display_buffer_display();
		vTaskDelay((ms_duration / 128) / portTICK_PERIOD_MS);
	}
}

static void ui_lock_screen(){

	display_clear();

	ui_print_icon_to_display(
		48, 8,
		32, 32,
		sizeof(epmw_big_lock_32x32_icon) * 8, epmw_big_lock_32x32_icon
	);

	display_puts(0, 48, "Wallet is locked");
	ui_wait_for_any_button_press();
	ui_wait_until_all_buttons_are_released();

	while(1){

		uint8_t pin_failed_attempts = wallet_management_get_pin_failed_attempts();

		pin_code_t pin_code = ui_enter_pin_screen();

		uint16_t seconds_to_wait = 1 << pin_failed_attempts;

		wallet_management_set_pin_failed_attempts(pin_failed_attempts+1);

		display_clear();

		display_puts(0, 0, "Please wait\n\nseconds");
		
		char tmp_bfr[6];

		while(seconds_to_wait){
			ui_uint32_to_str(seconds_to_wait, tmp_bfr);
			display_puts(0, 8, "     ");
			display_puts(0, 8, tmp_bfr);
			display_buffer_display();
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			--seconds_to_wait;
		}

		display_clear();

		display_puts(0, 0, "Checking pin...");
		display_buffer_display();
		vTaskDelay(500 / portTICK_PERIOD_MS);

		if(wallet_management_check_pin_code(pin_code)){
			display_clear();
			display_puts(0, 0, "Wallet unlocked\nsuccessfuly!");
			display_buffer_display();
			wallet_management_set_pin_failed_attempts(0);
			vTaskDelay(500 / portTICK_PERIOD_MS);
			break;
		}

		if(pin_failed_attempts >= PIN_CODE_MAX_RETRY_COUNT - 1){
			wallet_delete();
			display_clear();
			display_puts(0, 0, "All pin code\nattempts were\nexhausted!\n\nWallet was\ndeleted!");
			while(1){
				vTaskDelay(1000 / portTICK_PERIOD_MS);
			}
		}

		display_clear();
		display_puts(0, 0, "Pin code\ndo not match!\n\nPlease try again");
		display_buffer_display();

		ui_wait_for_any_button_press();
		ui_wait_until_all_buttons_are_released();

	}

}

static void ui_debug_wallet_delete(){

	display_clear();

	display_puts(0, 0, "DEBUG:\nDelete already\ninitialized\nwallet?");
	display_buffer_display();

	ui_button_t left_btn, right_btn;

	ui_button_init_button(&left_btn, "NO", 30, 48);
	ui_button_set_border_on(&left_btn, 1);
	ui_button_init_button(&right_btn, "OK", 60, 48);
	ui_button_set_border_on(&right_btn, 1);
	
	if(ui_wait_and_get_pressed_button() == LEFT_BUTTON){

		ui_button_set_active_state(&left_btn, 1);
		ui_wait_until_all_buttons_are_released();
		ui_button_set_active_state(&left_btn, 0);
		return;

	}else{

		ui_button_set_active_state(&right_btn, 1);
		ui_wait_until_all_buttons_are_released();
		ui_button_set_active_state(&right_btn, 0);

		display_clear();
		display_puts(0, 0, "Do you really\nwant to delete\nthe wallet?");
		display_buffer_display();

		ui_button_init_button(&left_btn, "NO", 30, 48);
		ui_button_set_border_on(&left_btn, 1);
		ui_button_init_button(&right_btn, "OK", 60, 48);
		ui_button_set_border_on(&right_btn, 1);

		if(ui_wait_and_get_pressed_button() == LEFT_BUTTON){

			ui_button_set_active_state(&left_btn, 1);
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&left_btn, 0);

			return;

		}else{

			ui_button_set_active_state(&right_btn, 1);
			ui_wait_until_all_buttons_are_released();
			ui_button_set_active_state(&right_btn, 0);

			wallet_delete();
			display_clear();
			display_puts(0, 0, "Wallet was\ndeleted\n\nPlease restart\nthe device!");
			display_buffer_display();

			while(1){
				vTaskDelay(1000 / portTICK_PERIOD_MS);
			}			

		}

	}
}

void ui_task(void *params){

	display_clear();

	ui_print_icon_to_display(
		0, 0,
		128, 64,
		sizeof(epmw_startup_screen_icon) * 8, epmw_startup_screen_icon
	);

	display_buffer_display();

	ui_startup_screen_progress_bar_animation(2000);

	if(!wallet_is_initialized()){
		ui_wallet_initialization();
	}else{
		ui_debug_wallet_delete();
	}

	ui_lock_screen();

	display_clear();
	display_puts(0, 0, "Deriving xpub");
	display_puts(0, 8, "from seed...");
	display_puts(0, 16, "Please wait :)");
	display_buffer_display();

	char xpub_buffer[157]; //todo remove "magic" from paranthesses number

	wallet_xpub_from_mnemonic(xpub_buffer, test_mnemonic);

	display_clear();
	qr_show(xpub_buffer);

	//keep yielding from this task
	while(1){
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
