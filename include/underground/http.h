#ifndef __PIKA_HTTP_H__
#define __PIKA_HTTP_H__

#include "def.h"
#include "socket.h"
#include "utf8.h"

#define PIKA_HTTP_GET  0
#define PIKA_HTTP_POST 1

#define PIKA_HTTP_REQUEST  0
#define PIKA_HTTP_RESPONSE 1

#define PIKA_HTTP_ERROR_SEND SOCKET_ERROR


#define PIKA_HTTP_HEADER  0
#define PIKA_HTTP_MESSAGE 1

#define PIKA_HTTP_ERROR_RECEIVE   1
#define PIKA_HTTP_ERROR_OVERSIDED 2

typedef struct HttpHeader {
    struct HttpHeader *next;
    String_UTF8 *key;
    String_UTF8 *value;
} HttpHeader;

typedef struct HttpData {
    uchar *dataReceived;
    ulonglong dataReceivedLength;

    HttpHeader *receivedHeader;
    HttpHeader **lastReceivedHeader;
    uchar *receivedMessage;
    ulonglong receivedMessageLength;
    
    HttpHeader *headerToSend;
    HttpHeader **lastHeaderToSend;
    uchar *messageToSend;
    ulonglong messageToSendLength;

    String_UTF8 *finalHeaderToSend;

    pika_byte getOrPost;
} HttpData;

typedef struct HttpClient {
    HttpData data;
    ClientSocket sock;  
} HttpClient;

typedef struct AcceptedHttpClient {
    HttpData data;
    AcceptedClientSocket *sock;
    ulonglong dataReceivedMaxLength;
} AcceptedHttpClient;

typedef ServerSocket HttpServer;


/* ===== Global ===== */

pika_byte http_receive(HttpData *data, pika_socket sock);


/* ===== HttpClient ===== */

pika_bool create_http_client(HttpClient *client, const char *hostname, const char *port);

pika_bool http_client_send(HttpClient *client, pika_byte mode);
#define http_client_receive(client) http_receive(&(client).data, (client).sock.socket)
void free_http_client(HttpClient *client);


/* ===== HttpServer ===== */

#define create_http_server(pServer, port, backlog) create_server_socket(pServer, port, PIKA_IP_V4, backlog)
AcceptedHttpClient *http_server_accept(HttpServer *server, ulonglong requestMessageMaxLength);
#define free_http_server(server) free_server_socket(server)


/* ===== HttpHeader ===== */

void http_add_header_element(HttpHeader **header, const uchar *key, const uchar *value);
void free_http_header(HttpHeader *header);


/* ===== HttpData ===== */

void http_data_write_final_header(HttpData *data, const uchar *url, pika_byte requestOrResponse, uchar *statusCode);
HttpHeader *http_data_parse_header(HttpData *data, HttpHeader **start);
void free_http_data(HttpData *data);


/* ===== AcceptedHttpClient ===== */

pika_bool accepted_http_client_send(AcceptedHttpClient *client, pika_byte mode);
#define accepted_http_client_receive(client) http_receive(&(client).data, (client).sock->socket)
void free_accepted_http_client(AcceptedHttpClient *client);

#endif