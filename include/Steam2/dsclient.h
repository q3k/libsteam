/* The following code is subject to the terms and conditions defined in the
   file 'COPYING' which is part of the source code distribution. */

#ifndef __S2_DSCLIENT_H__
#define __S2_DSCLIENT_H__

#include "Steam2/serverclient.h"

typedef struct {
	unsigned int ip;
	unsigned short port;
} T_S2_AUTH_SERVER;

// Sends a server list request
int s2_dsclient_get_server_list(T_S2_SERVERCLIENT *serverclient, char server_type, void *data, unsigned int data_length);
// Gets an auth server from the server
int s2_dsclient_get_auth_server(T_S2_SERVERCLIENT *serverclient, char *username, T_S2_AUTH_SERVER *server);


#endif