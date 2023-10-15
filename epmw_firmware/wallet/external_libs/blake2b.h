#ifndef BLAKE2B_H
#define BLAKE2B_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint8_t b[128]; // input buffer
	uint64_t h[8]; // chained state
	uint64_t t[2]; // total number of bytes
	size_t c; // pointer for b[]
	size_t outlen; // digest size
} blake2b_ctx_t;

// Initialize the hashing context "ctx" with optional key "key".
// 1 <= outlen <= 64 gives the digest size in bytes.
// Secret key (also <= 64 bytes) is optional (keylen = 0).
int blake2b_init(blake2b_ctx_t *ctx, size_t outlen,
const void *key, size_t keylen); // secret key
// Add "inlen" bytes from "in" into the hash.
void blake2b_update(blake2b_ctx_t *ctx, // context
const void *in, size_t inlen); // data to be hashed
// Generate the message digest (size given in init).
// Result placed in "out".
void blake2b_final(blake2b_ctx_t *ctx, void *out);
// All-in-one convenience function.
int blake2b(void *out, size_t outlen, // return buffer for digest
const void *key, size_t keylen, // optional secret key
const void *in, size_t inlen); // data to be hashed

#endif
