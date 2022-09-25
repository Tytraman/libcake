#include "../socket.h"
#include "../strutf8.h"
#include "../alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <openssl/err.h>


/* ===== INTERNAL FUNCTIONS ===== */

cake_bool __cake_client_socket_recv(void *_s, char *_buffer, ulonglong len, ulonglong *_bytesReceived) {
    Cake_ClientSocket *client = (Cake_ClientSocket *) _s;
    *_bytesReceived = recv(client->socket, _buffer, len, 0);
    if(*_bytesReceived == SOCKET_ERROR) {
        client->errorFrom = CAKE_SOCKET_ERROR_FROM_RECV;
        client->errorCode = cake_socket_get_last_error_code();
        return cake_false;
    }
    return cake_true;
}

cake_bool __cake_tls_client_recv(void *_s, char *_buffer, ulonglong len, ulonglong *_bytesReceived) {
    Cake_TLSClient *client = (Cake_TLSClient *) _s;
    int ret = SSL_read_ex(client->ssl, _buffer, len, _bytesReceived);
    if(ret != 1) {
        client->clientSocket.errorFrom = CAKE_SOCKET_ERROR_FROM_SSL_READ_EX;
        client->clientSocket.errorCode = SSL_get_error(client->ssl, ret);
        return cake_false;
    }
    return cake_true;
}

cake_bool __cake_accepted_client_socket_recv(void *_s, char *_buffer, ulonglong len, ulonglong *_bytesReceived) {
    Cake_AcceptedClientSocket *client = (Cake_AcceptedClientSocket *) _s;
    *_bytesReceived = recv(client->sock, _buffer, len, 0);
    if(*_bytesReceived == SOCKET_ERROR) {
        client->errorFrom = CAKE_SOCKET_ERROR_FROM_RECV;
        client->errorCode = cake_socket_get_last_error_code();
        return cake_false;
    }
    return cake_true;
}

Cake_String_UTF8 *__cake_socket_stream_read_line(Cake_SocketBuffer *_stream, cake_bool (*recvFunc)(void *, char *, ulonglong, ulonglong *), void *_s) {
    // TODO: portage Linux
#ifdef CAKE_WINDOWS
    char buffer[CAKE_SOCKET_READ_BUFFER_SIZE];
    ulonglong bytesReceived;
    ulonglong size, i;
    void *ptr;

    Cake_String_UTF8 *out = cake_strutf8("");

    // On vérifie si le buffer ne contient pas déjà un saut de ligne
    for(i = 0; i < _stream->size; i++) {
        if(_stream->ptr[i] == '\n') {
            cake_strutf8_add_bytes(out, _stream->ptr, i);
            i++;

            // On ignore les retours chariots
            while(i < _stream->size && _stream->ptr[i] == '\r')
                i++;

            // On modifie le buffer interne
            size = _stream->size - i;
            ptr = malloc(size);
            if(ptr != NULL) {
                memcpy(ptr, _stream->ptr + i, size);
                free(_stream->ptr);
                _stream->ptr = (cake_byte *) ptr;
                _stream->size = size;
            }

            return out;
        }
    }

    cake_strutf8_add_bytes(out, _stream->ptr, _stream->size);

    // Lecture des données depuis le socket
    while(recvFunc(_s, buffer, CAKE_SOCKET_READ_BUFFER_SIZE, &bytesReceived) && bytesReceived > 0) {
        // On vérifie s'il y a un saut de ligne
        for(i = 0; i < bytesReceived; i++) {
            if(buffer[i] == '\n') {
                cake_strutf8_add_bytes(out, buffer, i);
                i++;

                // On ignore les retours chariots
                while(i < bytesReceived && buffer[i] == '\r')
                    i++;

                // On modifie le buffer interne
                size = bytesReceived - i;
                ptr = malloc(size);
                if(ptr != NULL) {
                    free(_stream->ptr);
                    _stream->ptr = (cake_byte *) ptr;
                    _stream->size = size;
                    memcpy(_stream->ptr, buffer + i, size);
                }
                return out;
            }
        }

        size = _stream->size + bytesReceived;
        ptr = realloc(_stream->ptr, size);
        if(ptr != NULL) {
            _stream->ptr = (cake_byte *) ptr;
            memcpy(_stream->ptr + _stream->size, buffer, bytesReceived);
            _stream->size = size;
        }
    }
#else

#endif
    cake_free_strutf8(out);
    return NULL;
}

