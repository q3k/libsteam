/* The following code is subject to the terms and conditions defined in the
   file 'COPYING' which is part of the source code distribution. */

#ifndef __U_CRYPTO_H__
#define __U_CRYPTO_H__

unsigned int util_jenkins_hash(void *data, unsigned int data_length);
int util_aes_encrypt(void *input, unsigned long input_length, void *key, void *iv, void **output, unsigned int *output_length);
int util_aes_decrypt(void *input, unsigned long input_length, void *key, void *iv, void **output, unsigned int *output_length);
int util_sha1_hash(void *input, unsigned long input_length, void *output);
int util_generate_random_block(unsigned int size, void *output);
#endif