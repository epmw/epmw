#include "wallet/external_libs/qrcode.h"

#include "display.h"
#include "qr_show.h"

void qr_show(const char *text){

	display_clear();

	QRCode qrcode;

	uint8_t qrcodeBytes[qrcode_getBufferSize(7)];

	qrcode_initText(&qrcode, qrcodeBytes, 7, ECC_MEDIUM, text);

	uint8_t sx = (128 - 8 - qrcode.size) / 2;
	uint8_t sy = (64 - 8 - qrcode.size) / 2;

	for(uint8_t i=0; i<4; ++i) for(uint8_t j=0; j<qrcode.size+8; ++j) display_set_pixel(sx+j, sy+i, 1);
	for(uint8_t y = 0; y < qrcode.size; y++){
		for(uint8_t i=0; i<4; ++i) display_set_pixel(sx+i, sy+y+4, 1);
		for(uint8_t x = 0; x < qrcode.size; x++){
			if(qrcode_getModule(&qrcode, x, y)){
				display_set_pixel(sx+x+4, sy+y+4, 0);
			}else{
				display_set_pixel(sx+x+4, sy+y+4, 1);
			}
		}
		for(uint8_t i=0; i<4; ++i) display_set_pixel(sx+i+qrcode.size+4, sy+y+4, 1);
	}
	for(uint8_t i=0; i<4; ++i) for(uint8_t j=0; j<qrcode.size+8; ++j) display_set_pixel(sx+j, sy+i+qrcode.size+4, 1);

	display_buffer_display();
}