cake_bool __cake_tls_client_stream_read_raw(Cake_TLSClientStream *stream, ulonglong len, char *buff) {
    stream->tls.clientSocket.errorFrom = CAKE_SOCKET_ERROR_FROM_NO_ERROR;
    len -= stream->buffer.size;

    ulonglong b;
    ulonglong total = 0;
    while(1) {
        if(!__cake_tls_client_recv(&stream->tls, buff + total, len, &b))
            return cake_false;
        total += b;
        if(total == len)
            return cake_true;
    }
}

/* ===== CLIENT SOCKET FUNCTIONS ===== */

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

cake_bool cake_client_socket_send(Cake_ClientSocket *sock, const char *data, ulonglong size) {
    // TODO: portage Linux
    #ifdef CAKE_WINDOWS
    if(send(sock->socket, data, size, 0) == SOCKET_ERROR) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_SEND;
        sock->errorCode = cake_socket_get_last_error_code();
        return cake_false;
    }
    #else

    #endif
    return cake_true;
}

void cake_free_client_socket(Cake_ClientSocket *sock) {
    cake_close_socket(sock->socket);
    freeaddrinfo(sock->address);
}

/* ===== TLS CLIENT FUNCTIONS ===== */

cake_bool cake_create_tls_client(Cake_TLSClient *tls, const char *hostname, const char *port, cake_byte ipMode) {
    if(!cake_create_client_socket(&tls->clientSocket, hostname, port, ipMode))
        return cake_false;

    tls->ctx = SSL_CTX_new(TLS_client_method());
    if(tls->ctx == NULL) {
        ERR_print_errors_fp(stderr);
        return cake_false;
    }

    // On définie la version minimum et maximum du protocole utilisé
    SSL_CTX_set_min_proto_version(tls->ctx, TLS1_VERSION);
    SSL_CTX_set_max_proto_version(tls->ctx, TLS1_3_VERSION);

    SSL_CTX_set_cipher_list(tls->ctx, "ALL:eNULL");

    SSL_CTX_set_verify(tls->ctx, SSL_VERIFY_NONE, NULL);
    //SSL_CTX_set_verify_depth(tls->ctx, 4);
    SSL_CTX_set_options(tls->ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION);

    /* const char *cacertPath = "cacert.pem";
    SSL_CTX_set_client_CA_list(tls->ctx, SSL_load_client_CA_file(cacertPath));
    SSL_CTX_load_verify_locations(tls->ctx, cacertPath, NULL); */

    tls->ssl = SSL_new(tls->ctx);
    if(tls->ssl == NULL) {
        ERR_print_errors_fp(stderr);
        return cake_false;
    }

    return cake_true;
}

cake_bool cake_tls_client_connect(Cake_TLSClient *tls) {
    if(!cake_client_socket_connect(&tls->clientSocket))
        return cake_false;

    int ret = SSL_set_fd(tls->ssl, tls->clientSocket.socket);
    if(ret != 1) {
        tls->clientSocket.errorFrom = CAKE_SOCKET_ERROR_FROM_SSL_SET_FD;
        tls->clientSocket.errorCode = SSL_get_error(tls->ssl, ret);
        return cake_false;
    }

    ret = SSL_connect(tls->ssl);
    if(ret != 1) {
        tls->clientSocket.errorFrom = CAKE_SOCKET_ERROR_FROM_SSL_CONNECT;
        tls->clientSocket.errorCode = SSL_get_error(tls->ssl, ret);
        return cake_false;
    }

    return cake_true;
}

cake_bool cake_tls_client_send(Cake_TLSClient *tls, const char *data, ulonglong size) {
    // written est obligatoire ou alors SSL_write_ex fait planter le programme
    size_t written;
    int ret = SSL_write_ex(tls->ssl, data, size, &written);
    if(ret != 1) {
        tls->clientSocket.errorFrom = CAKE_SOCKET_ERROR_FROM_SSL_WRITE_EX;
        tls->clientSocket.errorCode = SSL_get_error(tls->ssl, ret);
        return cake_false;
    }
    return cake_true;
}

