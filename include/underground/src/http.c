#include "../http.h"

#include <stdio.h>
#include <stdlib.h>
#if PIKA_SSL > 0
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

pika_bool create_http_client(HttpClient *client, const char *hostname, const char *port) {
    if(!create_client_socket(&client->sock, hostname, port, PIKA_IP_V4))
        return pika_false;

    if(!client_socket_connect(&client->sock)) {
        free_client_socket(&client->sock);
        return pika_false;
    }

    create_http_request(&client->request);
    create_http_response(&client->response);

    http_add_header_element(&client->request.header, "host", hostname);
    client->request.nextHeaderValue = &client->request.header->next;
    http_add_header_element(client->request.nextHeaderValue, "user-agent", "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0");
    client->request.nextHeaderValue = &(*client->request.nextHeaderValue)->next;
    http_add_header_element(client->request.nextHeaderValue, "accept", "*/*");
    client->request.nextHeaderValue = &(*client->request.nextHeaderValue)->next;
    http_add_header_element(client->request.nextHeaderValue, "accept-language", "en-US");
    client->request.nextHeaderValue = &(*client->request.nextHeaderValue)->next;
    http_add_header_element(client->request.nextHeaderValue, "connection", "close");
    client->request.nextHeaderValue = &(*client->request.nextHeaderValue)->next;

    return pika_true;
}

#if PIKA_SSL > 0
AcceptedHttpClient *http_server_accept(HttpServer *server, ulonglong requestMessageMaxLength) {
    AcceptedHttpClient *client = (AcceptedHttpClient *) malloc(sizeof(AcceptedHttpClient));

    client->sock = server_socket_accept(server);
    client->dataReceivedMaxLength = requestMessageMaxLength;

    if(client->sock == NULL) {
        free(client);
        return NULL;
    }

    create_http_request(&client->request);
    create_http_response(&client->response);

    http_add_header_element(&client->response.header, "connection", "close");
    client->response.nextHeaderValue = &client->response.header->next;

    return client;
}

AcceptedHttpsClient *https_server_accept(HttpsServer *server, ulonglong requestMessageMaxLength) {
    AcceptedHttpsClient *client = (AcceptedHttpsClient *) malloc(sizeof(AcceptedHttpsClient));
    client->client = http_server_accept(&server->server, requestMessageMaxLength);
    if(client->client == NULL) {
        free(client);
        return NULL;
    }
    client->ssl = SSL_new(server->ctx);
    if(client->ssl == NULL) {
        fprintf(stderr, "Erreur: SSL_new\n");
        ERR_print_errors_fp(stderr);
        free_accepted_http_client(client->client);
        free(client);
        return NULL;
    }

    if(!SSL_set_fd(client->ssl, (int) client->client->sock->socket)) {
        fprintf(stderr, "Erreur: SSL_set_fd\n");
        ERR_print_errors_fp(stderr);
        free_accepted_http_client(client->client);
        free(client);
        return NULL;
    }

    if(SSL_accept(client->ssl) != 1) {
        fprintf(stderr, "Erreur: SSL_accept\n");
        ERR_print_errors_fp(stderr);
        free_accepted_http_client(client->client);
        free(client);
        return NULL;
    }
    return client;
}
#endif

void free_http_client(HttpClient *client) {
    clear_http_response(&client->response);
    clear_http_request(&client->request);
    close_socket(client->sock.socket);
}

void http_add_header_element(HttpHeader **header, const uchar *key, const uchar *value) {
    *header = (HttpHeader *) malloc(sizeof(HttpHeader));
    (*header)->key = strutf8(key);
    (*header)->value = strutf8(value);
    (*header)->next = NULL;
}

void free_http_header(HttpHeader *header) {
    if(header != NULL) {
        HttpHeader *current = header;
        HttpHeader *next = current->next;
        while(current != NULL) {
            free_strutf8(current->key);
            free_strutf8(current->value);
            next = current->next;
            free(current);
            current = next;
        }
    }
}

void __http_format_header(String_UTF8 *dest, BytesBuffer *message, HttpHeader *header) {
    while(header != NULL) {
        strutf8_add_char_array(dest, header->key->bytes);
        strutf8_add_char_array(dest, ":");
        strutf8_add_char_array(dest, header->value->bytes);
        strutf8_add_char_array(dest, "\r\n");
        header = header->next;
    }
    if(message->size > 0) {
        pika_byte lengthBuff[256];
        ulonglong_to_char_array(message->size, lengthBuff);
        strutf8_add_char_array(dest, "content-length");
        strutf8_add_char_array(dest, ":");
        strutf8_add_char_array(dest, lengthBuff);
        strutf8_add_char_array(dest, "\r\n");
    }
    strutf8_add_char_array(dest, "\r\n");
}

