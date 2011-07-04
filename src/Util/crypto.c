#include "Util/crypto.h"

unsigned long util_jenkins_hash(void *data, unsigned int data_length)
{
	unsigned long hash, i;
	
	for(hash = i = 0; i < data_length; ++i)
	{
		hash += ((unsigned char *)data)[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}