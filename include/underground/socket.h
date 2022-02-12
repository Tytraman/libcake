#ifndef __PIKA_SOCKET_H__
#define __PIKA_SOCKET_H__

#include "def.h"
#include <ws2tcpip.h>

#ifdef PIKA_WINDOWS

#define PIKA_IP_V4 AF_INET
#define PIKA_IP_V6 AF_INET6

typedef SOCKET pika_socket;

typedef struct ClientSocket {
    pika_socket socket;
    struct addrinfo *address;
    pika_byte errorFrom;
    int errorCode;
} ClientSocket;

typedef struct ServerSocket {
    pika_socket socket;
    pika_byte errorFrom;
    int errorCode;
} ServerSocket;

typedef struct AcceptedClientSocket {
    pika_socket socket;
    struct sockaddr_in addr;
} AcceptedClientSocket;

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

#define close_socket(sock) closesocket(sock)

#else

#endif

#define PIKA_SOCKET_ERROR_FROM_GETADDRINFO 1
#define PIKA_SOCKET_ERROR_FROM_SOCKET      2
#define PIKA_SOCKET_ERROR_FROM_CONNECT     3
#define PIKA_SOCKET_ERROR_FROM_BIND        4
#define PIKA_SOCKET_ERROR_FROM_LISTEN      5
#define PIKA_SOCKET_ERROR_FROM_ACCEPT      6

// TODO: portage Linux
pika_bool create_client_socket(ClientSocket *sock, const char *hostname, const char *port, pika_byte ipMode);
pika_bool client_socket_connect(ClientSocket *sock);
void free_client_socket(ClientSocket *sock);

pika_bool create_server_socket(ServerSocket *sock, const char *port, pika_byte ipMode, int backlog);
#define free_server_socket(sock) close_socket(sock.socket)

AcceptedClientSocket *server_socket_accept(ServerSocket *sock);
void free_accepted_client_socket(AcceptedClientSocket *sock);

#endif