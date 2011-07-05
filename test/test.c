/* The following code is subject to the terms and conditions defined in the
   file 'COPYING' which is part of the source code distribution. */

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#include "Steam2/serverclient.h"
#include "Steam2/dsclient.h"
#include "Steam2/authclient.h"
#include "Util/crypto.h"

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: %s username password\n", argv[0]);
		return 1;
	}
	
	T_S2_SERVERCLIENT serverclient;
	
	s2_serverclient_init(&serverclient);
	if (s2_serverclient_connect(&serverclient, "72.165.61.189", 27030))
	{
		printf("[e] Could not connect to GDS server!\n");
		return 1;
	}
	
	printf("[i] Connected to GDS server...\n");
	
	T_S2_AUTH_SERVER authserver;
	
	if (s2_dsclient_get_auth_server(&serverclient, argv[1], &authserver))
	{
		printf("[e] Could not get auth servers!\n");
		return 1;
	}
	
	char *server_ip = inet_ntoa(*(struct in_addr *)(void*)&authserver.ip);	
	printf("[i] Using auth server %s:%hu\n", server_ip, authserver.port);
	
	T_S2_AUTHCLIENT authclient;
	
	s2_authclient_init(&authclient);
	
	if (s2_authclient_connect(&authclient, server_ip, authserver.port))
	{
		printf("[e] Could not connect to the auth server!\n");
		return 1;
	}
	
	printf("[i] Connected to auth server.\n");
	
	if (s2_authclient_login(&authclient, argv[1], argv[2]))
	{
		printf("[e] Could not log in!\n");
		return 1;
	}
	
	char *server_a_ip = inet_ntoa(authclient.tgt.server_a.address);
	char *server_b_ip = inet_ntoa(authclient.tgt.server_b.address);
	
	printf("[i] TGT servers: %s:%i, %s:%i\n", server_a_ip, authclient.tgt.server_a.port, server_b_ip, authclient.tgt.server_b.port);
	
	return 0;
}