void http_response_format(HttpResponse *response) {
    array_char_to_strutf8("HTTP/1.1 ", response->formattedHeader);
    strutf8_add_char_array(response->formattedHeader, response->status);
    strutf8_add_char_array(response->formattedHeader, "\r\n");
    __http_format_header(response->formattedHeader, &response->message, response->header);
}

void http_request_format(HttpRequest *request) {
    if(request->method == PIKA_HTTP_GET)
        array_char_to_strutf8("GET ", request->formattedHeader);
    else
        array_char_to_strutf8("POST ", request->formattedHeader);
    strutf8_add_char_array(request->formattedHeader, request->url->bytes);
    strutf8_add_char_array(request->formattedHeader, " HTTP/1.1\r\n");
    __http_format_header(request->formattedHeader, &request->message, request->header);
}

pika_bool http_client_send(HttpClient *client, pika_byte mode) {
    ulonglong index = 0;
    int bytesSent;
    ulonglong diff;
    switch(mode) {
        case PIKA_HTTP_HEADER:
            while(1) {
                diff = client->request.formattedHeader->data.length - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock.socket, &client->request.formattedHeader->bytes[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff), 0)) == PIKA_HTTP_ERROR_SEND)
                    return pika_false;
                index += bytesSent;
            }
            break;
        case PIKA_HTTP_MESSAGE:
            while(1) {
                diff = client->request.message.size - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock.socket, &client->request.message.buffer[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff), 0)) == PIKA_HTTP_ERROR_SEND)
                    return pika_false;
                index += bytesSent;
            }
            break;
        default:
            return pika_false;
    }

    return pika_true;
}

typedef struct HttpSendFusion {
    pika_socket sock;
    #if PIKA_SSL > 0
    SSL *ssl;
    #endif
} HttpSendFusion;

int __http_send_callback(HttpSendFusion *fusion, uchar *buff, int length) {
    return send(fusion->sock, buff, length, 0);
}

#if PIKA_SSL > 0
int __https_send_callback(HttpSendFusion *fusion, uchar *buff, int length) {
    return SSL_write(fusion->ssl, buff, length);
}
#endif

pika_bool __accepted_http_client_send(AcceptedHttpClient *client, pika_byte mode, HttpSendFusion *fusion, int (*sendCallback)(HttpSendFusion *, uchar *, int)) {
    ulonglong index = 0;
    int bytesSent;
    ulonglong diff;
    switch(mode) {
        case PIKA_HTTP_HEADER:
            while(1) {
                diff = client->response.formattedHeader->data.length - index;
                if(diff == 0)
                    break;
                if((bytesSent = sendCallback(fusion, &client->response.formattedHeader->bytes[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff))) == PIKA_HTTP_ERROR_SEND)
                    return pika_false;
                index += bytesSent;
            }
            break;
        case PIKA_HTTP_MESSAGE:
            while(1) {
                diff = client->response.message.size - index;
                if(diff == 0)
                    break;
                if((bytesSent = sendCallback(fusion, &client->response.message.buffer[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff))) == PIKA_HTTP_ERROR_SEND)
                    return pika_false;
                index += bytesSent;
            }
            break;
        default:
            return pika_false;
    }

    return pika_true;
}

pika_bool accepted_http_client_send(AcceptedHttpClient *client, pika_byte mode) {
    HttpSendFusion fusion;
    fusion.sock = client->sock->socket;
    return __accepted_http_client_send(client, mode, &fusion, __http_send_callback);
}

#if PIKA_SSL > 0
pika_bool accepted_https_client_send(AcceptedHttpsClient *client, pika_byte mode) {
    HttpSendFusion fusion;
    fusion.ssl = client->ssl;
    return __accepted_http_client_send(client->client, mode, &fusion, __https_send_callback);
}
#endif

void free_accepted_http_client(AcceptedHttpClient *client) {
    clear_http_response(&client->response);
    clear_http_request(&client->request);
    free_accepted_client_socket(client->sock);
    free(client);
}

