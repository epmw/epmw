#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

#define BUTTONS_RCC RCC_GPIOB
#define BUTTONS_GPIO GPIOB
#define BUTTONS_GPIO_LEFT_BTN GPIO0
#define BUTTONS_GPIO_RIGHT_BTN GPIO1

#define BUTTONS_DEBOUNCE_TIME_MS 20

enum button_names {
	LEFT_BUTTON,
	RIGHT_BUTTON
};

void buttons_init();

uint8_t button_get_state(const uint8_t button);

#endif
