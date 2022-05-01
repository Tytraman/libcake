#include "../http.h"

#include <stdio.h>
#include <stdlib.h>
#if CAKE_SSL > 0
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif


#if defined(CAKE_UNIX) || (defined(CAKE_WINDOWS) && CAKE_WIN_SOCK > 0)
cake_bool cake_create_http_client(Cake_HttpClient *client, const char *hostname, const char *port) {
    if(!cake_create_client_socket(&client->sock, hostname, port, CAKE_IP_V4))
        return cake_false;

    if(!cake_client_socket_connect(&client->sock)) {
        cake_free_client_socket(&client->sock);
        return cake_false;
    }

    cake_create_http_request(&client->request);
    cake_create_http_response(&client->response);

    cake_http_add_header_element(&client->request.header, "host", hostname);
    client->request.nextHeaderValue = &client->request.header->next;
    cake_http_add_header_element(client->request.nextHeaderValue, "user-agent", "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0");
    client->request.nextHeaderValue = &(*client->request.nextHeaderValue)->next;
    cake_http_add_header_element(client->request.nextHeaderValue, "accept", "*/*");
    client->request.nextHeaderValue = &(*client->request.nextHeaderValue)->next;
    cake_http_add_header_element(client->request.nextHeaderValue, "accept-language", "en-US");
    client->request.nextHeaderValue = &(*client->request.nextHeaderValue)->next;
    cake_http_add_header_element(client->request.nextHeaderValue, "connection", "close");
    client->request.nextHeaderValue = &(*client->request.nextHeaderValue)->next;

    return cake_true;
}

#if CAKE_SSL > 0
Cake_AcceptedHttpClient *cake_http_server_accept(Cake_HttpServer *server, ulonglong requestMessageMaxLength) {
    Cake_AcceptedHttpClient *client = (Cake_AcceptedHttpClient *) malloc(sizeof(Cake_AcceptedHttpClient));

    client->sock = cake_server_socket_accept(server);
    client->dataReceivedMaxLength = requestMessageMaxLength;

    if(client->sock == NULL) {
        free(client);
        return NULL;
    }

    cake_create_http_request(&client->request);
    cake_create_http_response(&client->response);

    cake_http_add_header_element(&client->response.header, "connection", "close");
    client->response.nextHeaderValue = &client->response.header->next;

    return client;
}

Cake_AcceptedHttpsClient *cake_https_server_accept(Cake_HttpsServer *server, ulonglong requestMessageMaxLength) {
    Cake_AcceptedHttpsClient *client = (Cake_AcceptedHttpsClient *) malloc(sizeof(Cake_AcceptedHttpsClient));
    client->client = cake_http_server_accept(&server->server, requestMessageMaxLength);
    if(client->client == NULL) {
        free(client);
        return NULL;
    }
    client->ssl = SSL_new(server->ctx);
    if(client->ssl == NULL) {
        fprintf(stderr, "Erreur: SSL_new\n");
        ERR_print_errors_fp(stderr);
        cake_free_accepted_http_client(client->client);
        free(client);
        return NULL;
    }

    if(!SSL_set_fd(client->ssl, (int) client->client->sock->socket)) {
        fprintf(stderr, "Erreur: SSL_set_fd\n");
        ERR_print_errors_fp(stderr);
        cake_free_accepted_http_client(client->client);
        free(client);
        return NULL;
    }

    if(SSL_accept(client->ssl) != 1) {
        fprintf(stderr, "Erreur: SSL_accept\n");
        ERR_print_errors_fp(stderr);
        cake_free_accepted_http_client(client->client);
        free(client);
        return NULL;
    }
    return client;
}
#endif

void cake_free_http_client(Cake_HttpClient *client) {
    cake_clear_http_response(&client->response);
    cake_clear_http_request(&client->request);
    cake_close_socket(client->sock.socket);
}

void cake_http_add_header_element(Cake_HttpHeader **header, const char *key, const char *value) {
    *header = (Cake_HttpHeader *) malloc(sizeof(Cake_HttpHeader));
    (*header)->key = cake_strutf8(key);
    (*header)->value = cake_strutf8(value);
    (*header)->next = NULL;
}

void cake_free_http_header(Cake_HttpHeader *header) {
    if(header != NULL) {
        Cake_HttpHeader *current = header;
        Cake_HttpHeader *next = current->next;
        while(current != NULL) {
            cake_free_strutf8(current->key);
            cake_free_strutf8(current->value);
            next = current->next;
            free(current);
            current = next;
        }
    }
}

