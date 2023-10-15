#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "external_libs/usbcdc.h"

#include "buttons.h"
#include "display.h"
#include "flash_storage.h"
#include "wallet_management.h"
#include "ui.h"

void usb_data_receiver(void *params){
	char buffer[130];
	uint8_t index = 0;
	uint8_t cr_received = 0;
	while(1){
		char c = usb_getc();
		buffer[index++] = c;
		if(c == '\n' && cr_received){
			buffer[index] = 0x00;
			//todo parse it (put it into queue???)
			usb_puts("Received: ");
			usb_puts(buffer);
			display_puts(0, 0, buffer);
			display_buffer_display();
			//-/- todo
			index = 0;
			cr_received = 0;
		}
		cr_received = (c == '\r') ? 1 : 0;
	}
}

void blink_task(void *params){
	while(1){
		gpio_toggle(GPIOC,GPIO13);
		usb_puts("Test\r\n");
		vTaskDelay(500 / portTICK_PERIOD_MS);
		usb_puts("AnotherTest\r\n");
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

int main(void) {

	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC,GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL,GPIO13);

	buttons_init();
	display_init();
	wallet_management_init();

	display_clear();

	//usb communication is working, but for now is not needed yet
	//uncomment to test that it's working :)
	// usb_start();

	// xTaskCreate(blink_task, "blink_task", 500, NULL, configMAX_PRIORITIES-1, NULL);
	// xTaskCreate(usb_data_receiver, "usb_data_receiver", 500, NULL, configMAX_PRIORITIES-1, NULL);
	xTaskCreate(ui_task, "ui_task", 1500, NULL, configMAX_PRIORITIES-1, NULL);

	vTaskStartScheduler();

	while(1);

	return 0;
}
