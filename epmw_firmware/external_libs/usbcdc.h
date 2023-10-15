/* libusbcdc header
 * Warren W. Gay VE3WWG
 */
#ifndef LIBUSBCDC_H
#define LIBUSBCDC_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void usb_start(void);
bool usb_ready(void);

void usb_putc(char ch);
void usb_puts(const char *buf);
void usb_write(const char *buf,unsigned bytes);

int usb_getc(void);

#endif /* LIBUSBCDC_H */

/* End libusbcdc.h */
