#ifndef __PIKA_HTTP_H__
#define __PIKA_HTTP_H__

#include "def.h"
#include "socket.h"
#include "utf8.h"

#define PIKA_HTTP_GET  0
#define PIKA_HTTP_POST 1

typedef struct HttpHeader {
    struct HttpHeader *next;
    String_UTF8 *key;
    String_UTF8 *value;
} HttpHeader;

typedef struct HttpClient {
    ClientSocket sock;
    HttpHeader *requestHeader;
    uchar *requestMessage;
    ulonglong requestMessageLength;
    uchar *responseMessage;
    ulonglong responseMessageLength;

    String_UTF8 *finalRequestHeader;

    pika_byte getOrPost;
} HttpClient;

pika_bool create_http_client(HttpClient *client, const char *hostname, const char *port);

HttpHeader **http_add_header_element(HttpHeader **header, const uchar *key, const uchar *value);
void free_http_header(HttpHeader *header);

void http_client_write_final_header(HttpClient *client, const uchar *url);

#define PIKA_HTTP_ERROR_SEND SOCKET_ERROR

#define PIKA_HTTP_HEADER  0
#define PIKA_HTTP_MESSAGE 1
pika_bool http_client_send(HttpClient *client, pika_byte mode);

#define PIKA_HTTP_ERROR_RECEIVE SOCKET_ERROR
pika_bool http_client_receive(HttpClient *client);


void free_http_client(HttpClient *client);

#endif