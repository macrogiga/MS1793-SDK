//  aes_crypt.c
//  WeChat Embedded
//
//  Created by harlliu on 14-02-26.
//  Copyright 2014 Tencent. All rights reserved.
//

#include "aes_crypt.h"
#include <string.h>
#include <stdlib.h>

int aes_cbc_encrypt_init(AES_CTX *c, const uint8_t key[16])
{
	int ret;
	if (c == NULL || key == NULL)
		return -1;

	memcpy(c->iv, key, 16);

	ret = AES_set_encrypt_key(key, 128, &c->key);
	if(ret != 0) return -1;

	return 0;
}

int aes_cbc_encrypt_update(AES_CTX *c, const uint8_t in[16], uint8_t out[16])
{
	if (c == NULL || in == NULL || out == NULL)
		return -1;

	AES_cbc_encrypt(in, out, 16, &c->key, c->iv);
	return 0;
}

int aes_cbc_encrypt_final(AES_CTX *c, const uint8_t *in, int in_len, uint8_t out[32], int *out_len)
{
	int padding_len;
	if (c == NULL || in == NULL || in_len <= 0 || in_len > 16 || out == NULL || out_len == NULL)
		return -1;

	//pkcs#7 padding
	padding_len = 16 - (in_len % 16);
	*out_len = in_len + padding_len;

	if (in != out)
		memcpy(out, in, in_len);
	
	memset(out+in_len, padding_len, padding_len);

	if (*out_len > 16) {
		AES_cbc_encrypt(out, out, 16, &c->key, c->iv);
		out += 16;
		AES_cbc_encrypt(out, out, 16, &c->key, c->iv);
	} else {
		AES_cbc_encrypt(out, out, 16, &c->key, c->iv);
	}

	return 0;
}

int aes_cbc_decrypt_init(AES_CTX *c, const uint8_t key[16])
{
	int ret;
	if (c == NULL || key == NULL)
		return -1;

	memcpy(c->iv, key, 16);

	ret = AES_set_decrypt_key(key, 128, &c->key);
	if(ret != 0) return -1;

	return 0;
}

int aes_cbc_decrypt_update(AES_CTX *c, const uint8_t in[16], uint8_t out[16])
{
	if (c == NULL || in == NULL || out == NULL)
		return -1;

	AES_cbc_decrypt(in, out, 16, &c->key, c->iv);
	return 0;
}

int aes_cbc_decrypt_final(AES_CTX *c, const uint8_t in[16], uint8_t out[16], int *out_len)
{
	int padding_len;
	if (c == NULL || in == NULL || out == NULL)
		return -1;

	AES_cbc_decrypt(in, out, 16, &c->key, c->iv);
	padding_len = out[15];
	if (padding_len < 1 || padding_len > 16)
		return -1;
	
	if (out_len != NULL)
		*out_len = 16 - padding_len;

	return 0;
}
