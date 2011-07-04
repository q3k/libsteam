#ifndef __U_SOCKETUTIL_H__
#define __U_SOCKETUTIL_H__

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>

// Some wrappers and helpers around POSIX sockets

int util_getsockaddr(const char *address, unsigned short port, struct sockaddr_in **sockaddr_out, unsigned int *length_out);
// A quick wrapper around select() to implement a recv() timeout
int util_recv_timeout(int socket, void *data, unsigned int length, int timeout);
// Get local IP
unsigned int util_get_local_ip(void);

#endif