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

    http_add_header_element(&client->data.headerToSend, "host", hostname);
    client->data.lastHeaderToSend = &client->data.headerToSend->next;
    http_add_header_element(client->data.lastHeaderToSend, "user-agent", "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0");
    client->data.lastHeaderToSend = &(*client->data.lastHeaderToSend)->next;
    http_add_header_element(client->data.lastHeaderToSend, "accept", "*/*");
    client->data.lastHeaderToSend = &(*client->data.lastHeaderToSend)->next;
    http_add_header_element(client->data.lastHeaderToSend, "accept-language", "en-US");
    client->data.lastHeaderToSend = &(*client->data.lastHeaderToSend)->next;
    http_add_header_element(client->data.lastHeaderToSend, "connection", "close");
    client->data.lastHeaderToSend = &(*client->data.lastHeaderToSend)->next;

    client->data.dataReceived       = NULL;
    client->data.dataReceivedLength = 0;

    client->data.receivedHeader     = NULL;
    client->data.lastReceivedHeader = NULL;

    client->data.receivedMessage       = NULL;
    client->data.receivedMessageLength = 0;

    client->data.headerToSend     = NULL;
    client->data.lastHeaderToSend = NULL;

    client->data.messageToSend       = NULL;
    client->data.messageToSendLength = 0;

    client->data.finalHeaderToSend = strutf8("");

    client->data.getOrPost = PIKA_HTTP_GET;

    return pika_true;
}

AcceptedHttpClient *http_server_accept(HttpServer *server, ulonglong requestMessageMaxLength) {
    AcceptedHttpClient *client = (AcceptedHttpClient *) malloc(sizeof(AcceptedHttpClient));
    
    client->data.dataReceived       = NULL;
    client->data.dataReceivedLength = 0;

    client->data.receivedHeader     = NULL;
    client->data.lastReceivedHeader = NULL;

    client->data.receivedMessage       = NULL;
    client->data.receivedMessageLength = 0;

    client->data.headerToSend     = NULL;
    client->data.lastHeaderToSend = NULL;

    client->data.messageToSend       = NULL;
    client->data.messageToSendLength = 0;

    client->data.finalHeaderToSend = strutf8("");

    client->sock = server_socket_accept(server);

    client->dataReceivedMaxLength = requestMessageMaxLength;

    if(client->sock == NULL) {
        free(client);
        return NULL;
    }

    http_add_header_element(&client->data.headerToSend, "connection", "close");
    client->data.lastHeaderToSend = &client->data.headerToSend->next;

    return client;
}

void free_http_client(HttpClient *client) {
    free_http_data(&client->data);
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

void http_data_write_final_header(HttpData *data, const uchar *url, pika_byte requestOrResponse, uchar *statusCode) {
    uchar *httpVersion = "HTTP/1.1";
    if(requestOrResponse == PIKA_HTTP_REQUEST) {
        switch(data->getOrPost) {
            default:
                array_char_to_strutf8("GET ", data->finalHeaderToSend);
                break;
            case PIKA_HTTP_POST:
                array_char_to_strutf8("POST ", data->finalHeaderToSend);
                break;
        }
        strutf8_add_char_array(data->finalHeaderToSend, url);
        strutf8_add_char_array(data->finalHeaderToSend, " ");
        strutf8_add_char_array(data->finalHeaderToSend, httpVersion);
    }else {
        array_char_to_strutf8(httpVersion, data->finalHeaderToSend);
        strutf8_add_char_array(data->finalHeaderToSend, " ");
        strutf8_add_char_array(data->finalHeaderToSend, statusCode);
    }
    strutf8_add_char_array(data->finalHeaderToSend, "\r\n");
    HttpHeader *current = data->headerToSend;
    while(current != NULL) {
        strutf8_add_char_array(data->finalHeaderToSend, current->key->bytes);
        strutf8_add_char_array(data->finalHeaderToSend, ":");
        strutf8_add_char_array(data->finalHeaderToSend, current->value->bytes);
        strutf8_add_char_array(data->finalHeaderToSend, "\r\n");
        current = current->next;
    }
    if(data->messageToSendLength > 0) {
        uchar lengthBuff[256];
        ulonglong_to_char_array(data->messageToSendLength, lengthBuff);
        strutf8_add_char_array(data->finalHeaderToSend, "content-length");
        strutf8_add_char_array(data->finalHeaderToSend, ":");
        strutf8_add_char_array(data->finalHeaderToSend, lengthBuff);
        strutf8_add_char_array(data->finalHeaderToSend, "\r\n");
    }
    strutf8_add_char_array(data->finalHeaderToSend, "\r\n");
}

pika_bool http_client_send(HttpClient *client, pika_byte mode) {
    ulonglong index = 0;
    int bytesSent;
    ulonglong diff;
    switch(mode) {
        case PIKA_HTTP_HEADER:
            while(1) {
                diff = client->data.finalHeaderToSend->data.length - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock.socket, &client->data.finalHeaderToSend->bytes[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff), 0)) == PIKA_HTTP_ERROR_SEND)
                    return pika_false;
                index += bytesSent;
            }
            break;
        case PIKA_HTTP_MESSAGE:
            while(1) {
                diff = client->data.messageToSendLength - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock.socket, &client->data.messageToSend[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff), 0)) == PIKA_HTTP_ERROR_SEND)
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
    ulonglong index = 0;
    int bytesSent;
    ulonglong diff;
    switch(mode) {
        case PIKA_HTTP_HEADER:
            while(1) {
                diff = client->data.finalHeaderToSend->data.length - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock->socket, &client->data.finalHeaderToSend->bytes[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff), 0)) == PIKA_HTTP_ERROR_SEND)
                    return pika_false;
                index += bytesSent;
            }
            break;
        case PIKA_HTTP_MESSAGE:
            while(1) {
                diff = client->data.messageToSendLength - index;
                if(diff == 0)
                    break;
                if((bytesSent = send(client->sock->socket, &client->data.messageToSend[index], (diff > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : diff), 0)) == PIKA_HTTP_ERROR_SEND)
                    return pika_false;
                index += bytesSent;
            }
            break;
        default:
            return pika_false;
    }

    return pika_true;
}

