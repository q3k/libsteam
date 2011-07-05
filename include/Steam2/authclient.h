#ifndef __S2_AUTHCLIENT_H__
#define __S2_AUTHCLIENT_H__

#include "Steam2/serverclient.h"

typedef struct {
	T_S2_SERVERCLIENT serverclient;
	unsigned int ip_internal;
	unsigned int ip_external;
	unsigned int salt_a;
	unsigned int salt_b;
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

typedef struct {
	unsigned char iv[16];
	unsigned short plaintext_length;
	unsigned short ciphertext_length;
} __attribute__((packed)) T_S2_PACKET_AUTHENTICATE_HEADER;

void s2_authclient_init(T_S2_AUTHCLIENT *authclient);
#define s2_authclient_connect(a, b, c) s2_serverclient_connect((T_S2_SERVERCLIENT *)a, b, c)
// The 'main' login function
int s2_authclient_login(T_S2_AUTHCLIENT *authclient, char *username, char *password);
// Some login steps
int s2_authclient_request_ip(T_S2_AUTHCLIENT *authclient, char *username);
int s2_authclient_request_salt(T_S2_AUTHCLIENT *authclient, char *username);
int s2_authclient_do_login(T_S2_AUTHCLIENT *authclient, char *password);
unsigned char s2_authclient_get_account_info(T_S2_AUTHCLIENT *authclient);
// Some helper functions
int s2_authclient_generate_key(unsigned int salt_a, unsigned int salt_b, char *password, unsigned char *key_out);
unsigned long long s2_authclient_get_obfuscation_mask(unsigned int ip_internal, unsigned int ip_external);


#endif