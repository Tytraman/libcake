#ifndef __CAKE_SOCKET_H__
#define __CAKE_SOCKET_H__

#include "def.h"

#ifdef CAKE_WINDOWS

#ifndef CAKE_WIN_SOCK
#define CAKE_WIN_SOCK 0
#endif

#if CAKE_WIN_SOCK > 0
#include <ws2tcpip.h>
#endif

#define cake_init_winsock() WSADATA __winsock_data;\
                       WSAStartup(MAKEWORD(2, 2), &__winsock_data)

#define cake_clean_winsock() WSACleanup()

typedef SOCKET cake_socket;

#define CAKE_SOCKET_BAD_SOCKET INVALID_SOCKET
#define CAKE_SOCKET_ERROR SOCKET_ERROR

#define cake_socket_get_last_error_code() WSAGetLastError()

#define cake_close_socket(sock) closesocket(sock)
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define cake_init_winsock() 
#define cake_clean_winsock() 

typedef int cake_socket;

#define CAKE_SOCKET_BAD_SOCKET -1
#define CAKE_SOCKET_ERROR      -1

#define cake_socket_get_last_error_code() errno

#define cake_close_socket(sock) close(sock)
#endif

#define CAKE_IP_V4 AF_INET
#define CAKE_IP_V6 AF_INET6

typedef struct cake_clientsocket {
    cake_socket socket;
    struct addrinfo *address;
    cake_byte errorFrom;
    int errorCode;
} Cake_ClientSocket;

typedef struct cake_serversocket {
    cake_socket socket;
    cake_byte errorFrom;
    int errorCode;
} Cake_ServerSocket;

typedef struct cake_acceptedclientsocket {
    cake_socket socket;
    struct sockaddr_in addr;
} Cake_AcceptedClientSocket;

#define CAKE_CLIENT_SOCKET_CONNECT_OK 0

#define CAKE_SOCKET_CLOSE 0
#define CAKE_SOCKET_READ_ERROR SOCKET_ERROR

#define cake_socket_read(socket, buffer, size) recv(socket, buffer, size, 0)

#define cake_socket_send(socket, buffer, size) send(socket, buffer, size, 0)

#define CAKE_SOCKET_ERROR_FROM_GETADDRINFO 1
#define CAKE_SOCKET_ERROR_FROM_SOCKET      2
#define CAKE_SOCKET_ERROR_FROM_CONNECT     3
#define CAKE_SOCKET_ERROR_FROM_BIND        4
#define CAKE_SOCKET_ERROR_FROM_LISTEN      5
#define CAKE_SOCKET_ERROR_FROM_ACCEPT      6

cake_bool cake_create_client_socket(Cake_ClientSocket *sock, const char *hostname, const char *port, cake_byte ipMode);
cake_bool cake_client_socket_connect(Cake_ClientSocket *sock);
void cake_free_client_socket(Cake_ClientSocket *sock);

cake_bool cake_create_server_socket(Cake_ServerSocket *sock, const char *port, cake_byte ipMode, int backlog);
#define cake_free_server_socket(sock) cake_close_socket(sock.socket)

Cake_AcceptedClientSocket *cake_server_socket_accept(Cake_ServerSocket *sock);
void cake_free_accepted_client_socket(Cake_AcceptedClientSocket *sock);

#endif