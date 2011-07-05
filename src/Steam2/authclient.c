#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Steam2/authclient.h"
#include "Util/crypto.h"
#include "Util/socketutil.h"
#include "Util/error.h"

void s2_authclient_init(T_S2_AUTHCLIENT *authclient)
{
	s2_serverclient_init((T_S2_SERVERCLIENT *)authclient);
	authclient->ip_external = 0;
	authclient->ip_internal = 0;
}

int s2_authclient_login(T_S2_AUTHCLIENT *authclient, char *username, char *password)
{
	if (s2_authclient_request_ip(authclient, username))
	{
		ERROR("request ip");
		return 1;
	}
	
	if (s2_authclient_request_salt(authclient, username))
	{
		ERROR("request salt");
		return 1;
	}
	
	if (s2_authclient_do_login(authclient, password))
	{
		ERROR("request salt");
		return 1;
	}
	
	s2_authclient_get_account_info(authclient);
	
	return 0;
}

int s2_authclient_request_ip(T_S2_AUTHCLIENT *authclient, char *username)
{
	T_S2_PACKET_REQUEST_IP packet;
	packet.zero = 0;
	packet.four = 4;
	packet.ip_internal = util_get_local_ip();
	packet.user_hash = util_jenkins_hash((void *)username, strlen(username)) & 1;
	
	if (send(authclient->serverclient.socket, (void *)&packet, sizeof(packet), 0) < 0)
	{
		ERROR("write packet");
		return 1;
	}
	
	T_S2_PACKET_IP_RESPONSE response;
	if (recv(authclient->serverclient.socket, (void *)&response, sizeof(T_S2_PACKET_IP_RESPONSE), 0) < 0)
	{
		ERROR("read packet");
		return 1;
	}
	
	if (response.error != 0)
	{
		printf("%x\n", response.error);
		ERROR("invalid response");
		return 1;
	}
	
	authclient->ip_internal = util_get_local_ip();
	authclient->ip_external = ntohl(response.ip_external);
	
	char *external_ip = inet_ntoa(*(struct in_addr *)(void*)&authclient->ip_external);
	printf("[i] Out external IP address is %s\n", external_ip);
	
	return 0;
}

int s2_authclient_request_salt(T_S2_AUTHCLIENT *authclient, char *username)
{
	unsigned char *data = (unsigned char *)malloc(strlen(username) * 2 + 4);
	unsigned short username_length = htons(strlen(username));
	
	// username_length, username, username_length, username
	memcpy((void *)data, (void *)&username_length, 2);
	memcpy((void *)data + 2, (void *)username, strlen(username));
	memcpy((void *)data + 2 + strlen(username), (void *)data, 2 + strlen(username));
	
	if (s2_serverclient_send_command((T_S2_SERVERCLIENT *)authclient, 2, data, strlen(username) * 2 + 4))
	{
		free((void *)data);
		ERROR("salt request");
		return 1;
	}
	
	if (recv(authclient->serverclient.socket,(void *)&authclient->salt_a, 4, 0) < 0)
	{
		free((void *)data);
		ERROR("salt recv a");
		return 1;
	}
	if (recv(authclient->serverclient.socket,(void *)&authclient->salt_b, 4, 0) < 0)
	{
		free((void *)data);
		ERROR("salt recv b");
		return 1;
	}
	
	//authclient->salt_a = ntohl(authclient->salt_a);
	//authclient->salt_b = ntohl(authclient->salt_b);
	
	printf("[i] Salts: %08x %08x\n", authclient->salt_a, authclient->salt_b);
	
	return 0;
}

int s2_authclient_do_login(T_S2_AUTHCLIENT *authclient, char *password)
{
	unsigned char key[16];
	unsigned char iv[16];
	
	s2_authclient_generate_key(authclient->salt_a, authclient->salt_b, password, key);
	util_generate_random_block(16, iv);
	
	unsigned long long microtime = 0xDCBFFEFF2BC000UL + (unsigned long long)time(0) * 1000000UL;
	microtime ^= s2_authclient_get_obfuscation_mask(authclient->ip_internal, authclient->ip_external);
	
	unsigned char plaintext[12];
	memcpy(plaintext, &microtime, 8);
	memcpy(plaintext + 8, &authclient->ip_internal, 4);
	
	for (int i = 0; i < 12; i++)
		printf("%02x ", plaintext[i]);
	printf("\n");
	
	char *ciphertext;
	unsigned int ciphertext_length;
	
	util_aes_encrypt(plaintext, 12, key, iv, (void **)&ciphertext, &ciphertext_length);
	
	void *packet = malloc(sizeof(T_S2_PACKET_AUTHENTICATE_HEADER) + ciphertext_length);
	
	T_S2_PACKET_AUTHENTICATE_HEADER *header = (T_S2_PACKET_AUTHENTICATE_HEADER *)packet;
	memcpy(header->iv, iv, 16);
	header->plaintext_length = 12;
	header->ciphertext_length = ciphertext_length;
	memcpy((unsigned char *)packet + sizeof(T_S2_PACKET_AUTHENTICATE_HEADER), ciphertext, ciphertext_length);
	
	
	if (s2_serverclient_write((T_S2_SERVERCLIENT *)authclient, packet, sizeof(T_S2_PACKET_AUTHENTICATE_HEADER) + ciphertext_length))
	{
		free((void*)ciphertext);
		ERROR("packet write");
		return 1;
	}
	
	return 0;
}

unsigned char s2_authclient_get_account_info(T_S2_AUTHCLIENT *authclient)
{
	unsigned char result;
	if (recv(authclient->serverclient.socket, &result, 1, 0) < 0)
	{
		ERROR("result recv");
		return 1;
	}
	
	if (result != 0)
	{
		printf("%i\n", result);
		ERROR("result error");
		return 1;
	}
	
	return 0;
}

unsigned long long s2_authclient_get_obfuscation_mask(unsigned int ip_internal, unsigned int ip_external)
{
	unsigned char source[8];
	memcpy(source, &ip_external, 4);
	memcpy(source + 4, &ip_internal, 4);
	
	unsigned char digest[20];
	util_sha1_hash(source, 8, digest);
	
	return *(unsigned long long *)digest;
}

int s2_authclient_generate_key(unsigned int salt_a, unsigned int salt_b, char *password, unsigned char *key_out)
{
	void *source = malloc(8 + strlen(password));
	
	memcpy((char*)source, &salt_a, 4);
	memcpy(((char*)source) + 4, password, strlen(password));
	memcpy(((char*)source) + 4 + strlen(password), &salt_b, 4);
	
	unsigned char digest[20];
	util_sha1_hash(source, 8 + strlen(password), digest);
	
	memcpy(key_out, digest, 16);
	
	return 0;
}
