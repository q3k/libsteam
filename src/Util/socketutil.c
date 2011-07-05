#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>

#include "Util/socketutil.h"

int util_getsockaddr(const char *address, unsigned short port, struct sockaddr_in **sockaddr_out, unsigned int *length_out)
{
	struct addrinfo hints, *result;
	
	char port_ascii[10];
	snprintf(port_ascii, 10, "%i", port);
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	if (getaddrinfo(address, port_ascii, &hints, &result))
		return 1;
	
	*sockaddr_out = (struct sockaddr_in *)result->ai_addr;
	*length_out = result->ai_addrlen;
	
	return 0;
}

int util_recv_timeout(int socket, void *data, unsigned int length, int timeout)
{
	/*fd_set sockets;
	
	FD_ZERO(&sockets);
	FD_SET(socket, &sockets);
	
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	
	int result = select(socket + 1, &sockets, NULL, NULL, &tv);
	
	if (result == 0)
		return -2; //timeout
	else if (result == -1)
		return -1; // some other error*/
	
	return recv(socket, data, length, 0);
}

unsigned int util_get_local_ip(void)
{
	char hostname[200];
	if (gethostname(hostname, sizeof(hostname)))
		return 1;
	
	struct hostent *host = gethostbyname(hostname);
	if (host == 0)
		return 1;
	
	return *(unsigned int *)host->h_addr;
}