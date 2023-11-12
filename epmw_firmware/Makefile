######################################################################
#  Project Makefile
######################################################################

BINARY		= main
SRCFILES	= main.c display.c font.c buttons.c qr_show.c flash_storage.c wallet_management.c ui.c entropy.c wallet/wallet.c ui/ui_button.c
SRCFILES	+= external_libs/usbcdc.c
SRCFILES	+= external_libs/rtos/heap_4.c external_libs/rtos/list.c external_libs/rtos/port.c external_libs/rtos/queue.c external_libs/rtos/tasks.c external_libs/rtos/opencm3.c
SRCFILES	+= wallet/external_libs/pbkdf2-hmac-sha512.c wallet/external_libs/sha512.c wallet/external_libs/utils.c wallet/external_libs/bip32.c wallet/external_libs/bignum256.c wallet/external_libs/hmac_sha512.c wallet/external_libs/ecdsa.c wallet/external_libs/test_helpers.c wallet/external_libs/endian.c wallet/external_libs/qrcode.c wallet/external_libs/blake2b.c wallet/external_libs/base58.c
LDSCRIPT	= stm32f103c8t6.ld

include ./Makefile.incl
include ./Makefile.rtos

.PHONY: all

all: clobber
	make flash

######################################################################
