#include "../http.h"

#include <stdio.h>
#include <stdlib.h>

pika_bool create_http_client(HttpClient *client, const char *hostname, const char *port) {
    if(!create_client_socket(&client->sock, hostname, port, PIKA_IP_V4))
        return pika_false;

    if(!client_socket_connect(&client->sock)) {
        free_client_socket(&client->sock);
        return pika_false;
    }

    HttpHeader **lastHeader = http_add_header_element(&client->requestHeader, "host", hostname);
    lastHeader = http_add_header_element(lastHeader, "user-agent", "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0");
    lastHeader = http_add_header_element(lastHeader, "accept", "*/*");
    lastHeader = http_add_header_element(lastHeader, "accept-language", "en-US");
    lastHeader = http_add_header_element(lastHeader, "connection", "close");
    client->finalRequestHeader = strutf8("");
    client->requestMessage  = NULL;
    client->responseMessage = NULL;
    client->requestMessageLength  = 0;
    client->responseMessageLength = 0;
    client->getOrPost = PIKA_HTTP_GET;
    return pika_true;
}

void free_http_client(HttpClient *client) {
    free_http_header(client->requestHeader);
    free(client->requestMessage);
    free(client->responseMessage);
}

HttpHeader **http_add_header_element(HttpHeader **header, const uchar *key, const uchar *value) {
    *header = (HttpHeader *) malloc(sizeof(HttpHeader));
    (*header)->key = strutf8(key);
    (*header)->value = strutf8(value);
    (*header)->next = NULL;
    return &(*header)->next;
}

void free_http_header(HttpHeader *header) {
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

void http_client_write_final_header(HttpClient *client, const uchar *url) {
    switch(client->getOrPost) {
        default:
            array_char_to_strutf8("GET ", client->finalRequestHeader);
            break;
        case PIKA_HTTP_POST:
            array_char_to_strutf8("POST ", client->finalRequestHeader);
            break;
    }
    strutf8_add_char_array(client->finalRequestHeader, url);
    strutf8_add_char_array(client->finalRequestHeader, " HTTP/1.1\r\n");
    HttpHeader *current = client->requestHeader;
    while(current != NULL) {
        strutf8_add_char_array(client->finalRequestHeader, current->key->bytes);
        strutf8_add_char_array(client->finalRequestHeader, ":");
        strutf8_add_char_array(client->finalRequestHeader, current->value->bytes);
        strutf8_add_char_array(client->finalRequestHeader, "\r\n");
        current = current->next;
    }
    strutf8_add_char_array(client->finalRequestHeader, "\r\n");
}

pika_bool http_client_send(HttpClient *client, pika_byte mode) {
    ulonglong index = 0;
    int bytesSent;
    ulonglong diff;
    switch(mode) {
        case PIKA_HTTP_HEADER:
            while(1) {
                diff = client->finalRequestHeader->data.length - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock.socket, &client->finalRequestHeader->bytes[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff), 0)) == PIKA_HTTP_ERROR_SEND)
                    return pika_false;
                index += bytesSent;
            }
            break;
        case PIKA_HTTP_MESSAGE:
            while(1) {
                diff = client->requestMessageLength - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock.socket, &client->requestMessage[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff), 0)) == PIKA_HTTP_ERROR_SEND)
                    return pika_false;
                index += bytesSent;
            }
            break;
        default:
            return pika_false;
    }

    return pika_true;
}

pika_bool http_client_receive(HttpClient *client) {
    int bytesRead;
    client->responseMessageLength = 0;
    uchar buffer[PIKA_BUFF_SIZE];
    ulonglong tempLength = client->responseMessageLength;
    while(1) {
        if((bytesRead = recv(client->sock.socket, buffer, PIKA_BUFF_SIZE, 0)) == PIKA_HTTP_ERROR_RECEIVE)
            return pika_false;
        else if(bytesRead == 0)
            break;
        client->responseMessageLength += bytesRead;
        client->responseMessage = (uchar *) realloc(client->responseMessage, client->responseMessageLength * sizeof(uchar) + sizeof(uchar));
        memcpy(&client->responseMessage[tempLength], buffer, bytesRead);
        client->responseMessage[client->responseMessageLength] = '\0';
        tempLength = client->responseMessageLength;
    }
    return pika_true;
}