HttpHeader *http_header_parse(BytesBuffer *data, HttpHeader **start, pika_byte *getOrPost, String_UTF8 *url) {
    *start = NULL;
    HttpHeader *header;
    http_add_header_element(&header, "", "");
    HttpHeader **lastElement = &header->next;

    pika_byte *ptr = data->buffer, *ptrKey = data->buffer, *ptrValue = NULL;
    pika_bool loop = pika_true;
    pika_bool startFound = pika_false, found = pika_false;
    pika_bool b;
    while(loop) {
        while(1) {
            // On cherche le séparateur ':'
            if(*ptr == ':') {
                // Si on le trouve on le remplace par '\0'
                *ptr = '\0';
                ptr++;
                // On ignore les espaces
                while(*ptr == ' ')
                    ptr++;
                ptrValue = ptr;
                while(*ptr != '\r' && *ptr != '\n')
                    ptr++;
                if(*ptr == '\0') {
                    loop = pika_false;
                    break;
                }
                *ptr = '\0';
                ptr++;
                while(*ptr == '\r' || *ptr == '\n')
                    ptr++;
                if(startFound)
                    found = pika_true;
                break;
            // Si on trouve un saut de ligne
            }else if(*ptr == '\r' || *ptr =='\n') {
                startFound = pika_true;
                *ptr = '\0';
                ptrValue = ptr;
                ptr++;
                while(*ptr == '\r' || *ptr == '\n')
                    ptr++;
                break;
            }else if(*ptr == '\0')
                goto accepted_http_client_parse_header_ignore;
            ptr++;
        }
        http_add_header_element(lastElement, ptrKey, ptrValue);
        b = pika_false;
        if(strutf8_start_with((*lastElement)->key, "GET")) {
            b = pika_true;
            if(getOrPost)
                *getOrPost = PIKA_HTTP_GET;
        }else if(strutf8_start_with(((*lastElement)->key), "POST")) {
            b = pika_true;
            if(getOrPost)
                *getOrPost = PIKA_HTTP_POST;
        }
        if(b && url != NULL) {
            uchar *waw = (*lastElement)->key->bytes;
            while(*waw != '\0' && *waw != ' ')
                waw++;
            while(*waw != '\0' && *waw == ' ')
                waw++;
            uchar *www = waw;
            while(*waw != '\0' && *waw != ' ')
                waw++;
            uchar tempWaw = *waw;
            *waw = '\0';
            array_char_to_strutf8(www, url);
            *waw = tempWaw;
        }
        if(found) {
            *start = *lastElement;
            startFound = pika_false;
            found      = pika_false;
        }
        lastElement = &(*lastElement)->next;
        ptrKey = ptr;
    }
accepted_http_client_parse_header_ignore:

    return header;
}

typedef struct HttpReceiveFusion {
    pika_socket sock;
    #if PIKA_SSL > 0 
    SSL *ssl;
    #endif
} HttpReceiveFusion;

int __http_receive_callback(HttpReceiveFusion *fusion, uchar *buff, int length) {
    return recv(fusion->sock, buff, length, 0);
}

#if PIKA_SSL > 0
int __https_receive_callback(HttpReceiveFusion *fusion, uchar *buff, int length) {
    return SSL_read(fusion->ssl, buff, length);
}
#endif

