#ifndef DISPLAY_H
#define DISPLAY_H

#define DISPLAY_I2C I2C2
#define DISPLAY_I2C_ADDR 0x3c

#include <stdint.h>

void display_init();

void display_putc(const uint8_t x, const uint8_t y, const char c);
void display_puts(const uint8_t x, const uint8_t y, const char *str);
void display_set_pixel(const uint8_t x, const uint8_t y, const uint8_t value);

void display_manage_sleep_mode(const uint8_t sleep);

void display_buffer_display();

void display_clear();

#endif
