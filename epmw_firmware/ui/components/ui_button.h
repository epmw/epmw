#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#define UI_BUTTON_MAX_TEXT_LENGTH 15

#define UI_BUTTON_BORDER_THICKNESS_PX 1
#define UI_BUTTON_BORDER_PADDING_PX 2

#include <stdint.h>

typedef struct{
	uint8_t start_x, start_y;
	uint8_t end_x, end_y;
	char text[UI_BUTTON_MAX_TEXT_LENGTH+1];
	uint8_t border_on : 1;
	uint8_t active: 1;
} ui_button_t;

uint8_t ui_button_init_button(
	ui_button_t *button,
	const char *text,
	const uint8_t start_x, const uint8_t start_y
);

void ui_button_render_button(const ui_button_t *button);

void ui_button_set_active_state(ui_button_t *button, const uint8_t state);

void ui_button_set_border_on(ui_button_t *button, const uint8_t state);

#endif
