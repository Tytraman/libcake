#ifndef __CAKE_HTTP_H__
#define __CAKE_HTTP_H__

#include "def.h"
#include "socket.h"
#include "strutf8.h"

#ifndef CAKE_WIN_SOCK
#define CAKE_WIN_SOCK 0
#endif

#ifndef CAKE_SSL
#define CAKE_SSL 0
#endif

#if CAKE_SSL > 0
#include <openssl/ssl.h>
#else
typedef cake_undefined_type SSL;
typedef cake_undefined_type SSL_CTX;
#endif

#define CAKE_HTTP_GET  0
#define CAKE_HTTP_POST 1

#define CAKE_HTTP_REQUEST  0
#define CAKE_HTTP_RESPONSE 1

// Code d'erreur interne
#define CAKE_HTTP_ERROR_SEND CAKE_SOCKET_ERROR
// Code d'erreur interne
#define CAKE_HTTP_ERROR_RECV CAKE_SOCKET_ERROR

#define CAKE_HTTP_CONNECTION_CLOSED 100

#define CAKE_HTTP_HEADER  0
#define CAKE_HTTP_MESSAGE 1

#define CAKE_HTTP_ERROR_RECEIVE   1
#define CAKE_HTTP_ERROR_OVERSIDED 2

typedef struct cake_httpheader {
    struct cake_httpheader *next;
    Cake_String_UTF8 *key;
    Cake_String_UTF8 *value;
} Cake_HttpHeader;

typedef struct cake_httpresponse {
    Cake_BytesBuffer receivedData;

    Cake_HttpHeader *header;
    Cake_HttpHeader **nextHeaderValue;
    
    Cake_String_UTF8 *formattedHeader;
    Cake_BytesBuffer message;

    uchar *status;
} Cake_HttpResponse;

typedef struct cake_httprequest {
    Cake_BytesBuffer receivedData;

    Cake_HttpHeader *header;
    Cake_HttpHeader **nextHeaderValue;

    Cake_String_UTF8 *url;

    Cake_String_UTF8 *formattedHeader;
    Cake_BytesBuffer message;

    cake_byte method;
} Cake_HttpRequest;

typedef struct cake_httpclient {
    Cake_ClientSocket sock;
    // La requête est ce qui est demandée au serveur.
    Cake_HttpRequest request;
    // La réponse est ce qui est reçue du serveur.
    Cake_HttpResponse response;
} Cake_HttpClient;

typedef struct cake_acceptedhttpclient {
    Cake_AcceptedClientSocket *sock;
    // La requête est ce qui est demandée par le client.
    Cake_HttpRequest request;
    // La réponse est ce qui est envoyée au client.
    Cake_HttpResponse response;
    ulonglong dataReceivedMaxLength;
} Cake_AcceptedHttpClient;

typedef Cake_ServerSocket Cake_HttpServer;


typedef struct cake_acceptedhttpsclient {
    Cake_AcceptedHttpClient *client;
    SSL *ssl;
} Cake_AcceptedHttpsClient;

typedef struct cake_httpsserver {
    Cake_HttpServer server;
    SSL_CTX *ctx;
} Cake_HttpsServer;


/* ===== Global ===== */

cake_byte cake_http_receive(Cake_BytesBuffer *dest, Cake_BytesBuffer *destMessage, Cake_HttpHeader **header, cake_byte *getOrPost, Cake_String_UTF8 *url, cake_socket sock);
cake_byte cake_https_receive(Cake_BytesBuffer *dest, Cake_BytesBuffer *destMessage, Cake_HttpHeader **header, cake_byte *getOrPost, Cake_String_UTF8 *url, SSL *ssl);

/* ===== Cake_HttpResponse ===== */

void cake_http_response_format(Cake_HttpResponse *response);
void cake_create_http_response(Cake_HttpResponse *response);
void cake_clear_http_response(Cake_HttpResponse *response);


/* ===== Cake_HttpRequest ===== */

void cake_http_request_format(Cake_HttpRequest *request);
void cake_create_http_request(Cake_HttpRequest *request);
void cake_clear_http_request(Cake_HttpRequest *request);


/* ===== Cake_HttpClient ===== */

cake_bool cake_create_http_client(Cake_HttpClient *client, const char *hostname, const char *port);

cake_bool cake_http_client_send(Cake_HttpClient *client, cake_byte mode);
#define cake_http_client_receive(client) cake_http_receive(&(client).data, (client).sock.socket)
void cake_free_http_client(Cake_HttpClient *client);


/* ===== Cake_HttpServer ===== */

#define cake_create_http_server(pServer, port, backlog) cake_create_server_socket(pServer, port, CAKE_IP_V4, backlog)

cake_bool cake_create_https_server(Cake_HttpsServer *serverDest, const uchar *port, int backlog, const uchar *cacertPath, const uchar *certPath, const uchar *keyPath);
Cake_AcceptedHttpsClient *cake_https_server_accept(Cake_HttpsServer *server, ulonglong requestMessageMaxLength);
void cake_free_https_server(Cake_HttpsServer *server);

Cake_AcceptedHttpClient *cake_http_server_accept(Cake_HttpServer *server, ulonglong requestMessageMaxLength);
#define cake_free_http_server(server) cake_free_server_socket(server)


/* ===== Cake_HttpHeader ===== */

void cake_http_add_header_element(Cake_HttpHeader **header, const char *key, const char *value);
void cake_free_http_header(Cake_HttpHeader *header);
Cake_HttpHeader *cake_http_header_parse(Cake_BytesBuffer *data, Cake_HttpHeader **start, cake_byte *getOrPost, Cake_String_UTF8 *url);


/* ===== Cake_AcceptedHttpClient ===== */

cake_bool cake_accepted_http_client_send(Cake_AcceptedHttpClient *client, cake_byte mode);
#define cake_accepted_http_client_receive(c) cake_http_receive(&(c).request.receivedData, &(c).request.message, &(c).request.header, &(c).request.method, (c).request.url, (c).sock->socket)
Cake_LinkedList_String_UTF8_Pair *cake_accepted_http_client_parse_post_message(Cake_AcceptedHttpClient *client);
void cake_free_accepted_http_client(Cake_AcceptedHttpClient *client);

cake_bool cake_accepted_https_client_send(Cake_AcceptedHttpsClient *client, cake_byte mode);
#define cake_accepted_https_client_receive(c) cake_https_receive(&(*(c).client).request.receivedData, &(*(c).client).request.message, &(*(c).client).request.header, &(*(c).client).request.method, (*(c).client).request.url, (c).ssl)
void cake_free_accepted_https_client(Cake_AcceptedHttpsClient *client);
void cake_init_openssl();

Cake_HttpHeader *cake_http_header_find(Cake_HttpHeader *first, const char *key);

#endif