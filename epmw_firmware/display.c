#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "display.h"

#include "font.h"

static uint8_t display_buffer[128*8] = {0};

#define _IF_SB(i2c) ((I2C_SR1(i2c) & I2C_SR1_SB) == 0)
#define _IF_BTF(i2c) ((I2C_SR1(i2c) & I2C_SR1_BTF) == 0)
#define _IF_ADDR(i2c) ((I2C_SR1(i2c) & I2C_SR1_ADDR) == 0)
#define _IF_TxE(i2c) (I2C_SR1(i2c) & I2C_SR1_TxE) == 0

static void ssd1306_i2c_start(const uint8_t addr) {
  i2c_send_start(DISPLAY_I2C);
  while (_IF_SB(DISPLAY_I2C));
  i2c_send_7bit_address(DISPLAY_I2C, addr, I2C_WRITE);
  while (_IF_ADDR(DISPLAY_I2C));
  // cleaning ADDR condition sequence
  I2C_SR2(DISPLAY_I2C);
}

static void ssd1306_i2c_stop(void) {
  i2c_send_stop(DISPLAY_I2C);
  while (_IF_BTF(DISPLAY_I2C));
}

static void ssd1306_i2c_send(uint8_t spec) {
  i2c_send_data(DISPLAY_I2C, spec);
  while (_IF_TxE(DISPLAY_I2C));
}

static void ssd1306_i2c_send_cmd(const uint8_t cmd){
	ssd1306_i2c_start(DISPLAY_I2C_ADDR);
	ssd1306_i2c_send(0x00);
	ssd1306_i2c_send(cmd);
	ssd1306_i2c_stop();
}

//just a dummy not rtos busy delay...
static void delay(const uint32_t value) {
	uint32_t delay_cycles = value * (rcc_ahb_frequency / 1000);

	// Use inline assembly for a non-optimized delay loop
	__asm volatile (
		"1: \n"
		"   subs %0, %0, #1 \n"
		"   bne 1b \n"
		: "+r" (delay_cycles)   // Input/output operand (delay_cycles)
		:                      // No input operands
		:                      // No clobbered registers
	);
}

static void display_clock_setup(){
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_I2C2);
	rcc_periph_clock_enable(RCC_AFIO);
	// AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_JNTRST;	
}

static void i2c_setup(void) {

  i2c_peripheral_disable(DISPLAY_I2C);

  i2c_set_clock_frequency(DISPLAY_I2C, I2C_CR2_FREQ_36MHZ);

  i2c_set_fast_mode(DISPLAY_I2C); //400kHz

  i2c_set_ccr(DISPLAY_I2C, 0x1e);

  i2c_set_trise(DISPLAY_I2C, 0x0b);

  i2c_enable_ack(DISPLAY_I2C);

  i2c_set_own_7bit_slave_address(DISPLAY_I2C, 0x42); // just some dummy number...

  i2c_peripheral_enable(DISPLAY_I2C);
}


void display_init(){

	display_clock_setup();

	gpio_set_mode(
		GPIOB,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
		GPIO_I2C2_SCL | GPIO_I2C2_SDA
	);

	i2c_setup();

	ssd1306_i2c_start(DISPLAY_I2C_ADDR);

	//init bytes setup inspired from: https://github.com/durydevelop/arduino-lib-oled/tree/master
	ssd1306_i2c_send(0x00);
	ssd1306_i2c_send(0xAE);
	ssd1306_i2c_send(0xD5);
	ssd1306_i2c_send(0x80);
	ssd1306_i2c_send(0xA8);
	ssd1306_i2c_send(63);
	ssd1306_i2c_send(0xD3);
	ssd1306_i2c_send(0x00);
	ssd1306_i2c_send(0x40);
	ssd1306_i2c_send(0x8D);
	ssd1306_i2c_send(0x14);
	ssd1306_i2c_send(0x20);
	ssd1306_i2c_send(0x00);
	ssd1306_i2c_send(0xA1);
	ssd1306_i2c_send(0xC8);
	ssd1306_i2c_send(0xDA);
	ssd1306_i2c_send(0x12);
	ssd1306_i2c_send(0x81);
	ssd1306_i2c_send(0x80);
	ssd1306_i2c_send(0xD9);
	ssd1306_i2c_send(0x22);
	ssd1306_i2c_send(0xDB);
	ssd1306_i2c_send(0x20);
	ssd1306_i2c_send(0xA4);
	ssd1306_i2c_send(0xA6);
	ssd1306_i2c_send(0x2E);

	ssd1306_i2c_stop();

	delay(50);

	ssd1306_i2c_start(DISPLAY_I2C_ADDR);

	ssd1306_i2c_send(0x00);
	ssd1306_i2c_send(0xAF);

	ssd1306_i2c_stop();

	display_clear();
}

