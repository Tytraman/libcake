#include "../socket.h"
#include "../utf8.h"

#include <stdio.h>
#include <stdlib.h>

cake_bool cake_create_client_socket(Cake_ClientSocket *sock, const char *hostname, const char *port, cake_byte ipMode) {
    sock->errorFrom = CAKE_NO_ERROR;
    struct addrinfo hints = { 0 };
    hints.ai_family   = ipMode;
    hints.ai_socktype = SOCK_STREAM;

    sock->errorCode = getaddrinfo(hostname, port, &hints, &sock->address);
    if(sock->errorCode != 0) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_GETADDRINFO;
        return cake_false;
    }

    sock->socket = socket(sock->address->ai_family, sock->address->ai_socktype, sock->address->ai_protocol);
    if(sock->socket == CAKE_SOCKET_BAD_SOCKET) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_SOCKET;
        sock->errorCode = cake_socket_get_last_error_code();
        freeaddrinfo(sock->address);
        sock->address = NULL;
        return cake_false;
    }

    return cake_true;
}

cake_bool cake_client_socket_connect(Cake_ClientSocket *sock) {
    sock->errorFrom = CAKE_NO_ERROR;
    if(connect(sock->socket, sock->address->ai_addr, sock->address->ai_addrlen) != 0) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_CONNECT;
        sock->errorCode = cake_socket_get_last_error_code();
        return cake_false;
    }
    return cake_true;
}

void cake_free_client_socket(Cake_ClientSocket *sock) {
    cake_close_socket(sock->socket);
    freeaddrinfo(sock->address);
}

cake_bool cake_create_server_socket(Cake_ServerSocket *sock, const char *port, cake_byte ipMode, int backlog) {
    sock->errorFrom = CAKE_NO_ERROR;
    struct addrinfo hints = { 0 }, *addr = { 0 };
    hints.ai_family   = ipMode;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    sock->errorCode = getaddrinfo(NULL, port, &hints, &addr);
    if(sock->errorCode != 0) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_GETADDRINFO;
        return cake_false;
    }

    sock->socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if(sock->socket == CAKE_SOCKET_BAD_SOCKET) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_SOCKET;
        sock->errorCode = cake_socket_get_last_error_code();
        freeaddrinfo(addr);
        return cake_false;
    }

    if(bind(sock->socket, addr->ai_addr, addr->ai_addrlen) == CAKE_SOCKET_ERROR) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_BIND;
        sock->errorCode = cake_socket_get_last_error_code();
        freeaddrinfo(addr);
        cake_close_socket(sock->socket);
        return cake_false;
    }
    freeaddrinfo(addr);

    if(listen(sock->socket, backlog) == CAKE_SOCKET_ERROR) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_LISTEN;
        sock->errorCode = cake_socket_get_last_error_code();
        cake_close_socket(sock->socket);
        return cake_false;
    }

    return cake_true;
}

Cake_AcceptedClientSocket *cake_server_socket_accept(Cake_ServerSocket *sock) {
    Cake_AcceptedClientSocket *client = (Cake_AcceptedClientSocket *) malloc(sizeof(Cake_AcceptedClientSocket));
    int length = sizeof(client->addr);

    client->socket = accept(sock->socket, (struct sockaddr *) &client->addr, &length);
    if(client->socket == CAKE_SOCKET_BAD_SOCKET) {
        free(client);
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_ACCEPT;
        sock->errorCode = cake_socket_get_last_error_code();
        return NULL;
    }

    return client;
}

void cake_free_accepted_client_socket(Cake_AcceptedClientSocket *sock) {
    cake_close_socket(sock->socket);
    free(sock);
}
