#include <FreeRTOS.h>
#include <task.h>

#include "ui.h"
#include "buttons.h"
#include "display.h"
#include "qr_show.h"
#include "wallet/wallet.h"
#include "wallet_management.h"

#include "config.h"
const char *test_mnemonic = TEST_MNEMONIC;

static void uint32_to_str(uint32_t x, char *str){
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

static void wait_for_any_button_press(){
	while((!button_get_state(LEFT_BUTTON)) && (!button_get_state(RIGHT_BUTTON))){
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

static void ui_wallet_initialization(){
	display_clear();
	display_puts(0, 0, "Wallet is not");
	display_puts(0, 8, "initialized");
	display_puts(0, 16, "Start init");
	display_puts(0, 24, "by pressing");
	display_puts(0, 32, "any button");
	display_buffer_display();

	wait_for_any_button_press();

	display_clear();
	//todo show entropy state during entropy retrieval...
	display_puts(0, 0, "Entropy:");
	char buffer[17];
	uint32_to_str(1234, buffer);
	display_puts(0, 8, buffer);
	display_buffer_display();
}

void ui_task(void *params){

	display_clear();
	display_puts(0, 0, "Ergo Poor Man's ");
	display_puts(0, 8, "     Wallet     ");
	display_buffer_display();

	vTaskDelay(2000 / portTICK_PERIOD_MS);

	//Currently commented so we can mock the wallet initialized state
	// if(!wallet_is_initialized()){
	// 	ui_wallet_initialization();
	// }

	display_clear();
	display_puts(0, 0, "Deriving xpub");
	display_puts(0, 8, "from seed...");
	display_puts(0, 16, "Please wait :)");
	display_buffer_display();

	char xpub_buffer[157]; //todo remove "magic" from paranthesses number

	wallet_xpub_from_mnemonic(xpub_buffer, test_mnemonic);

	display_clear();
	qr_show(xpub_buffer);

	while(1);
}
