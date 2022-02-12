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

pika_bool create_server_socket(ServerSocket *sock, const char *port, pika_byte ipMode, int backlog) {
    sock->errorFrom = PIKA_NO_ERROR;
    struct addrinfo hints = { 0 }, *addr = { 0 };
    hints.ai_family   = ipMode;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    sock->errorCode = getaddrinfo(NULL, port, &hints, &addr);
    if(sock->errorCode != 0) {
        sock->errorFrom = PIKA_SOCKET_ERROR_FROM_GETADDRINFO;
        return pika_false;
    }

    sock->socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if(sock->socket == INVALID_SOCKET) {
        sock->errorFrom = PIKA_SOCKET_ERROR_FROM_SOCKET;
        sock->errorCode = WSAGetLastError();
        freeaddrinfo(addr);
        return pika_false;
    }

    if(bind(sock->socket, addr->ai_addr, addr->ai_addrlen) == SOCKET_ERROR) {
        sock->errorFrom = PIKA_SOCKET_ERROR_FROM_BIND;
        sock->errorCode = WSAGetLastError();
        freeaddrinfo(addr);
        close_socket(sock->socket);
        return pika_false;
    }
    freeaddrinfo(addr);

    if(listen(sock->socket, backlog) == SOCKET_ERROR) {
        sock->errorFrom = PIKA_SOCKET_ERROR_FROM_LISTEN;
        sock->errorCode = WSAGetLastError();
        close_socket(sock->socket);
        return pika_false;
    }

    return pika_true;
}

AcceptedClientSocket *server_socket_accept(ServerSocket *sock) {
    AcceptedClientSocket *client = (AcceptedClientSocket *) malloc(sizeof(AcceptedClientSocket));
    int length = sizeof(client->addr);

    client->socket = accept(sock->socket, (struct sockaddr *) &client->addr, &length);
    if(client->socket == INVALID_SOCKET) {
        free(client);
        sock->errorFrom = PIKA_SOCKET_ERROR_FROM_ACCEPT;
        sock->errorCode = WSAGetLastError();
        return NULL;
    }

    return client;
}

void free_accepted_client_socket(AcceptedClientSocket *sock) {
    close_socket(sock->socket);
    free(sock);
}
