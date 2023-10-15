OFILES=main.o pbkdf2-hmac-sha512.o sha512.o utils.o bip32.o bignum256.o hmac_sha512.o ecdsa.o test_helpers.o endian.o qrcode.o blake2b.o base58.o
CC=gcc
CFLAGS=-std=c99 -fstack-usage

all: wallet

wallet: $(OFILES)
	$(CC) $(CFLAGS) -o wallet $(OFILES) # -lm
	chmod +x ./wallet

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean

clean:
	rm -f wallet
	rm -f *.o
	rm -f *.su