pika_byte __http_receive(BytesBuffer *dest, BytesBuffer *destMessage, HttpHeader **header, pika_byte *getOrPost, String_UTF8 *url, HttpReceiveFusion *fusion, int (*recvCallback)(HttpReceiveFusion *, uchar *, int)) {
    int bytesRead;
    
    uchar buffer[PIKA_BUFF_SIZE];
    ulonglong tempLength = 0;
    uchar *search;
    pika_bool pass = pika_false;

    // On lit d'abord le header
    ulonglong index = 0;
    while(1) {
        if((bytesRead = recvCallback(fusion, buffer, PIKA_BUFF_SIZE)) == PIKA_HTTP_ERROR_RECV)
            return PIKA_HTTP_ERROR_RECEIVE;
        else if(bytesRead == 0)
            return PIKA_HTTP_CONNECTION_CLOSED;
        dest->size += bytesRead;
        dest->buffer = (uchar *) realloc(dest->buffer, dest->size * sizeof(uchar) + sizeof(uchar));
        if(pass)
            search = &dest->buffer[tempLength - 3];
        else {
            pass = pika_true;
            search = &dest->buffer[tempLength];
        }
        memcpy(&dest->buffer[tempLength], buffer, bytesRead);
        dest->buffer[dest->size] = '\0';
        tempLength = dest->size;
        
        if((search = str_search_array(search, "\r\n\r\n")) != NULL) {
            index = search - dest->buffer + 4;
            dest->buffer = (uchar *) realloc(dest->buffer, dest->size * sizeof(uchar) + sizeof(uchar) * 2);
            memcpy(&dest->buffer[index + 1], &dest->buffer[index], dest->size + 1 - index);
            dest->buffer[index] = '\0';
            break;
        }
    }
    HttpHeader *start, *found = NULL;
    *header = http_header_parse(dest, &start, getOrPost, url);
    while(start != NULL) {
        strutf8_to_lower(start->key);
        if(strutf8_equals(start->key, "content-length"))
            found = start;
        start = start->next;
    }

    if(found != NULL) {
        destMessage->size = strutf8_to_ulonglong(found->value);
        if(destMessage->size > 0) {
            destMessage->buffer = (uchar *) malloc(destMessage->size * sizeof(uchar) + sizeof(uchar));
            destMessage->buffer[destMessage->size] = '\0';
            ulonglong current = 0;
            ulonglong remain;
            if(dest->size > index) {
                remain = destMessage->size - (dest->size - index);
                memcpy(destMessage->buffer, &dest->buffer[index + 1], destMessage->size * sizeof(uchar));
            }else
                remain = destMessage->size;
            if(remain > 0) {
                while(1) {
                    bytesRead = recvCallback(fusion, &destMessage->buffer[current], (remain > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : remain));
                    if(bytesRead == PIKA_HTTP_ERROR_RECV)
                        return PIKA_HTTP_ERROR_RECEIVE;
                    else if(bytesRead == 0)
                        return PIKA_HTTP_CONNECTION_CLOSED;
                    current += bytesRead;
                    dest->size += bytesRead;
                    remain -= bytesRead;
                    if(current == destMessage->size)
                        break;
                }
            }
        }
    }

    return PIKA_NO_ERROR;
}

pika_byte http_receive(BytesBuffer *dest, BytesBuffer *destMessage, HttpHeader **header, pika_byte *getOrPost, String_UTF8 *url, pika_socket sock) {
    HttpReceiveFusion fusion;
    fusion.sock = sock;
    return __http_receive(dest, destMessage, header, getOrPost, url, &fusion, __http_receive_callback);
}

#if PIKA_SSL > 0
pika_byte https_receive(BytesBuffer *dest, BytesBuffer *destMessage, HttpHeader **header, pika_byte *getOrPost, String_UTF8 *url, SSL *ssl) {
    HttpReceiveFusion fusion;
    fusion.ssl = ssl;
    return __http_receive(dest, destMessage, header, getOrPost, url, &fusion, __https_receive_callback);
}
#endif

LinkedList_String_UTF8_Pair *accepted_http_client_parse_post_message(AcceptedHttpClient *client) {
    LinkedList_String_UTF8_Pair *current = NULL;
    LinkedList_String_UTF8_Pair *last = NULL;
    LinkedList_String_UTF8_Pair *list = NULL;
    pika_bool b = pika_false;

    ulonglong i = 0;
    uchar *lastPtr = client->request.message.buffer;
    uchar *ptrValue  = client->request.message.buffer;
    uchar *replace = NULL;
    while(1) {
        if(client->request.message.buffer[i] == '=') {
            replace = &client->request.message.buffer[i];
            *replace = '\0';
            ptrValue = replace + 1;
        }else if(client->request.message.buffer[i] == '&') {
            client->request.message.buffer[i] = '\0';
            current = (LinkedList_String_UTF8_Pair *) malloc(sizeof(LinkedList_String_UTF8_Pair));

            if(b)
                last->next = current;
            else {
                b = pika_true;
                list = current;
            }

            last = current;
            current->next = NULL;
            current->pair = strutf8_pair(lastPtr, ptrValue);
            strutf8_decode_url(current->pair->value);
            client->request.message.buffer[i] = '&';
            *replace = '=';
            lastPtr = &client->request.message.buffer[i + 1];
        }
        if(i == client->request.message.size) {
            current = (LinkedList_String_UTF8_Pair *) malloc(sizeof(LinkedList_String_UTF8_Pair));
            if(b)
                last->next = current;
                else
                    list = current;
            current->next = NULL;
            current->pair = strutf8_pair(lastPtr, ptrValue);
            strutf8_decode_url(current->pair->value);
            break;
        }
        i++;
    }

    return list;
}

