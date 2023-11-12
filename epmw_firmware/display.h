#ifndef DISPLAY_H
#define DISPLAY_H

#define DISPLAY_I2C I2C2
#define DISPLAY_I2C_ADDR 0x3c

#include <stdint.h>

void display_init();

void display_putc(const uint8_t x, const uint8_t y, const char c);
void display_putc_inverted(const uint8_t x, const uint8_t y, const char c);
void display_puts(const uint8_t x, const uint8_t y, const char *str);
void display_puts_inverted(const uint8_t x, const uint8_t y, const char *str);
void display_set_pixel(const uint8_t x, const uint8_t y, const uint8_t value);
void display_draw_hline(const uint8_t sx, const uint8_t y, const uint8_t ex, const uint8_t fill_value);
void display_draw_vline(const uint8_t x, const uint8_t sy, const uint8_t ey, const uint8_t fill_value);

void display_manage_sleep_mode(const uint8_t sleep);

void display_buffer_display();
// todo fix function bellow
void display_buffer_display_selection(const uint8_t sx, const uint8_t sy, const uint8_t ex, const uint8_t ey);

void display_clear();

#endif
