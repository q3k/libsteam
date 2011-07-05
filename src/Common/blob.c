#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Common/blob.h"
#include "Util/error.h"
#include "Util/crypto.h"

T_BLOB *blob_create(void *data, unsigned int data_length, unsigned char *key)
{
	if (data_length < sizeof(T_BLOB_HEADER))
		return 0;
	
	T_BLOB *blob = (T_BLOB *)malloc(sizeof(T_BLOB));
	
	// Copy over the header from the input
	memcpy(&blob->header, data, sizeof(T_BLOB_HEADER));
	
	switch (blob->header.cache_state)
	{
		case BLOB_CACHE_EMPTY:
			blob->num_children = 0;
			return blob;
		
		case BLOB_CACHE_MALLOC_PREPROCESSED:
		case BLOB_CACHE_MALLOC_PLAINTEXT:
		case BLOB_CACHE_PTR_PREPROCESSED:
		case BLOB_CACHE_PTR_PLAINTEXT:
			break;
			
		default:
			ERROR("Unknown cache state.");
			free((void*)blob);
			return 0;
	}
	
	if (blob->header.serialized_size > data_length)
	{
		printf("missing %i\n", blob->header.serialized_size - data_length);
		ERROR("Cache incomplete! (did you download it completely?");
		free((void*)blob);
		return 0;
	}
	
	printf("[i] Blob size %i bytes\n", blob->header.serialized_size);
	unsigned int serialized_left = blob->header.serialized_size;
	serialized_left -= sizeof(T_BLOB_HEADER);
	
	switch (blob->header.encapsulation)
	{
		case BLOB_ENCAPSULATION_PLAINTEXT:
			blob->data_start = ((char *)data) + sizeof(T_BLOB_HEADER);
			break;
			
		case BLOB_ENCAPSULATION_ENCRYPTED:
			if (key == 0)
			{
				ERROR("Blob was encrypted, but no key was given!");
				free((void*)blob);
				return 0;
			}
			
			T_BLOB_ENCRYPTION_HEADER *encryption = (T_BLOB_ENCRYPTION_HEADER *)(((char *)data) + sizeof(T_BLOB_HEADER));
			blob->extra_data = (void *)encryption;
			
			printf("[i] Encrypted blob %i bytes, %i bytes decrypted\n", serialized_left - 20, encryption->decrypted_size);
			
			void *ciphertext = (void *)(((char *)data) + sizeof(T_BLOB_HEADER) + sizeof(T_BLOB_ENCRYPTION_HEADER));
			
			serialized_left -= sizeof(T_BLOB_ENCRYPTION_HEADER);
			
			unsigned int decrypted_size_check;
			util_aes_decrypt(ciphertext, serialized_left - 20, key, encryption->iv, &blob->data_start, &decrypted_size_check);
			
			printf("[i] Decrypted %i bytes\n", decrypted_size_check);
			
			// Encrypted blobs are a special case, as they have only one child, which is probably also a blob.
			T_BLOB_CHILD *child = (T_BLOB_CHILD *)malloc(sizeof(T_BLOB_CHILD));
			child->header.descriptor_length = 0;
			child->header.data_length = decrypted_size_check;
			child->descriptor = 0;
			
			blob->num_children = 1;
			blob->children = child;
			
			child->data = blob_create(blob->data_start, encryption->decrypted_size, 0);
			
			serialized_left = 0;
			return blob;
		
		default:
			ERROR("Unknown encapsulation method!");
			free((void*)blob);
			return 0;
	}
	
	unsigned char *field = blob->data_start;
	
	// First pass - determine the number of fields
	blob->num_children = 0;
	while (serialized_left > sizeof(T_BLOB_FIELD_HEADER))
	{
		T_BLOB_FIELD_HEADER *header = (T_BLOB_FIELD_HEADER *)field;
		
		field += sizeof(T_BLOB_FIELD_HEADER);
		field += header->descriptor_length;
		field += header->data_length;
		
		serialized_left -= sizeof(T_BLOB_FIELD_HEADER);
		serialized_left -= header->descriptor_length;
		serialized_left -= header->data_length;
		
		blob->num_children++;
	}
	printf("[i] %i children nodes.\n", blob->num_children);
	
	// Now create the list
	blob->children = (T_BLOB_CHILD *)malloc(sizeof(T_BLOB_CHILD) * blob->num_children);
	
	// Second pass - create the children nodes
	field = blob->data_start;
	for (unsigned int i = 0; i < blob->num_children; i++)
	{
		T_BLOB_CHILD *child = &blob->children[i];		
		memcpy(&child->header, field, sizeof(T_BLOB_FIELD_HEADER));		
		
		field += sizeof(T_BLOB_FIELD_HEADER);		
		child->descriptor = field;		
		field += child->header.descriptor_length;
		child->data = field;
		field += child->header.data_length;		
		
		printf("[i] Child %08x, %u bytes of data.\n", *(unsigned int *)child->descriptor, child->header.data_length);
		if (blob_is_ascii_string(child->data, child->header.data_length))
			printf("	ASCII %s\n", (char *)child->data);
	}
	
	return blob;
}

unsigned char blob_is_ascii_string(void *data, unsigned int data_length)
{
	if (((char *)data)[data_length-1] != 0)
		return 0;
	
	for (unsigned int i = 0; i < data_length - 1; i++)
	{
		char letter = ((char *)data)[i];
		if (letter < 32 || letter > 127)
			return 0;
	}
	
	return 1;
}