HttpHeader *http_header_find(HttpHeader *first, const uchar *key) {
    while(first != NULL) {
        if(strutf8_equals(first->key, key))
            break;
        first = first->next;
    }
    return first;
}

#if PIKA_SSL > 0
void init_openssl() {
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    SSL_library_init();
}


pika_bool create_https_server(HttpsServer *serverDest, const uchar *port, int backlog, const uchar *cacertPath, const uchar *certPath, const uchar *keyPath) {
    if(!create_http_server(&serverDest->server, port, backlog))
        return pika_false;
    serverDest->ctx = SSL_CTX_new(TLS_server_method());
    if(!serverDest->ctx) {
        fprintf(stderr, "Erreur: SSL_CTX_new\n");
	    ERR_print_errors_fp(stderr);
        free_http_server(serverDest->server);
        return pika_false;
    }

    
    // On définie la version minimum et maximum du protocole utilisé
    SSL_CTX_set_min_proto_version(serverDest->ctx, TLS1_VERSION);
    SSL_CTX_set_max_proto_version(serverDest->ctx, TLS1_3_VERSION);
    
    SSL_CTX_set_verify(serverDest->ctx, SSL_VERIFY_NONE, NULL);
    //SSL_CTX_set_verify_depth(serverDest->ctx, 4);

    SSL_CTX_set_options(serverDest->ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION);
    SSL_CTX_set_client_CA_list(serverDest->ctx, SSL_load_client_CA_file(cacertPath));
    if(!SSL_CTX_load_verify_locations(serverDest->ctx, cacertPath, NULL)) {
        fprintf(stderr, "Erreur: SSL_CTX_load_verify_locations\n");
        ERR_print_errors_fp(stderr);
        free_http_server(serverDest->server);
        return pika_false;
    }

    SSL_CTX_set_cipher_list(serverDest->ctx, "ALL:eNULL");

    if(SSL_CTX_use_certificate_chain_file(serverDest->ctx, certPath) <= 0) {
        fprintf(stderr, "Erreur: SSL_CTX_use_certificate_chain_file\n");
        ERR_print_errors_fp(stderr);
        free_http_server(serverDest->server);
        return pika_false;
    }

    if(SSL_CTX_use_PrivateKey_file(serverDest->ctx, keyPath, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "Erreur: SSL_CTX_use_PrivateKey_file\n");
        ERR_print_errors_fp(stderr);
        free_http_server(serverDest->server);
        return pika_false;
    }

    if(!SSL_CTX_check_private_key(serverDest->ctx)) {
        fprintf(stderr, "Erreur: SSL_CTX_check_private_key\n");
        ERR_print_errors_fp(stderr);
        free_http_server(serverDest->server);
        return pika_false;
    }

    return pika_true;
}

void free_https_server(HttpsServer *server) {
    free_http_server(server->server);
    SSL_CTX_free(server->ctx);
}

void free_accepted_https_client(AcceptedHttpsClient *client) {
    SSL_shutdown(client->ssl);
    SSL_free(client->ssl);
    free_accepted_http_client(client->client);
    free(client);
}
#endif

void create_http_response(HttpResponse *response) {
    response->formattedHeader = strutf8("");
    response->header = NULL;
    response->nextHeaderValue = &response->header;
    response->message.buffer = NULL;
    response->message.size = 0;
    response->receivedData.buffer = NULL;
    response->receivedData.size = 0;
    response->status = "200";
}

void create_http_request(HttpRequest *request) {
    request->formattedHeader = strutf8("");
    request->header = NULL;
    request->nextHeaderValue = &request->header;
    request->message.buffer = NULL;
    request->message.size = 0;
    request->receivedData.buffer = NULL;
    request->receivedData.size = 0;
    request->method = PIKA_HTTP_GET;
    request->url = strutf8("");
}

void clear_http_response(HttpResponse *response) {
    free_strutf8(response->formattedHeader);
    free_http_header(response->header);
    free(response->receivedData.buffer);
    free(response->message.buffer);
}

void clear_http_request(HttpRequest *request) {
    free_strutf8(request->formattedHeader);
    free_strutf8(request->url);
    free_http_header(request->header);
    free(request->receivedData.buffer);
    free(request->message.buffer);
}