void __http_format_header(Cake_String_UTF8 *dest, Cake_BytesBuffer *message, Cake_HttpHeader *header) {
    while(header != NULL) {
        cake_strutf8_add_char_array(dest, (cchar_ptr) header->key->bytes);
        cake_strutf8_add_char_array(dest, ":");
        cake_strutf8_add_char_array(dest, (cchar_ptr) header->value->bytes);
        cake_strutf8_add_char_array(dest, "\r\n");
        header = header->next;
    }
    if(message->size > 0) {
        char lengthBuff[256];
        cake_ulonglong_to_char_array(message->size, lengthBuff);
        cake_strutf8_add_char_array(dest, "content-length");
        cake_strutf8_add_char_array(dest, ":");
        cake_strutf8_add_char_array(dest, (cchar_ptr) lengthBuff);
        cake_strutf8_add_char_array(dest, "\r\n");
    }
    cake_strutf8_add_char_array(dest, "\r\n");
}

void cake_http_response_format(Cake_HttpResponse *response) {
    cake_char_array_to_strutf8("HTTP/1.1 ", response->formattedHeader);
    cake_strutf8_add_char_array(response->formattedHeader, (cchar_ptr) response->status);
    cake_strutf8_add_char_array(response->formattedHeader, "\r\n");
    __http_format_header(response->formattedHeader, &response->message, response->header);
}

void cake_http_request_format(Cake_HttpRequest *request) {
    if(request->method == CAKE_HTTP_GET)
        cake_char_array_to_strutf8("GET ", request->formattedHeader);
    else
        cake_char_array_to_strutf8("POST ", request->formattedHeader);
    cake_strutf8_add_char_array(request->formattedHeader, (const char *) request->url->bytes);
    cake_strutf8_add_char_array(request->formattedHeader, " HTTP/1.1\r\n");
    __http_format_header(request->formattedHeader, &request->message, request->header);
}

cake_bool cake_http_client_send(Cake_HttpClient *client, cake_byte mode) {
    ulonglong index = 0;
    int bytesSent;
    ulonglong diff;
    switch(mode) {
        case CAKE_HTTP_HEADER:
            while(1) {
                diff = client->request.formattedHeader->data.length - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock.socket, &client->request.formattedHeader->bytes[index], (diff > CAKE_BUFF_SIZE ? CAKE_BUFF_SIZE : diff), 0)) == CAKE_HTTP_ERROR_SEND)
                    return cake_false;
                index += bytesSent;
            }
            break;
        case CAKE_HTTP_MESSAGE:
            while(1) {
                diff = client->request.message.size - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock.socket, &client->request.message.buffer[index], (diff > CAKE_BUFF_SIZE ? CAKE_BUFF_SIZE : diff), 0)) == CAKE_HTTP_ERROR_SEND)
                    return cake_false;
                index += bytesSent;
            }
            break;
        default:
            return cake_false;
    }

    return cake_true;
}

typedef struct HttpSendFusion {
    cake_socket sock;
    #if CAKE_SSL > 0
    SSL *ssl;
    #endif
} HttpSendFusion;

int __http_send_callback(HttpSendFusion *fusion, uchar *buff, int length) {
    return send(fusion->sock, buff, length, 0);
}

#if CAKE_SSL > 0
int __https_send_callback(HttpSendFusion *fusion, uchar *buff, int length) {
    return SSL_write(fusion->ssl, buff, length);
}
#endif

cake_bool __accepted_http_client_send(Cake_AcceptedHttpClient *client, cake_byte mode, HttpSendFusion *fusion, int (*sendCallback)(HttpSendFusion *, uchar *, int)) {
    ulonglong index = 0;
    int bytesSent;
    ulonglong diff;
    switch(mode) {
        case CAKE_HTTP_HEADER:
            while(1) {
                diff = client->response.formattedHeader->data.length - index;
                if(diff == 0)
                    break;
                if((bytesSent = sendCallback(fusion, &client->response.formattedHeader->bytes[index], (diff > CAKE_BUFF_SIZE ? CAKE_BUFF_SIZE : diff))) == CAKE_HTTP_ERROR_SEND)
                    return cake_false;
                index += bytesSent;
            }
            break;
        case CAKE_HTTP_MESSAGE:
            while(1) {
                diff = client->response.message.size - index;
                if(diff == 0)
                    break;
                if((bytesSent = sendCallback(fusion, &client->response.message.buffer[index], (diff > CAKE_BUFF_SIZE ? CAKE_BUFF_SIZE : diff))) == CAKE_HTTP_ERROR_SEND)
                    return cake_false;
                index += bytesSent;
            }
            break;
        default:
            return cake_false;
    }

    return cake_true;
}