static inline uint8_t replace_bits(
	const uint8_t save_to_upper_part, const uint8_t amount, uint8_t original, const uint8_t replacement
){
	if(save_to_upper_part){
		original &=~((1 << amount) - 1);
		original |= ((((1 << amount) - 1) << (8-amount)) & replacement) >> (8-amount);
	}else{
		original &=~(((1 << amount) - 1) << (8-amount));
		original |= (((1 << amount) - 1) & replacement) << (8-amount);
	}
	return original;
}

void display_putc(const uint8_t x, const uint8_t y, const char c){
	if(x > 120 || y > 56) return;

	if(y % 8){
		uint8_t upper_bits, lower_bits;
		upper_bits = 8 - (y % 8);
		lower_bits = y % 8;
		for(uint8_t i=0; i<8; ++i){
			display_buffer[(y/8) * 128 + x + i] = replace_bits(
				0, upper_bits, display_buffer[(y/8) * 128 + x + i], font_bytes[c*8+i]
			);
			display_buffer[((y/8)+1) * 128 + x + i] = replace_bits(
				1, lower_bits, display_buffer[((y/8)+1) * 128 + x + i], font_bytes[c*8+i]
			);
		}
	}else{
		memcpy(&(display_buffer[(y / 8) * 128 + x]), &(font_bytes[c*8]), 8);
	}
}

void display_puts(const uint8_t x, const uint8_t y, const char *str){
	char c;
	uint8_t tmp_x = x;
	while((c = (*(str++)))){
		display_putc(tmp_x, y, c);
		tmp_x += 8;
	}
	display_buffer_display();//todo
}

void display_set_pixel(const uint8_t x, const uint8_t y, const uint8_t value){
	display_buffer[(y/8)*128+x] &=~(1 << (0 + (y % 8)));
	if(value) display_buffer[(y/8)*128+x] |= (1 << (0 + (y % 8)));
}

void display_manage_sleep_mode(const uint8_t sleep){
	ssd1306_i2c_start(DISPLAY_I2C_ADDR);

	ssd1306_i2c_send(0x00);
	ssd1306_i2c_send(sleep ? 0xAE : 0xAF);

	ssd1306_i2c_stop();
}

void display_buffer_display(){
	for(uint8_t pg = 0; pg < 8; ++pg){
		ssd1306_i2c_start(DISPLAY_I2C_ADDR);

		ssd1306_i2c_send(0x00);
		ssd1306_i2c_send(0xB0 + pg);
		
		ssd1306_i2c_send(0x02);
		ssd1306_i2c_send(0x10);

		ssd1306_i2c_stop();
		ssd1306_i2c_start(DISPLAY_I2C_ADDR);

		ssd1306_i2c_send(0x40);
		for(uint8_t x = 0; x < 128; ++x){
			ssd1306_i2c_send(display_buffer[pg*128+x]);
		}
		ssd1306_i2c_stop();
	}
}

void display_clear(){
	memset(display_buffer, 0x00, sizeof(display_buffer));
	display_buffer_display();
}
