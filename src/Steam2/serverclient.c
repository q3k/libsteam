#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "Steam2/serverclient.h"
#include "Util/error.h"
#include "Util/socketutil.h"

// timeout for recv() calls
#define S2_TIMEOUT 100

void s2_serverclient_init(T_S2_SERVERCLIENT *serverclient)
{
	serverclient->connected = 0;
	serverclient->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serverclient->ip = "";
}

int s2_serverclient_connect(T_S2_SERVERCLIENT *serverclient, char *ip, unsigned short port)
{
	struct sockaddr_in *address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	unsigned int address_length;
	
	if (util_getsockaddr(ip, port, &address, &address_length))
	{
		ERROR("s2_serverclient_connect failed (util_getsockaddr)!");
		free((void *)address);
		return 1;
	}
	
	serverclient->ip = ip;
	
	if (connect(serverclient->socket, (struct sockaddr *)address, address_length))
	{
		ERROR("s2_serverclient_connect failed (connect)!");
		free((void *)address);
		return 1;
	}
	
	serverclient->connected = 1;
	free((void *)address);
	return 0;
}

int s2_serverclient_read(T_S2_SERVERCLIENT *serverclient, void **data_out, unsigned int *length_out)
{
	int result;
	
	unsigned int length;
	result = util_recv_timeout(serverclient->socket, &length, 4, S2_TIMEOUT);
	
	if (result == -2)
	{
		ERROR("read length timeout");
		return 1;
	}
	else if (result == -1)
	{
		ERROR("read length error");
		return 1;
	}
	
	length = ntohl(length);
	
	*data_out = malloc(length);
	result = util_recv_timeout(serverclient->socket, *data_out, length, S2_TIMEOUT);
	
	if (result == -2)
	{
		ERROR("read data timeout");
		free(*data_out);
		return 1;
	}
	else if (result == -1)
	{
		ERROR("read data error");
		free(*data_out);
		return 1;
	}
	
	*length_out = length;
	
	return 0;
}

int s2_serverclient_write(T_S2_SERVERCLIENT *serverclient, void *data, unsigned int length)
{
	unsigned char *data_final = (unsigned char *)malloc(length + 4);
	unsigned int length_network = ntohl(length);
	
	memcpy(data_final, &length_network, 4);
	memcpy(data_final + 4, data, length);
	
	unsigned int bytes_sent = 0;
	unsigned int bytes_total = length + 4;
	int result = 0;
	
	while (bytes_sent < bytes_total)
	{
		result = send(serverclient->socket, data_final + bytes_sent, bytes_total, 0);
		if (result)
			break;
		
		bytes_sent += result;
		bytes_total -= result;
	}
	free(data_final);
	return result == -1 ? 1 : 0;
}

void s2_serverclient_disconnect(T_S2_SERVERCLIENT *serverclient)
{
	if (serverclient->connected)
	{
		serverclient->connected = 0;
		close(serverclient->socket);
	}
}

int s2_serverclient_send_command(T_S2_SERVERCLIENT *serverclient, unsigned char command, void *data, unsigned int data_length)
{
	unsigned char *data_final = (unsigned char *)malloc(data_length + 1);
	data_final[0] = command;
	
	memcpy(data_final + 1, data, data_length);
	
	if (s2_serverclient_write(serverclient, data_final, data_length + 1))
	{
		ERROR("send_command write");
		free((void *)data_final);
		return 1;
	}	
	
	free((void *)data_final);
	return 0;
}

int s2_serverclient_handshake(T_S2_SERVERCLIENT *serverclient, unsigned int expected_server_type)
{
	unsigned long type_network = htonl(expected_server_type);	
	if (send(serverclient->socket, (void *)&type_network, sizeof(unsigned long), 0) < 1)
	{
		ERROR("handshake write error");
		return 1;
	}
	
	unsigned char result;
	if (recv(serverclient->socket, &result, 1, 0) < 0)
	{
		ERROR("handshake read error");
		return 1;
	}
	
	return result == 1 ? 0 : 1;
}