cake_bool cake_accepted_http_client_send(Cake_AcceptedHttpClient *client, cake_byte mode) {
    HttpSendFusion fusion;
    fusion.sock = client->sock->socket;
    return __accepted_http_client_send(client, mode, &fusion, __http_send_callback);
}

#if CAKE_SSL > 0
cake_bool cake_accepted_https_client_send(Cake_AcceptedHttpsClient *client, cake_byte mode) {
    HttpSendFusion fusion;
    fusion.ssl = client->ssl;
    return __accepted_http_client_send(client->client, mode, &fusion, __https_send_callback);
}
#endif

void cake_free_accepted_http_client(Cake_AcceptedHttpClient *client) {
    cake_clear_http_response(&client->response);
    cake_clear_http_request(&client->request);
    cake_free_accepted_client_socket(client->sock);
    free(client);
}

Cake_HttpHeader *cake_http_header_parse(Cake_BytesBuffer *data, Cake_HttpHeader **start, cake_byte *getOrPost, Cake_String_UTF8 *url) {
    *start = NULL;
    Cake_HttpHeader *header;
    cake_http_add_header_element(&header, "", "");
    Cake_HttpHeader **lastElement = &header->next;

    cake_byte *ptr = data->buffer, *ptrKey = data->buffer, *ptrValue = NULL;
    cake_bool loop = cake_true;
    cake_bool startFound = cake_false, found = cake_false;
    cake_bool b;
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
                    loop = cake_false;
                    break;
                }
                *ptr = '\0';
                ptr++;
                while(*ptr == '\r' || *ptr == '\n')
                    ptr++;
                if(startFound)
                    found = cake_true;
                break;
            // Si on trouve un saut de ligne
            }else if(*ptr == '\r' || *ptr =='\n') {
                startFound = cake_true;
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
        cake_http_add_header_element(lastElement, (const char *) ptrKey, (const char *) ptrValue);
        b = cake_false;
        if(cake_strutf8_start_with((*lastElement)->key, "GET")) {
            b = cake_true;
            if(getOrPost)
                *getOrPost = CAKE_HTTP_GET;
        }else if(cake_strutf8_start_with(((*lastElement)->key), "POST")) {
            b = cake_true;
            if(getOrPost)
                *getOrPost = CAKE_HTTP_POST;
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
            cake_char_array_to_strutf8((const char *) www, url);
            *waw = tempWaw;
        }
        if(found) {
            *start = *lastElement;
            startFound = cake_false;
            found      = cake_false;
        }
        lastElement = &(*lastElement)->next;
        ptrKey = ptr;
    }
accepted_http_client_parse_header_ignore:

    return header;
}

typedef struct HttpReceiveFusion {
    cake_socket sock;
    #if CAKE_SSL > 0 
    SSL *ssl;
    #endif
} HttpReceiveFusion;

int __http_receive_callback(HttpReceiveFusion *fusion, uchar *buff, int length) {
    return recv(fusion->sock, buff, length, 0);
}

#if CAKE_SSL > 0
int __https_receive_callback(HttpReceiveFusion *fusion, uchar *buff, int length) {
    return SSL_read(fusion->ssl, buff, length);
}
#endif

