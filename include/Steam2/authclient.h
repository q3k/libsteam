#ifndef __S2_AUTHCLIENT_H__
#define __S2_AUTHCLIENT_H__

#include "Steam2/serverclient.h"

typedef struct {
	T_S2_SERVERCLIENT serverclient;
	unsigned int ip_internal;
	unsigned int ip_external;
} T_S2_AUTHCLIENT;

typedef struct {
	unsigned int zero;
	unsigned char four;
	unsigned int ip_internal;
	unsigned int user_hash;
} __attribute__((packed)) T_S2_PACKET_REQUEST_IP;

typedef struct {
	unsigned char error;
	unsigned int ip_external;
} __attribute__((packed)) T_S2_PACKET_IP_RESPONSE;

void s2_authclient_init(T_S2_AUTHCLIENT *authclient);
#define s2_authclient_connect(a, b, c) s2_serverclient_connect((T_S2_SERVERCLIENT *)a, b, c)
// The 'main' login function
int s2_authclient_login(T_S2_AUTHCLIENT *authclient, char *username, char *password);
// Some login steps
int s2_authclient_request_ip(T_S2_AUTHCLIENT *authclient, char *username);



#endif