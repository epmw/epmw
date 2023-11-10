#include <string.h>

#include "external_libs/pbkdf2-hmac-sha512.h"
#include "external_libs/bip32.h"
#include "external_libs/ecdsa.h" //for ECC math
#include "external_libs/blake2b.h"
#include "external_libs/libbase58.h"

#include "wallet.h"

//for now ignore passphrase but to the future I will be happy if all ergo wallets will support passphrase B-)
const char *salt = "mnemonic";

static char hex_nibble_to_char(const uint8_t nibble){
	if(nibble > 0x0F) return 'X'; //fail in case of wrong argument value
	return (nibble < 0x0A) ? ('0' + nibble) : ('A' + (nibble - 0x0A));
}

static void convert_uint8_t_into_hex_chars(const uint8_t byte, char *output){
	output[0] = hex_nibble_to_char((byte >> 4) & 0x0F);
	output[1] = hex_nibble_to_char(byte & 0x0F);
	output[2] = 0x00;
}

uint8_t wallet_xpub_from_mnemonic(char *xpub_buffer, const char *mnemonic){
	uint8_t out[64];
	pbkdf2_hmac_sha512(out, 64, (const uint8_t*)mnemonic, strlen(mnemonic), (const uint8_t*)salt, strlen(salt), 2048);

	uint8_t master_node[64];
	bip32SeedToNode(master_node, out, 64);

	uint8_t big_num_out[32], chain_code[32];

	//traditional ergo path, todo make it configurable...
	uint32_t path[4] = {
		44 | (1 << 31),
		429 | (1 << 31),
		0 | (1 << 31),
		0
	};

	bip32DerivePrivatePlusChainCode(big_num_out, chain_code, master_node, path, 4);

	PointAffine p;
	uint8_t priv[32];
	uint8_t pub[ECDSA_MAX_SERIALISE_SIZE];

	setToG(&p);
	memcpy(priv, big_num_out, 32);//todo fix me (redundant memory allocation...)
	pointMultiply(&p, priv);
	ecdsaSerialise(pub, &p, 1);

	//version bytes, depth, fingerprint, child number
	const char *xpub_prefix = "0488B21E" "04" "00000000" "00000000";
	// char xpub_buffer[8 + 2 + 8 + 8 + 32 * 2 + 33 * 2 + 1] = {0};

	memcpy(xpub_buffer, xpub_prefix, strlen(xpub_prefix));
	char *c = &(xpub_buffer[strlen(xpub_prefix)]);

	for(uint8_t i=0; i<32; ++i){
		convert_uint8_t_into_hex_chars(chain_code[i], c);
		c += 2;
	}

	for(uint8_t i=0; i<33; ++i){
		convert_uint8_t_into_hex_chars(pub[i], c);
		c += 2;
	}

	//todo - check mnemonic seed corretness before any xpub calculation?
	return 1;
}

uint8_t wallet_address_from_mnemonic(char *address, const char *mnemonic, const uint32_t child_index){

	uint8_t out[64];
	pbkdf2_hmac_sha512(out, 64, (const uint8_t*)mnemonic, strlen(mnemonic), (const uint8_t*)salt, strlen(salt), 2048);

	uint8_t master_node[64];
	bip32SeedToNode(master_node, out, 64);

	uint8_t big_num_out[32], chain_code[32];

	uint32_t path_to_child[5] = {
		44 | (1 << 31),
		429 | (1 << 31),
		0 | (1 << 31),
		0,
		child_index
	};

	bip32DerivePrivate(big_num_out, master_node, path_to_child, 5);

	PointAffine p;
	uint8_t priv[32];
	uint8_t pub[ECDSA_MAX_SERIALISE_SIZE];

	setToG(&p);
	memcpy(priv, big_num_out, 32);//todo fix me (redundant memory allocation...)
	pointMultiply(&p, priv);
	ecdsaSerialise(pub, &p, 1);

	uint8_t network_prefix_and_address_type_byte = 0x00 + 0x01; //todo fix me???
	uint8_t checksum_hash_output[32];
	
	blake2b_ctx_t ctx;
	blake2b_init(&ctx, 32, 0x00, 0);
	blake2b_update(&ctx, &network_prefix_and_address_type_byte, 1);
	blake2b_update(&ctx, pub, 33);
	blake2b_final(&ctx, checksum_hash_output);

	uint8_t op1[38];
	op1[0] = network_prefix_and_address_type_byte;
	memcpy(&(op1[1]), pub, 33);
	memcpy(&(op1[34]), checksum_hash_output, 4);

	char b58[52+1];
	size_t tmp;

	b58enc(b58, &tmp, op1, 38);

	memcpy(address, b58, sizeof(b58)); //todo fix me - store b58 directly into address

	//todo - check mnemonic seed corretness before any xpub calculation?
	return 1;
}

uint8_t wallet_ergo_schnorr_sign(char *signed_msg_buffer, const uint8_t *msg, uint8_t *sk, uint8_t *random_bytes){

	uint8_t y[32];
	bigModulo(y, random_bytes);
	swapEndian256(y);

	if(bigIsZero(y)){
		return 0;
	}

	PointAffine w, pk;
	uint8_t w_ser[ECDSA_MAX_SERIALISE_SIZE], pk_ser[ECDSA_MAX_SERIALISE_SIZE];
	swapEndian256(sk);

	setToG(&w);
	pointMultiply(&w, y);
	ecdsaSerialise(w_ser, &w, 1);

	setToG(&pk);
	pointMultiply(&pk, sk);
	ecdsaSerialise(pk_ser, &pk, 1);

	uint8_t commitment[77];
	
	commitment[0] = 0x01;
	commitment[1] = 0x00;
	commitment[2] = 0x27;
	commitment[3] = 0x10;
	commitment[4] = 0x01;
	commitment[5] = 0x08;
	commitment[6] = 0xcd;

	memcpy(&(commitment[7]), pk_ser, 33);

	commitment[40] = 0x73;
	commitment[41] = 0x00;
	commitment[42] = 0x00;
	commitment[43] = 0x21;

	memcpy(&(commitment[44]), w_ser, 33);

	uint8_t c_hash[32];

	blake2b_ctx_t ctx;
	
	blake2b_init(&ctx, 32, 0x00, 0);
	blake2b_update(&ctx, commitment, 77);
	blake2b_update(&ctx, msg, 32);
	blake2b_final(&ctx, c_hash);

	memset(&(c_hash[24]), 0x00, 8);

	if(bigIsZero(c_hash)){
		return 0;
	}

	setFieldToN();

	swapEndianUniversal(c_hash, 24);
	uint8_t z[32];
	bigMultiply(z, sk, c_hash);
	bigAdd(z, z, y);
	swapEndian256(z);

	swapEndianUniversal(c_hash, 24);

	char signed_msg[112+1];
	char *c = &(signed_msg[0]);

	for(uint8_t i=0; i<24; ++i){
		convert_uint8_t_into_hex_chars(c_hash[i], c);
		c += 2;
	}

	for(uint8_t i=0; i<32; ++i){
		convert_uint8_t_into_hex_chars(z[i], c);
		c += 2;
	}

	memcpy(signed_msg_buffer, signed_msg, sizeof(signed_msg));
	return 1;
}