cake_byte __cake_http_receive(Cake_BytesBuffer *dest, Cake_BytesBuffer *destMessage, Cake_HttpHeader **header, cake_byte *getOrPost, Cake_String_UTF8 *url, HttpReceiveFusion *fusion, int (*recvCallback)(HttpReceiveFusion *, uchar *, int)) {
    int bytesRead;
    
    uchar buffer[CAKE_BUFF_SIZE];
    ulonglong tempLength = 0;
    uchar *search;
    cake_bool pass = cake_false;

    // On lit d'abord le header
    ulonglong index = 0;
    while(1) {
        if((bytesRead = recvCallback(fusion, buffer, CAKE_BUFF_SIZE)) == CAKE_HTTP_ERROR_RECV)
            return CAKE_HTTP_ERROR_RECEIVE;
        else if(bytesRead == 0)
            return CAKE_HTTP_CONNECTION_CLOSED;
        dest->size += bytesRead;
        dest->buffer = (uchar *) realloc(dest->buffer, dest->size * sizeof(uchar) + sizeof(uchar));
        if(pass)
            search = &dest->buffer[tempLength - 3];
        else {
            pass = cake_true;
            search = &dest->buffer[tempLength];
        }
        memcpy(&dest->buffer[tempLength], buffer, bytesRead);
        dest->buffer[dest->size] = '\0';
        tempLength = dest->size;
        
        if((search = cake_str_search_array((const char *) search, "\r\n\r\n")) != NULL) {
            index = search - dest->buffer + 4;
            dest->buffer = (uchar *) realloc(dest->buffer, dest->size * sizeof(uchar) + sizeof(uchar) * 2);
            memmove(dest->buffer + index + 1, dest->buffer + index, dest->size + 1 - index);
            dest->buffer[index] = '\0';
            break;
        }
    }
    Cake_HttpHeader *start, *found = NULL;
    *header = cake_http_header_parse(dest, &start, getOrPost, url);
    while(start != NULL) {
        cake_strutf8_to_lower(start->key);
        if(cake_strutf8_equals(start->key, "content-length"))
            found = start;
        start = start->next;
    }

    if(found != NULL) {
        destMessage->size = cake_strutf8_to_ulonglong(found->value);
        if(destMessage->size > 0) {
            destMessage->buffer = (uchar *) malloc(destMessage->size * sizeof(uchar) + sizeof(uchar));
            destMessage->buffer[destMessage->size] = '\0';
            ulonglong current = 0;
            ulonglong remain;
            if(dest->size > index) {
                remain = destMessage->size - (dest->size - index);
                memmove(destMessage->buffer, dest->buffer + index + 1, destMessage->size * sizeof(uchar));
            }else
                remain = destMessage->size;
            if(remain > 0) {
                while(1) {
                    bytesRead = recvCallback(fusion, &destMessage->buffer[current], (remain > CAKE_BUFF_SIZE ? CAKE_BUFF_SIZE : remain));
                    if(bytesRead == CAKE_HTTP_ERROR_RECV)
                        return CAKE_HTTP_ERROR_RECEIVE;
                    else if(bytesRead == 0)
                        return CAKE_HTTP_CONNECTION_CLOSED;
                    current += bytesRead;
                    dest->size += bytesRead;
                    remain -= bytesRead;
                    if(current == destMessage->size)
                        break;
                }
            }
        }
    }

    return CAKE_NO_ERROR;
}

cake_byte cake_http_receive(Cake_BytesBuffer *dest, Cake_BytesBuffer *destMessage, Cake_HttpHeader **header, cake_byte *getOrPost, Cake_String_UTF8 *url, cake_socket sock) {
    HttpReceiveFusion fusion;
    fusion.sock = sock;
    return __cake_http_receive(dest, destMessage, header, getOrPost, url, &fusion, __http_receive_callback);
}

#if CAKE_SSL > 0
cake_byte cake_https_receive(Cake_BytesBuffer *dest, Cake_BytesBuffer *destMessage, Cake_HttpHeader **header, cake_byte *getOrPost, Cake_String_UTF8 *url, SSL *ssl) {
    HttpReceiveFusion fusion;
    fusion.ssl = ssl;
    return __cake_http_receive(dest, destMessage, header, getOrPost, url, &fusion, __https_receive_callback);
}
#endif

Cake_LinkedList_String_UTF8_Pair *cake_accepted_http_client_parse_post_message(Cake_AcceptedHttpClient *client) {
    Cake_LinkedList_String_UTF8_Pair *current = NULL;
    Cake_LinkedList_String_UTF8_Pair *last = NULL;
    Cake_LinkedList_String_UTF8_Pair *list = NULL;
    cake_bool b = cake_false;

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
            current = (Cake_LinkedList_String_UTF8_Pair *) malloc(sizeof(Cake_LinkedList_String_UTF8_Pair));

            if(b)
                last->next = current;
            else {
                b = cake_true;
                list = current;
            }

            last = current;
            current->next = NULL;
            current->pair = cake_strutf8_pair((const char *) lastPtr, (const char *) ptrValue);
            cake_strutf8_decode_url(current->pair->value);
            client->request.message.buffer[i] = '&';
            *replace = '=';
            lastPtr = &client->request.message.buffer[i + 1];
        }
        if(i == client->request.message.size) {
            current = (Cake_LinkedList_String_UTF8_Pair *) malloc(sizeof(Cake_LinkedList_String_UTF8_Pair));
            if(b)
                last->next = current;
                else
                    list = current;
            current->next = NULL;
            current->pair = cake_strutf8_pair((const char *) lastPtr, (const char *) ptrValue);
            cake_strutf8_decode_url(current->pair->value);
            break;
        }
        i++;
    }

    return list;
}

