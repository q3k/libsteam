#include "Steam2/authclient.h"

void s2_authclient_init(T_S2_AUTHCLIENT *authclient)
{
	s2_serverclient_init((T_S2_SERVERCLIENT *)authclient);
	authclient->ip_external = 0;
	authclient->ip_internal = 0;
}