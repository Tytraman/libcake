#include "../error.h"
#include "../socket.h"

#include <stdio.h>
#include <openssl/err.h>

const char *cake_get_ssl_error(int errorCode) {
    switch(errorCode) {
        default:
            return "";
        case SSL_ERROR_NONE:
            return "SSL_ERROR_NONE";
        case SSL_ERROR_ZERO_RETURN:
            return "SSL_ERROR_ZERO_RETURN";
        case SSL_ERROR_WANT_READ:
            return "SSL_ERROR_WANT_READ";
        case SSL_ERROR_WANT_WRITE:
            return "SSL_ERROR_WANT_WRITE";
        case SSL_ERROR_WANT_CONNECT:
            return "SSL_ERROR_WANT_CONNECT";
        case SSL_ERROR_WANT_ACCEPT:
            return "SSL_ERROR_WANT_ACCEPT";
        case SSL_ERROR_WANT_X509_LOOKUP:
            return "SSL_ERROR_WANT_X509_LOOKUP";
        case SSL_ERROR_WANT_ASYNC:
            return "SSL_ERROR_WANT_ASYNC";
        case SSL_ERROR_WANT_ASYNC_JOB:
            return "SSL_ERROR_WANT_ASYNC_JOB";
        case SSL_ERROR_WANT_CLIENT_HELLO_CB:
            return "SSL_ERROR_WANT_CLIENT_HELLO_CB";
        case SSL_ERROR_SYSCALL:
            return "SSL_ERROR_SYSCALL";
        case SSL_ERROR_SSL:
            return "SSL_ERROR_SSL";
    }
}

void cake_print_socket_error(cake_byte errorFrom, ulong errorCode) {
    const char *error, *desc = "";
    switch(errorFrom) {
        default: {
            fprintf(stderr, "Aucune erreur liée.\n");
        } return;
        case CAKE_SOCKET_ERROR_FROM_GETADDRINFO: {
            error = "getaddrinfo";
        } break;
        case CAKE_SOCKET_ERROR_FROM_SOCKET: {
            error = "socket";
        } break;
        case CAKE_SOCKET_ERROR_FROM_CONNECT: {
            error = "connect";
        } break;
        case CAKE_SOCKET_ERROR_FROM_BIND: {
            error = "bind";
        } break;
        case CAKE_SOCKET_ERROR_FROM_LISTEN: {
            error = "listen";
        } break;
        case CAKE_SOCKET_ERROR_FROM_ACCEPT: {
            error = "accept";
        } break;
        case CAKE_SOCKET_ERROR_FROM_RECV: {
            error = "recv";
        } break;
        case CAKE_SOCKET_ERROR_FROM_SEND: {
            error = "send";
        } break;
        case CAKE_SOCKET_ERROR_FROM_SSL_SET_FD: {
            error = "SSL_set_fd";
            desc = cake_get_ssl_error(errorCode);
        } break;
        case CAKE_SOCKET_ERROR_FROM_SSL_CONNECT: {
            error = "SSL_connect";
            desc = cake_get_ssl_error(errorCode);
        } break;
        case CAKE_SOCKET_ERROR_FROM_SSL_WRITE_EX: {
            error = "SSL_write_ex";
            desc = cake_get_ssl_error(errorCode);
        } break;
        case CAKE_SOCKET_ERROR_FROM_SSL_READ_EX: {
            error = "SSL_read_ex";
            desc = cake_get_ssl_error(errorCode);
        } break;
    }
    fprintf(stderr, "[CAKE] [ERREUR] %s: %s\n", error, desc);
}
