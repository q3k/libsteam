#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "Steam2/dsclient.h"
#include "Util/crypto.h"
#include "Util/error.h"

int s2_dsclient_get_server_list(T_S2_SERVERCLIENT *serverclient, char server_type, void *data, unsigned int data_length)
{
	if (s2_serverclient_handshake(serverclient, E_S2_SERVER_GeneralDirectoryServer))
	{
		ERROR("dsclient list handshake");
		return 1;
	}
	
	if (s2_serverclient_send_command(serverclient, server_type, data, data_length))
	{
		ERROR("dsclient list command");
		return 1;
	}
	
	return 0;
}

int s2_dsclient_get_auth_server(T_S2_SERVERCLIENT *serverclient, char *username, T_S2_AUTH_SERVER *server)
{
	unsigned int user_hash = util_jenkins_hash((void *)username, strlen(username)) & 1;
	if (s2_dsclient_get_server_list(serverclient, E_S2_SERVER_ProxyASClientAuthentication, (void* )&user_hash, 4))
	{
		ERROR("get auth server list");
		return 1;
	}
	
	void *result;
	unsigned int result_length;
	if (s2_serverclient_read(serverclient, &result, &result_length))
	{
		free(result);
		return 1;
	}
	
	if (result_length < 4)
	{
		ERROR("no auth server");
		free(result);
		return 1;
	}
	
	unsigned short num_servers = ntohs(*(unsigned short *)result);
	
	if (num_servers < 1)
	{
		ERROR("no auth server");
		free(result);
		return 1;
	}
	
	server->ip = *(unsigned int *)(((char *)result) + 2);
	server->port = *(unsigned short *)(((char *)result) + 6);
	
	free(result);
	return 0;
}