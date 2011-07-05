/* The following code is subject to the terms and conditions defined in the
   file 'COPYING' which is part of the source code distribution. */

#ifndef __S2_SERVERCLIENT_H__
#define __S2_SERVERCLIENT_H__

#include <sys/socket.h>

// Internal structure
typedef struct {
	int socket;
	char *ip;
	char connected;
} T_S2_SERVERCLIENT;

// Server types
#define E_S2_SERVER_ProxyASClientAuthentication 0
#define E_S2_SERVER_ContentServer 1
#define E_S2_SERVER_GeneralDirectoryServer 2
#define E_S2_SERVER_ConfigServer 3
#define E_S2_SERVER_CSDS 6

// Initializes the internal data structure
void s2_serverclient_init(T_S2_SERVERCLIENT *serverclient);
// Connects to an IP address
int s2_serverclient_connect(T_S2_SERVERCLIENT *serverclient, char *ip, unsigned short port);
// Reads a packet (warning, please free() the result data!)
int s2_serverclient_read(T_S2_SERVERCLIENT *serverclient, void **data_out, unsigned int *length_out);
// Writes a packet
int s2_serverclient_write(T_S2_SERVERCLIENT *serverclient, void *data, unsigned int length);
// Closes socket.
void s2_serverclient_disconnect(T_S2_SERVERCLIENT *serverclient);
// Sends a command to the server
int s2_serverclient_send_command(T_S2_SERVERCLIENT *serverclient, unsigned char command, void *data, unsigned int data_length);
// Performs a handshake
int s2_serverclient_handshake(T_S2_SERVERCLIENT *serverclient, unsigned int expected_server_type);

#endif