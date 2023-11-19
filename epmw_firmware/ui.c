#include <FreeRTOS.h>
// #include <task.h>

#include "ui.h"
#include "ui/ui_common.h"
#include "ui/components/ui_icons.h"
#include "ui/components/ui_button.h"
#include "ui/screens/ui_pin_screen.h"
#include "buttons.h"
#include "display.h"
#include "qr_show.h"
#include "wallet/wallet.h"
#include "wallet_management.h"

#include "config.h"
const char *test_mnemonic = TEST_MNEMONIC;

static void ui_new_wallet_new_wallet(){
	display_clear();
	display_puts(20, 20, "TODO NEW");
	display_buffer_display();
	ui_wait_for_any_button_press();
	ui_wait_until_all_buttons_are_released();

	ui_enter_pin_screen();

	display_clear();
	display_puts(20, 20, "After pin");
	display_buffer_display();
	ui_wait_for_any_button_press();
	ui_wait_until_all_buttons_are_released();
}

static void ui_new_wallet_restore_wallet(){
	display_clear();
	display_puts(20, 20, "TODO RESTORE");
	display_buffer_display();
	ui_wait_for_any_button_press();
	ui_wait_until_all_buttons_are_released();	
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

	if(current_selection){
		ui_new_wallet_restore_wallet();
	}else{
		ui_new_wallet_new_wallet();
	}
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

void ui_task(void *params){

	display_clear();
	ui_print_icon_to_display(128, 64, epmw_startup_screen_icon);
	display_buffer_display();
	ui_startup_screen_progress_bar_animation(2000);

	if(!wallet_is_initialized()){
		ui_wallet_initialization();
	}

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
