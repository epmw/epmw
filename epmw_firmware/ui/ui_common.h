#ifndef UI_COMMON_H
#define UI_COMMON_H

#include <stdint.h>

void ui_uint32_to_str(uint32_t x, char *str);

void ui_wait_for_any_button_press();

void ui_wait_until_all_buttons_are_released();

uint8_t ui_wait_and_get_pressed_button();

void ui_print_icon_to_display(const uint8_t width, const uint8_t height, const uint8_t *icon);

#endif