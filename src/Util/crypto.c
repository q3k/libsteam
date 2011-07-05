/* The following code is subject to the terms and conditions defined in the
   file 'COPYING' which is part of the source code distribution. */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>

#include "Util/crypto.h"

unsigned int util_jenkins_hash(void *data, unsigned int data_length)
{
	unsigned int a = 0x9e3779b9;
	unsigned int b = 0x9e3779b9;
	unsigned int c = 0;
	
	char *buffer = (char *)data;
	
	unsigned int i = 0;
	while (i + 12 <= data_length)
	{
		unsigned int increment = (unsigned int)buffer[i++];
		increment |= ((unsigned int)buffer[i++] << 8);
		increment |= ((unsigned int)buffer[i++] << 16);
		increment |= ((unsigned int)buffer[i++] << 24);
		a +=  increment; 
		
		increment = (unsigned int)buffer[i++];
		increment |= ((unsigned int)buffer[i++] << 8);
		increment |= ((unsigned int)buffer[i++] << 16);
		increment |= ((unsigned int)buffer[i++] << 24);
		b += increment;
		
		increment = (unsigned int)buffer[i++];
		increment |= ((unsigned int)buffer[i++] << 8);
		increment |= ((unsigned int)buffer[i++] << 16);
		increment |= ((unsigned int)buffer[i++] << 24);
		c += increment;
		
		a -= b; a -= c; a ^= (c>>13);
		b -= c; b -= a; b ^= (a<<8);
		c -= a; c -= b; c ^= (b>>13);
		a -= b; a -= c; a ^= (c>>12);
		b -= c; b -= a; b ^= (a<<16);
		c -= a; c -= b; c ^= (b>>5);
		a -= b; a -= c; a ^= (c>>3);
		b -= c; b -= a; b ^= (a<<10);
		c -= a; c -= b; c ^= (b>>15);
	}
	
	c += data_length;
	
	if (i < data_length)
		a += buffer[i++];
	if (i < data_length)
		a += (unsigned int)buffer[i++] << 8;
	if (i < data_length)
		a += (unsigned int)buffer[i++] << 16;
	if (i < data_length)
		a += (unsigned int)buffer[i++] << 24;
	if (i < data_length)
		b += (unsigned int)buffer[i++];
	if (i < data_length)
		b += (unsigned int)buffer[i++] << 8;
	if (i < data_length)
		b += (unsigned int)buffer[i++] << 16;
	if (i < data_length)
		b += (unsigned int)buffer[i++] << 24;
	if (i < data_length)
		c += (unsigned int)buffer[i++] << 8;
	if (i < data_length)
		c += (unsigned int)buffer[i++] << 16;
	if (i < data_length)
		c += (unsigned int)buffer[i++] << 24;
	
	a -= b; a -= c; a ^= (c>>13);
	b -= c; b -= a; b ^= (a<<8);
	c -= a; c -= b; c ^= (b>>13);
	a -= b; a -= c; a ^= (c>>12);
	b -= c; b -= a; b ^= (a<<16);
	c -= a; c -= b; c ^= (b>>5);
	a -= b; a -= c; a ^= (c>>3);
	b -= c; b -= a; b ^= (a<<10);
	c -= a; c -= b; c ^= (b>>15);
	return c;
}
int util_aes_encrypt(void *input, unsigned long input_length, void *key, void *iv, void **output, unsigned int *output_length)
{	
	// Initialize OpenSSL AES stuff
	EVP_CIPHER_CTX e_ctx;
	EVP_CIPHER_CTX_init(&e_ctx);
	EVP_EncryptInit_ex(&e_ctx, EVP_aes_128_cbc(), NULL, key, iv);
	
	// Encrypt
	unsigned char *ciphertext = malloc(input_length + 128);
	unsigned int ciphertext_length;
	unsigned int final_length;
	EVP_EncryptInit_ex(&e_ctx, NULL, NULL, NULL, NULL);
	EVP_EncryptUpdate(&e_ctx, ciphertext, (int *)&ciphertext_length, input, input_length);
	EVP_EncryptFinal_ex(&e_ctx, ciphertext + ciphertext_length, (int *)&final_length);
	
	*output = ciphertext;
	*output_length = ciphertext_length + final_length;
	
	return 0;
}

int util_aes_decrypt(void *input, unsigned long input_length, void *key, void *iv, void **output, unsigned int *output_length)
{	
	// Initialize OpenSSL AES stuff
	EVP_CIPHER_CTX d_ctx;
	EVP_CIPHER_CTX_init(&d_ctx);
	EVP_DecryptInit_ex(&d_ctx, EVP_aes_128_cbc(), NULL, key, iv);
	
	// Decrypt
	unsigned char *plaintext = malloc(input_length);
	unsigned int plaintext_length;
	unsigned int final_length;
	EVP_DecryptInit_ex(&d_ctx, NULL, NULL, NULL, NULL);
	EVP_DecryptUpdate(&d_ctx, plaintext, (int *)&plaintext_length, input, input_length);
	EVP_DecryptFinal_ex(&d_ctx, plaintext + plaintext_length, (int *)&final_length);
	
	*output = plaintext;
	*output_length = plaintext_length + final_length;
	
	return 0;
}

int util_sha1_hash(void *input, unsigned long input_length, void *output)
{
	EVP_MD_CTX md_ctx;
	
	EVP_MD_CTX_init(&md_ctx);
	EVP_DigestInit_ex(&md_ctx, EVP_sha1(), NULL);
	EVP_DigestUpdate(&md_ctx, input, input_length);
	unsigned int output_length;
	EVP_DigestFinal_ex(&md_ctx, output, &output_length);
	EVP_MD_CTX_cleanup(&md_ctx);
	
	return 0;
}

int util_generate_random_block(unsigned int size, void *output)
{
	// Yeah, that's not very secure.
	srand(time(NULL));
	for (unsigned int i = 0; i < size; i ++)
		((char *)output)[i] = rand() % 255;
	
	return 0;
}