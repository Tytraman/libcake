/**
* @file socket.h
* @brief Fichier contenant le prototypes de tout ce qui touche aux sockets.
* @author Tytraman
*/

#ifndef __CAKE_SOCKET_H__
#define __CAKE_SOCKET_H__

/// @cond
#include "def.h"
#include "strutf8.h"
/// @endcond

#ifdef CAKE_WINDOWS

/// @cond
#include <ws2tcpip.h>
/// @endcond

typedef int cake_socklen;

typedef SOCKET cake_socket;

#define CAKE_SOCKET_BAD_SOCKET INVALID_SOCKET
#define CAKE_SOCKET_ERROR SOCKET_ERROR


#else
/// @cond
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
/// @endcond

typedef socklen_t cake_socklen;

#define cake_init_winsock() 
#define cake_clean_winsock() 

typedef int cake_socket;

#define CAKE_SOCKET_BAD_SOCKET -1
#define CAKE_SOCKET_ERROR      -1

#define cake_socket_get_last_error_code() errno

#define cake_close_socket(sock) close(sock)
#endif

/// @cond
#include <openssl/ssl.h>
/// @endcond

#define CAKE_IP_V4 AF_INET
#define CAKE_IP_V6 AF_INET6

/* ===== SOCKET ===== */

/**
 * @brief Socket de connexion client.
*/
typedef struct cake_clientsocket {
    cake_socket socket;
    struct addrinfo *address;
    cake_byte errorFrom;            ///< Permet de savoir d'où vient l'erreur.
    ulong errorCode;                ///< Code de l'erreur.
} Cake_ClientSocket;

/**
 * @brief Socket de connexion client utilisant le protocole TLS, sécurisant les communications.
*/
typedef struct cake_tlsclient {
    Cake_ClientSocket clientSocket;     ///< Socket client interne.
    SSL_CTX *ctx;
    SSL *ssl;
} Cake_TLSClient;

/**
 * @brief Socket client accepté par un socket serveur.
*/
typedef struct cake_acceptedclientsocket {
    cake_socket sock;
    struct sockaddr_in addr;
    cake_byte errorFrom;            ///< Permet de savoir d'où vient l'erreur.
    ulong errorCode;                ///< Code de l'erreur.
} Cake_AcceptedClientSocket;

/**
 * @brief Socket de serveur.
*/
typedef struct cake_serversocket {
    cake_socket socket;
    cake_byte errorFrom;            ///< Permet de savoir d'où vient l'erreur.
    int errorCode;                  ///< Code de l'erreur.
} Cake_ServerSocket;

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

typedef struct cake_acceptedclientsocketstream {
    Cake_SocketBuffer buffer;
    Cake_AcceptedClientSocket client;
} Cake_AcceptedClientSocketStream;

#define CAKE_CLIENT_SOCKET_CONNECT_OK 0

#define CAKE_SOCKET_CLOSE 0
#define CAKE_SOCKET_READ_ERROR SOCKET_ERROR

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

#ifdef CAKE_WINDOWS
inline int cake_init_winsock() {
    WSADATA win;
    return WSAStartup(MAKEWORD(2, 2), &win);
}

inline int cake_clean_winsock() {
    return WSACleanup();
}

inline int cake_socket_get_last_error_code() {
    return WSAGetLastError();
}

inline int cake_socket_read(cake_socket sock, char *buffer, int size) {
    return recv(sock, buffer, size, 0);
}

inline int cake_socket_send(cake_socket sock, char *buffer, int size) {
    return send(sock, buffer, size, 0);
}

inline int cake_close_socket(cake_socket sock) {
    return closesocket(sock);
}
#else
#define cake_init_winsock
#define cake_clean_winsock
#endif

inline void cake_init_ssl() {
    SSL_library_init();
    ERR_load_crypto_strings();
    SSL_load_error_strings();
}

/* ===== CLIENT SOCKET ===== */ 

cake_bool __cake_client_socket_recv(void *_s, char *_buffer, ulonglong len, ulonglong *_bytesReceived);
    
