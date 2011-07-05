#ifndef __C_BLOB_H__
#define __C_BLOB_H__

#define BLOB_CACHE_EMPTY 0
#define BLOB_CACHE_MALLOC_PREPROCESSED 1
#define BLOB_CACHE_MALLOC_PLAINTEXT 2
#define BLOB_CACHE_PTR_PREPROCESSED 3
#define BLOB_CACHE_PTR_PLAINTEXT 4

#define BLOB_ENCAPSULATION_PLAINTEXT 'P'
#define BLOB_ENCAPSULATION_COMPRESSED 'C'
#define BLOB_ENCAPSULATION_ENCRYPTED 'E'

typedef struct {
	unsigned char cache_state;
	unsigned char encapsulation;
	unsigned int serialized_size;
	unsigned int spare_size;
} __attribute__((packed)) T_BLOB_HEADER;

typedef struct {
	unsigned short descriptor_length;
	unsigned int data_length;
} __attribute__((packed)) T_BLOB_FIELD_HEADER;

typedef struct {
	T_BLOB_FIELD_HEADER header;
	void *descriptor;
	void *data;
} T_BLOB_CHILD;

typedef struct {
	T_BLOB_HEADER header;
	void *extra_data;
	void *data_start;
	unsigned char hmac[20];
	unsigned int num_children;
	T_BLOB_CHILD *children;
} T_BLOB;

typedef struct {
	unsigned int decrypted_size;
	unsigned char iv[16];
} __attribute__((packed)) T_BLOB_ENCRYPTION_HEADER;

T_BLOB *blob_create(void *data, unsigned int data_length, unsigned char *key);
void blob_destroy(T_BLOB *blob);
unsigned char blob_is_ascii_string(void *data, unsigned int data_length);

#endif