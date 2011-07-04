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