/**
 * @brief Crée un socket client sur l'adresse fournie en paramètre.
 * @param[out] sock Socket a initialiser.
 * @param[in] hostname Adresse du serveur auquel se connecter.
 * @param[in] port Port serveur de connexion.
 * @param ipMode Mode IP utilisé, CAKE_IP_V4 ou CAKE_IP_V6.
 * @return @ref cake_false en cas d'erreur.
*/
cake_bool cake_create_client_socket(Cake_ClientSocket *sock, const char *hostname, const char *port, cake_byte ipMode);

/**
 * @brief Permet de connecter un socket client au serveur cible.
 * @param sock Le socket à connecter.
 * @return @ref cake_false en cas d'erreur.
*/
cake_bool cake_client_socket_connect(Cake_ClientSocket *sock);
cake_bool cake_client_socket_send(Cake_ClientSocket *sock, const char *data, ulonglong size);

/**
 * @brief Ferme la connexion d'un socket client.
 * @param sock Le socket a fermer.
*/
void cake_free_client_socket(Cake_ClientSocket *sock);

/* ===== TLS CLIENT ===== */

cake_bool __cake_tls_client_recv(void *_s, char *_buffer, ulonglong len, ulonglong *_bytesReceived);

cake_bool cake_create_tls_client(Cake_TLSClient *tls, const char *hostname, const char *port, cake_byte ipMode);
cake_bool cake_tls_client_connect(Cake_TLSClient *tls);
cake_bool cake_tls_client_send(Cake_TLSClient *tls, const char *data, ulonglong size);
char *cake_tls_client_recv_dyn(Cake_TLSClient *tls, ulonglong size);
void cake_free_tls_client(Cake_TLSClient *tls);

/* ===== ACCEPTED CLIENT SOCKET ===== */

cake_bool __cake_accepted_client_socket_recv(void *_s, char *_buffer, ulonglong len, ulonglong *_bytesReceived);

cake_bool cake_accepted_client_socket_send(Cake_AcceptedClientSocket *sock, const char *data, ulonglong size);
inline void cake_free_accepted_client_socket(Cake_AcceptedClientSocket *client) {
    cake_close_socket(client->sock);
}

/* ===== SERVER SOCKET ===== */

cake_bool cake_create_server_socket(Cake_ServerSocket *sock, const char *port, cake_byte ipMode, int backlog);

inline void cake_free_server_socket(Cake_ServerSocket *sock) {
    cake_close_socket(sock->socket);
}

cake_bool cake_server_socket_accept(Cake_ServerSocket *sock, Cake_AcceptedClientSocket *dest);


/* ===== SOCKET STREAM FUNCTIONS ===== */

Cake_String_UTF8 *__cake_socket_stream_read_line(Cake_SocketBuffer *_stream, cake_bool (*recvFunc)(void *, char *, ulonglong, ulonglong *), void *_s);

cake_bool __cake_tls_client_stream_read_raw(Cake_TLSClientStream *stream, ulonglong len, char *buff);

inline Cake_String_UTF8 *cake_client_socket_stream_read_line(Cake_ClientSocketStream *stream) {
    return __cake_socket_stream_read_line(&stream->buffer, __cake_client_socket_recv, &stream->client);
}
void cake_free_client_socket_stream(Cake_ClientSocketStream *stream);

Cake_String_UTF8 *cake_tls_client_stream_read_line(Cake_TLSClientStream *stream);
char *cake_tls_client_stream_read_raw(Cake_TLSClientStream *stream, ulonglong len);
Cake_String_UTF8 *cake_tls_client_stream_read_str(Cake_TLSClientStream *stream, ulonglong len);
void cake_free_tls_client_stream(Cake_TLSClientStream *stream);

inline Cake_String_UTF8 *cake_accepted_client_socket_stream_read_line(Cake_AcceptedClientSocketStream *stream) {
    return __cake_socket_stream_read_line(&stream->buffer, __cake_accepted_client_socket_recv, &stream->client);
}
void cake_free_accepted_client_socket_stream(Cake_AcceptedClientSocketStream *stream);

#ifdef __cplusplus
}
#endif

#endif