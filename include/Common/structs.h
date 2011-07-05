/* The following code is subject to the terms and conditions defined in the
   file 'COPYING' which is part of the source code distribution. */

#ifndef __C_STRUCTS_H__
#define __C_STRUCTS_H__

#include <netinet/in.h>

typedef struct {
	unsigned short instance;
	unsigned long long account_id;
} __attribute__((packed)) T_STEAM_GUID;

typedef struct {
	struct in_addr address;
	unsigned short port;
} __attribute__((packed)) T_ADDRESS_PORT;

#endif