#include "../socket.h"
#include "../utf8.h"

#include <stdio.h>
#include <stdlib.h>

pika_bool create_client_socket(ClientSocket *sock, const char *hostname, const char *port, pika_byte ipMode) {
    sock->errorFrom = PIKA_NO_ERROR;
    struct addrinfo hints = { 0 };
    hints.ai_family   = ipMode;
    hints.ai_socktype = SOCK_STREAM;

    sock->errorCode = getaddrinfo(hostname, port, &hints, &sock->address);
    if(sock->errorCode != 0) {
        sock->errorFrom = PIKA_SOCKET_ERROR_FROM_GETADDRINFO;
        return pika_false;
    }

    sock->socket = socket(sock->address->ai_family, sock->address->ai_socktype, sock->address->ai_protocol);
    if(sock->socket == INVALID_SOCKET) {
        sock->errorFrom = PIKA_SOCKET_ERROR_FROM_SOCKET;
        sock->errorCode = WSAGetLastError();
        freeaddrinfo(sock->address);
        sock->address = NULL;
        return pika_false;
    }

    return pika_true;
}

pika_bool client_socket_connect(ClientSocket *sock) {
    sock->errorFrom = PIKA_NO_ERROR;
    if(connect(sock->socket, sock->address->ai_addr, sock->address->ai_addrlen) != 0) {
        sock->errorFrom = PIKA_SOCKET_ERROR_FROM_CONNECT;
        sock->errorCode = WSAGetLastError();
        return pika_false;
    }
    return pika_true;
}

void free_client_socket(ClientSocket *sock) {
    closesocket(sock->socket);
    freeaddrinfo(sock->address);
}
