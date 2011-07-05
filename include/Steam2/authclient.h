/* The following code is subject to the terms and conditions defined in the
   file 'COPYING' which is part of the source code distribution. */

#ifndef __S2_AUTHCLIENT_H__
#define __S2_AUTHCLIENT_H__

#include "Steam2/serverclient.h"
#include "Common/structs.h"

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

typedef struct {
	unsigned char account_record_key[16];
	T_STEAM_GUID user_id;
	
	T_ADDRESS_PORT server_a;
	T_ADDRESS_PORT server_b;
	
	unsigned long long creation_time;
	unsigned long long expiration_time;
} __attribute__((packed)) T_S2_CLIENT_TGT; // ticket granting ticked

typedef struct {
	T_S2_SERVERCLIENT serverclient;
	unsigned int ip_internal;
	unsigned int ip_external;
	unsigned int salt_a;
	unsigned int salt_b;
	unsigned char key[16];
	
	T_S2_CLIENT_TGT tgt;
} T_S2_AUTHCLIENT;

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