void free_accepted_http_client(AcceptedHttpClient *client) {
    free_http_data(&client->data);
    free_accepted_client_socket(client->sock);
    free(client);
}

HttpHeader *http_data_parse_header(HttpData *data, HttpHeader **start) {
    *start = NULL;
    HttpHeader *header;
    http_add_header_element(&header, "", "");
    HttpHeader **lastElement = &header->next;

    uchar *ptr = data->dataReceived, *ptrKey = data->dataReceived, *ptrValue = NULL;
    pika_bool loop = pika_true;
    pika_bool startFound = pika_false, found = pika_false;
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

void free_http_data(HttpData *data) {
    free(data->dataReceived);
    free(data->receivedMessage);
    free(data->messageToSend);
    free_http_header(data->receivedHeader);
    free_http_header(data->headerToSend);
    free_strutf8(data->finalHeaderToSend);
}

pika_byte http_receive(HttpData *data, pika_socket sock) {
    data->dataReceived       = NULL;
    data->dataReceivedLength = 0;
    
    data->receivedMessage       = NULL;
    data->receivedMessageLength = 0;

    int bytesRead;
    
    uchar buffer[PIKA_BUFF_SIZE];
    ulonglong tempLength = 0;
    uchar *search;
    pika_bool pass = pika_false;

    // On lit d'abord le header
    ulonglong index = 0;
    while(1) {
        if((bytesRead = recv(sock, buffer, PIKA_BUFF_SIZE, 0)) == SOCKET_ERROR)
            return PIKA_HTTP_ERROR_RECEIVE;
        else if(bytesRead == 0)
            break;
        data->dataReceivedLength += bytesRead;
        data->dataReceived = (uchar *) realloc(data->dataReceived, data->dataReceivedLength * sizeof(uchar) + sizeof(uchar));
        if(pass)
            search = &data->dataReceived[tempLength - 3];
        else {
            pass = pika_true;
            search = &data->dataReceived[tempLength];
        }
        memcpy(&data->dataReceived[tempLength], buffer, bytesRead);
        data->dataReceived[data->dataReceivedLength] = '\0';
        tempLength = data->dataReceivedLength;
        
        if((search = str_search_array(search, "\r\n\r\n")) != NULL) {
            index = search - data->dataReceived + 4;
            data->dataReceived = (uchar *) realloc(data->dataReceived, data->dataReceivedLength * sizeof(uchar) + sizeof(uchar) * 2);
            memcpy(&data->dataReceived[index + 1], &data->dataReceived[index], data->dataReceivedLength + 1 - index);
            data->dataReceived[index] = '\0';
            break;
        }
    }
    HttpHeader *start, *found = NULL;
    HttpHeader *header = http_data_parse_header(data, &start);
    while(start != NULL) {
        strutf8_to_lower(start->key);
        if(strutf8_equals(start->key, "content-length"))
            found = start;
        start = start->next;
    }

    if(found != NULL) {
        data->receivedMessageLength = strutf8_to_ulonglong(found->value);
        if(data->receivedMessageLength > 0) {
            data->receivedMessage = (uchar *) malloc(data->receivedMessageLength * sizeof(uchar) + sizeof(uchar));
            data->receivedMessage[data->receivedMessageLength] = '\0';
            ulonglong current = 0;
            ulonglong remain;
            if(data->dataReceivedLength > index)
                remain = data->receivedMessageLength - (data->dataReceivedLength - index);
            else
                remain = data->receivedMessageLength;
            if(remain > 0) {
                while(1) {
                    bytesRead = recv(sock, &data->receivedMessage[current], (remain > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : remain), 0);
                    if(bytesRead == SOCKET_ERROR)
                        return PIKA_HTTP_ERROR_RECEIVE;
                    else if(bytesRead == 0)
                        break;
                    current += bytesRead;
                    data->dataReceivedLength += bytesRead;
                    remain -= bytesRead;
                    if(current == data->receivedMessageLength)
                        break;
                }
            }else
                memcpy(data->receivedMessage, &data->dataReceived[index + 1], data->receivedMessageLength * sizeof(uchar));
        }
    }

    return PIKA_NO_ERROR;
}
