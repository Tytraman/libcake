#ifndef __CAKE_SOCKET_H__
#define __CAKE_SOCKET_H__

#include "def.h"
#include "strutf8.h"

#ifdef CAKE_WINDOWS

#include <ws2tcpip.h>

typedef int cake_socklen;

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

typedef socklen_t cake_socklen;

#define cake_init_winsock() 
#define cake_clean_winsock() 

typedef int cake_socket;

#define CAKE_SOCKET_BAD_SOCKET -1
#define CAKE_SOCKET_ERROR      -1

#define cake_socket_get_last_error_code() errno

#define cake_close_socket(sock) close(sock)
#endif

#include <openssl/ssl.h>

#define CAKE_IP_V4 AF_INET
#define CAKE_IP_V6 AF_INET6

typedef struct cake_clientsocket {
    cake_socket socket;
    struct addrinfo *address;
    cake_byte errorFrom;
    ulong errorCode;
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



typedef struct cake_tlsclient {
    Cake_ClientSocket clientSocket;
    SSL_CTX *ctx;
    SSL *ssl;
} Cake_TLSClient;


/* ===== STREAM ===== */

typedef struct cake_socketbuffer {
    cake_byte *ptr;
    ulonglong size;
} Cake_SocketBuffer;

typedef struct cake_clientsocketstream {
    Cake_SocketBuffer buffer;
    Cake_ClientSocket client;
} Cake_ClientSocketStream;

typedef struct cake_tlsclientstream {
    Cake_SocketBuffer buffer;
    Cake_TLSClient tls;
} Cake_TLSClientStream;

#define CAKE_CLIENT_SOCKET_CONNECT_OK 0

#define CAKE_SOCKET_CLOSE 0
#define CAKE_SOCKET_READ_ERROR SOCKET_ERROR

#define cake_socket_read(socket, buffer, size) recv(socket, buffer, size, 0)

#define cake_socket_send(socket, buffer, size) send(socket, buffer, size, 0)

#define CAKE_SOCKET_ERROR_FROM_NO_ERROR     0
#define CAKE_SOCKET_ERROR_FROM_GETADDRINFO  1
#define CAKE_SOCKET_ERROR_FROM_SOCKET       2
#define CAKE_SOCKET_ERROR_FROM_CONNECT      3
#define CAKE_SOCKET_ERROR_FROM_BIND         4
#define CAKE_SOCKET_ERROR_FROM_LISTEN       5
#define CAKE_SOCKET_ERROR_FROM_ACCEPT       6
#define CAKE_SOCKET_ERROR_FROM_RECV         7
#define CAKE_SOCKET_ERROR_FROM_SEND         8
#define CAKE_SOCKET_ERROR_FROM_SSL_SET_FD   9
#define CAKE_SOCKET_ERROR_FROM_SSL_CONNECT  10
#define CAKE_SOCKET_ERROR_FROM_SSL_WRITE_EX 11
#define CAKE_SOCKET_ERROR_FROM_SSL_READ_EX  12

#define CAKE_SOCKET_READ_BUFFER_SIZE 2048

#ifdef __cplusplus
extern "C" {
#endif

/* ===== CLIENT SOCKET ===== */

cake_bool cake_create_client_socket(Cake_ClientSocket *sock, const char *hostname, const char *port, cake_byte ipMode);
cake_bool cake_client_socket_connect(Cake_ClientSocket *sock);
cake_bool cake_client_socket_send(Cake_ClientSocket *sock, const char *data, ulonglong size);
void cake_free_client_socket(Cake_ClientSocket *sock);


/* ===== TLS CLIENT ===== */

cake_bool cake_create_tls_client(Cake_TLSClient *tls, const char *hostname, const char *port, cake_byte ipMode);
cake_bool cake_tls_client_connect(Cake_TLSClient *tls);
cake_bool cake_tls_client_send(Cake_TLSClient *tls, const char *data, ulonglong size);
char *cake_tls_client_recv_dyn(Cake_TLSClient *tls, ulonglong size);
void cake_free_tls_client(Cake_TLSClient *tls);

cake_bool cake_create_server_socket(Cake_ServerSocket *sock, const char *port, cake_byte ipMode, int backlog);
#define cake_free_server_socket(sock) cake_close_socket(sock.socket)

Cake_AcceptedClientSocket *cake_server_socket_accept(Cake_ServerSocket *sock);
void cake_free_accepted_client_socket(Cake_AcceptedClientSocket *sock);

/* ===== SOCKET STREAM FUNCTIONS ===== */

Cake_String_UTF8 *cake_client_socket_stream_read_line(Cake_ClientSocketStream *stream);
void cake_free_client_socket_stream(Cake_ClientSocketStream *stream);

Cake_String_UTF8 *cake_tls_client_stream_read_line(Cake_TLSClientStream *stream);
char *cake_tls_client_stream_read_raw(Cake_TLSClientStream *stream, ulonglong len);
Cake_String_UTF8 *cake_tls_client_stream_read_str(Cake_TLSClientStream *stream, ulonglong len);
void cake_free_tls_client_stream(Cake_TLSClientStream *stream);

#ifdef __cplusplus
}
#endif

#endif