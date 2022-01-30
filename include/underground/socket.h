#ifndef __PIKA_SOCKET_H__
#define __PIKA_SOCKET_H__

#include "def.h"
#include <ws2tcpip.h>

#ifdef PIKA_WINDOWS

#define PIKA_IP_V4 AF_INET
#define PIKA_IP_V6 AF_INET6

typedef struct ClientSocket {
    int socket;
    struct addrinfo *address;
    pika_byte errorFrom;
    int errorCode;
} ClientSocket;

#define PIKA_CLIENT_SOCKET_CONNECT_OK 0



#define PIKA_SOCKET_CLOSE 0
#define PIKA_SOCKET_READ_ERROR SOCKET_ERROR

// TODO: portage Linux
#define socket_read(socket, buffer, size) recv(socket, buffer, size, 0)

// TODO: portage Linux
#define socket_send(socket, buffer, size) send(socket, buffer, size, 0)

#define init_winsock() WSADATA __winsock_data;\
                       WSAStartup(MAKEWORD(2, 2), &__winsock_data)

#define clean_winsock() WSACleanup()

#else

#endif

#define PIKA_SOCKET_ERROR_FROM_GETADDRINFO 1
#define PIKA_SOCKET_ERROR_FROM_SOCKET      2
#define PIKA_SOCKET_ERROR_FROM_CONNECT     3

// TODO: portage Linux
pika_bool create_client_socket(ClientSocket *sock, const char *hostname, const char *port, pika_byte ipMode);
pika_bool client_socket_connect(ClientSocket *sock);
void free_client_socket(ClientSocket *sock);

#endif