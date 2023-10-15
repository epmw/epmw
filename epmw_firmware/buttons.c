#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/cm3/nvic.h>

#include <FreeRTOS.h>
#include <task.h>

#include "buttons.h"

void buttons_init(){
	rcc_periph_clock_enable(BUTTONS_RCC);
	gpio_set_mode(BUTTONS_GPIO, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, BUTTONS_GPIO_LEFT_BTN | BUTTONS_GPIO_RIGHT_BTN);
	gpio_set(BUTTONS_GPIO, BUTTONS_GPIO_LEFT_BTN | BUTTONS_GPIO_RIGHT_BTN);

	exti_select_source(EXTI0, GPIOB);
	exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);
	exti_enable_request(EXTI0);
	
	exti_select_source(EXTI1, GPIOB);
	exti_set_trigger(EXTI1, EXTI_TRIGGER_FALLING);
	exti_enable_request(EXTI1);
	
	nvic_enable_irq(NVIC_EXTI0_IRQ);
	nvic_enable_irq(NVIC_EXTI1_IRQ);
}

uint8_t button_get_state(const uint8_t button){
	const uint16_t button_gpio = (button == LEFT_BUTTON) ? BUTTONS_GPIO_LEFT_BTN : BUTTONS_GPIO_RIGHT_BTN;
	if(gpio_get(BUTTONS_GPIO, button_gpio)) return 0;
	vTaskDelay(BUTTONS_DEBOUNCE_TIME_MS / portTICK_PERIOD_MS);
	return gpio_get(BUTTONS_GPIO, button_gpio) ? 0 : 1;
}

// #include "display.h"
void exti0_isr(){
	if(exti_get_flag_status(EXTI0)){
		// display_puts(0, 0, "LEFT");
		// display_buffer_display();
		exti_reset_request(EXTI0);
	}
}

void exti1_isr(){
	if(exti_get_flag_status(EXTI1)){
		// display_puts(0, 0, "RIGHT");
		// display_buffer_display();
		exti_reset_request(EXTI1);
	}
}
