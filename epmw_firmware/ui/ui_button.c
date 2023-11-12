#include <string.h>

#include "ui_button.h"
#include "../display.h"

uint8_t ui_button_init_button(
	ui_button_t *button,
	const char *text,
	const uint8_t start_x, const uint8_t start_y
){
	//todo optimize me - start copying string to button-text, fail if string is longer
	//otherwise we will have already copied result inside of button->text B-)
	if(strlen(text) > UI_BUTTON_MAX_TEXT_LENGTH) return 2;
	
	memcpy(button->text, text, strlen(text)+1);
	
	button->start_x = start_x;
	button->start_y = start_y;

	button->end_x = (
		start_x +
		2 * (UI_BUTTON_BORDER_THICKNESS_PX + UI_BUTTON_BORDER_PADDING_PX) +
		8 * strlen(text)
	);

	button->end_y = (
		start_y +
		2 * (UI_BUTTON_BORDER_THICKNESS_PX + UI_BUTTON_BORDER_PADDING_PX) +
		8
	);

	if(button->end_x >= 128 || button->end_y >= 64) return 3;

	button->border_on = 0;
	button->active = 0;

	ui_button_render_button(button);
	return 1;
}

void ui_button_render_button(const ui_button_t *button){
	//todo fix me for cases where border thickness is not equal to 1px
	display_draw_hline(button->start_x, button->start_y, button->end_x, button->border_on);
	display_draw_hline(button->start_x, button->end_y, button->end_x, button->border_on);
	display_draw_vline(button->start_x, button->start_y, button->end_y, button->border_on);
	display_draw_vline(button->end_x, button->start_y, button->end_y, button->border_on);

	for(uint8_t i=0; i<UI_BUTTON_BORDER_PADDING_PX; ++i){
		display_draw_hline(button->start_x+1, button->start_y+i+1, button->end_x-1, button->active);
		display_draw_hline(button->start_x+1, button->end_y-i-1, button->end_x-1, button->active);
		display_draw_vline(button->start_x+i+1, button->start_y+1, button->end_y-1, button->active);
		display_draw_vline(button->end_x-i-1, button->start_y+1, button->end_y-1, button->active);
	}

	if(button->active){
		display_puts_inverted(
			button->start_x + (UI_BUTTON_BORDER_THICKNESS_PX + UI_BUTTON_BORDER_PADDING_PX),
			button->start_y + (UI_BUTTON_BORDER_THICKNESS_PX + UI_BUTTON_BORDER_PADDING_PX),
			button->text
		);		
	}else{
		display_puts(
			button->start_x + (UI_BUTTON_BORDER_THICKNESS_PX + UI_BUTTON_BORDER_PADDING_PX),
			button->start_y + (UI_BUTTON_BORDER_THICKNESS_PX + UI_BUTTON_BORDER_PADDING_PX),
			button->text
		);
	}
}

void ui_button_set_active_state(ui_button_t *button, const uint8_t state){
	button->active = state ? 1 : 0;
	ui_button_render_button(button);
}

void ui_button_set_border_on(ui_button_t *button, const uint8_t state){
	button->border_on = state ? 1 : 0;
	ui_button_render_button(button);
}