Cake_HttpHeader *cake_http_header_find(Cake_HttpHeader *first, const char *key) {
    while(first != NULL) {
        if(cake_strutf8_equals(first->key, key))
            break;
        first = first->next;
    }
    return first;
}

#if CAKE_SSL > 0
void cake_init_openssl() {
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    SSL_library_init();
}


cake_bool cake_create_https_server(Cake_HttpsServer *serverDest, const uchar *port, int backlog, const uchar *cacertPath, const uchar *certPath, const uchar *keyPath) {
    if(!cake_create_http_server(&serverDest->server, port, backlog))
        return cake_false;
    serverDest->ctx = SSL_CTX_new(TLS_server_method());
    if(!serverDest->ctx) {
        fprintf(stderr, "Erreur: SSL_CTX_new\n");
	    ERR_print_errors_fp(stderr);
        cake_free_http_server(serverDest->server);
        return cake_false;
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
        cake_free_http_server(serverDest->server);
        return cake_false;
    }

    SSL_CTX_set_cipher_list(serverDest->ctx, "ALL:eNULL");

    if(SSL_CTX_use_certificate_chain_file(serverDest->ctx, certPath) <= 0) {
        fprintf(stderr, "Erreur: SSL_CTX_use_certificate_chain_file\n");
        ERR_print_errors_fp(stderr);
        cake_free_http_server(serverDest->server);
        return cake_false;
    }

    if(SSL_CTX_use_PrivateKey_file(serverDest->ctx, keyPath, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "Erreur: SSL_CTX_use_PrivateKey_file\n");
        ERR_print_errors_fp(stderr);
        cake_free_http_server(serverDest->server);
        return cake_false;
    }

    if(!SSL_CTX_check_private_key(serverDest->ctx)) {
        fprintf(stderr, "Erreur: SSL_CTX_check_private_key\n");
        ERR_print_errors_fp(stderr);
        cake_free_http_server(serverDest->server);
        return cake_false;
    }

    return cake_true;
}

void cake_free_https_server(Cake_HttpsServer *server) {
    cake_free_http_server(server->server);
    SSL_CTX_free(server->ctx);
}

void cake_free_accepted_https_client(Cake_AcceptedHttpsClient *client) {
    SSL_shutdown(client->ssl);
    SSL_free(client->ssl);
    cake_free_accepted_http_client(client->client);
    free(client);
}
#endif

void cake_create_http_response(Cake_HttpResponse *response) {
    response->formattedHeader = cake_strutf8("");
    response->header = NULL;
    response->nextHeaderValue = &response->header;
    response->message.buffer = NULL;
    response->message.size = 0;
    response->receivedData.buffer = NULL;
    response->receivedData.size = 0;
    response->status = (uchar *) "200";
}

void cake_create_http_request(Cake_HttpRequest *request) {
    request->formattedHeader = cake_strutf8("");
    request->header = NULL;
    request->nextHeaderValue = &request->header;
    request->message.buffer = NULL;
    request->message.size = 0;
    request->receivedData.buffer = NULL;
    request->receivedData.size = 0;
    request->method = CAKE_HTTP_GET;
    request->url = cake_strutf8("");
}

void cake_clear_http_response(Cake_HttpResponse *response) {
    cake_free_strutf8(response->formattedHeader);
    cake_free_http_header(response->header);
    free(response->receivedData.buffer);
    free(response->message.buffer);
}

void cake_clear_http_request(Cake_HttpRequest *request) {
    cake_free_strutf8(request->formattedHeader);
    cake_free_strutf8(request->url);
    cake_free_http_header(request->header);
    free(request->receivedData.buffer);
    free(request->message.buffer);
}
#endif
