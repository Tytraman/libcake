#ifndef __PIKA_HTTP_H__
#define __PIKA_HTTP_H__

#include "def.h"
#include "socket.h"
#include "utf8.h"

#include <openssl/ssl.h>

#define PIKA_HTTP_GET  0
#define PIKA_HTTP_POST 1

#define PIKA_HTTP_REQUEST  0
#define PIKA_HTTP_RESPONSE 1

// Code d'erreur interne
#define PIKA_HTTP_ERROR_SEND PIKA_SOCKET_ERROR
// Code d'erreur interne
#define PIKA_HTTP_ERROR_RECV PIKA_SOCKET_ERROR

#define PIKA_HTTP_CONNECTION_CLOSED 100

#define PIKA_HTTP_HEADER  0
#define PIKA_HTTP_MESSAGE 1

#define PIKA_HTTP_ERROR_RECEIVE   1
#define PIKA_HTTP_ERROR_OVERSIDED 2

typedef struct HttpHeader {
    struct HttpHeader *next;
    String_UTF8 *key;
    String_UTF8 *value;
} HttpHeader;

typedef struct HttpResponse {
    BytesBuffer receivedData;

    HttpHeader *header;
    HttpHeader **nextHeaderValue;
    
    String_UTF8 *formattedHeader;
    BytesBuffer message;

    uchar *status;
} HttpResponse;

typedef struct HttpRequest {
    BytesBuffer receivedData;

    HttpHeader *header;
    HttpHeader **nextHeaderValue;

    String_UTF8 *url;

    String_UTF8 *formattedHeader;
    BytesBuffer message;

    pika_byte method;
} HttpRequest;

typedef struct HttpClient {
    ClientSocket sock;
    // La requête est ce qui est demandée au serveur.
    HttpRequest request;
    // La réponse est ce qui est reçue du serveur.
    HttpResponse response;
} HttpClient;

typedef struct AcceptedHttpClient {
    AcceptedClientSocket *sock;
    // La requête est ce qui est demandée par le client.
    HttpRequest request;
    // La réponse est ce qui est envoyée au client.
    HttpResponse response;
    ulonglong dataReceivedMaxLength;
} AcceptedHttpClient;

typedef struct AcceptedHttpsClient {
    AcceptedHttpClient *client;
    SSL *ssl;
} AcceptedHttpsClient;

typedef ServerSocket HttpServer;

typedef struct HttpsServer {
    HttpServer server;
    SSL_CTX *ctx;
} HttpsServer;


/* ===== Global ===== */

pika_byte http_receive(BytesBuffer *dest, BytesBuffer *destMessage, HttpHeader **header, pika_byte *getOrPost, String_UTF8 *url, pika_socket sock);
pika_byte https_receive(BytesBuffer *dest, BytesBuffer *destMessage, HttpHeader **header, pika_byte *getOrPost, String_UTF8 *url, SSL *ssl);

/* ===== HttpResponse ===== */

void http_response_format(HttpResponse *response);
void create_http_response(HttpResponse *response);
void clear_http_response(HttpResponse *response);


/* ===== HttpRequest ===== */

void http_request_format(HttpRequest *request);
void create_http_request(HttpRequest *request);
void clear_http_request(HttpRequest *request);


/* ===== HttpClient ===== */

pika_bool create_http_client(HttpClient *client, const char *hostname, const char *port);

pika_bool http_client_send(HttpClient *client, pika_byte mode);
#define http_client_receive(client) http_receive(&(client).data, (client).sock.socket)
void free_http_client(HttpClient *client);


/* ===== HttpServer ===== */

#define create_http_server(pServer, port, backlog) create_server_socket(pServer, port, PIKA_IP_V4, backlog)
pika_bool create_https_server(HttpsServer *serverDest, const uchar *port, int backlog, const uchar *cacertPath, const uchar *certPath, const uchar *keyPath);
AcceptedHttpClient *http_server_accept(HttpServer *server, ulonglong requestMessageMaxLength);
AcceptedHttpsClient *https_server_accept(HttpsServer *server, ulonglong requestMessageMaxLength);
#define free_http_server(server) free_server_socket(server)
void free_https_server(HttpsServer *server);


/* ===== HttpHeader ===== */

void http_add_header_element(HttpHeader **header, const uchar *key, const uchar *value);
void free_http_header(HttpHeader *header);
HttpHeader *http_header_parse(BytesBuffer *data, HttpHeader **start, pika_byte *getOrPost, String_UTF8 *url);


/* ===== AcceptedHttpClient ===== */

pika_bool accepted_http_client_send(AcceptedHttpClient *client, pika_byte mode);
pika_bool accepted_https_client_send(AcceptedHttpsClient *client, pika_byte mode);
#define accepted_http_client_receive(c) http_receive(&(c).request.receivedData, &(c).request.message, &(c).request.header, &(c).request.method, (c).request.url, (c).sock->socket)
#define accepted_https_client_receive(c) https_receive(&(*(c).client).request.receivedData, &(*(c).client).request.message, &(*(c).client).request.header, &(*(c).client).request.method, (*(c).client).request.url, (c).ssl)
LinkedList_String_UTF8_Pair *accepted_http_client_parse_post_message(AcceptedHttpClient *client);
void free_accepted_http_client(AcceptedHttpClient *client);
void free_accepted_https_client(AcceptedHttpsClient *client);

HttpHeader *http_header_find(HttpHeader *first, const uchar *key);

void init_openssl();

#endif