char *cake_tls_client_recv_dyn(Cake_TLSClient *tls, ulonglong size) {
    tls->clientSocket.errorFrom = CAKE_SOCKET_ERROR_FROM_NO_ERROR;
    char *ptr = (char *) malloc(size);
    if(ptr == NULL)
        return NULL;
    ulonglong b;
    if(!__cake_tls_client_recv(tls, ptr, size, &b)) {
        free(ptr);
        return NULL;
    }
    return ptr;
}

void cake_free_tls_client(Cake_TLSClient *tls) {
    cake_free_client_socket(&tls->clientSocket);
    SSL_free(tls->ssl);
    SSL_CTX_free(tls->ctx);
}

/* ===== ACCEPTED CLIENT SOCKET ===== */

cake_bool cake_accepted_client_socket_send(Cake_AcceptedClientSocket *sock, const char *data, ulonglong size) {
    // TODO: portage Linux
#ifdef CAKE_WINDOWS
    if(send(sock->sock, data, size, 0) == SOCKET_ERROR) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_SEND;
        sock->errorCode = cake_socket_get_last_error_code();
        return cake_false;
    }
#else

#endif
    return cake_true;
}

/* ===== SERVER SOCKET FUNCTIONS ===== */

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

cake_bool cake_server_socket_accept(Cake_ServerSocket *sock, Cake_AcceptedClientSocket *dest) {
    cake_socklen size = sizeof(dest->addr);
    dest->sock = accept(sock->socket, (struct sockaddr *) &dest->addr, &size);
    if(dest->sock == CAKE_SOCKET_BAD_SOCKET) {
        sock->errorFrom = CAKE_SOCKET_ERROR_FROM_ACCEPT;
        sock->errorCode = cake_socket_get_last_error_code();
        return cake_false;
    }
    return cake_true;
}

/* ===== SOCKET STREAM FUNCTIONS ===== */

char *cake_tls_client_stream_read_raw(Cake_TLSClientStream *stream, ulonglong len) {
    if(len > stream->buffer.size) {
        char *ptr = (char *) cake_realloc(stream->buffer.ptr, len);
        if(ptr == NULL)
            return NULL;

        if(!__cake_tls_client_stream_read_raw(stream, len, ptr + stream->buffer.size)) {
            stream->buffer.ptr = (cake_byte *) ptr;
            return NULL;
        }

        stream->buffer.ptr  = NULL;
        stream->buffer.size = 0;

        return ptr;
    }

    // TODO: len <= buffer size
    return NULL;
}

Cake_String_UTF8 *cake_tls_client_stream_read_str(Cake_TLSClientStream *stream, ulonglong len) {
    Cake_String_UTF8 *ret = (Cake_String_UTF8 *) cake_new(sizeof(*ret));
    if(ret == NULL)
        return NULL;

    if(len > stream->buffer.size) {
        ret->bytes = (uchar *) cake_realloc(stream->buffer.ptr, len * sizeof(*ret->bytes) + sizeof(*ret->bytes));
        if(ret->bytes == NULL) {
            cake_free(ret);
            return NULL;
        }

        if(!__cake_tls_client_stream_read_raw(stream, len, ret->bytes)) {
            stream->buffer.ptr = (cake_byte *) ret->bytes;
            cake_free(ret);
            return NULL;
        }
        
        stream->buffer.ptr  = NULL;
        stream->buffer.size = 0;

        ret->bytes[len] = '\0';
        ret->data.length = len;
        ret->length = cake_strutf8_length(ret);

        return ret;
    }
    // TODO: len <= buffer size
    return NULL;
}

void cake_free_client_socket_stream(Cake_ClientSocketStream *stream) {
    free(stream->buffer.ptr);
    cake_free_client_socket(&stream->client);
}

Cake_String_UTF8 *cake_tls_client_stream_read_line(Cake_TLSClientStream *stream) {
    return __cake_socket_stream_read_line(&stream->buffer, __cake_tls_client_recv, &stream->tls);
}

void cake_free_tls_client_stream(Cake_TLSClientStream *stream) {
    free(stream->buffer.ptr);
    cake_free_tls_client(&stream->tls);
}

void cake_free_accepted_client_socket_stream(Cake_AcceptedClientSocketStream *stream) {
    free(stream->buffer.ptr);
    cake_free_accepted_client_socket(&stream->client);
}
