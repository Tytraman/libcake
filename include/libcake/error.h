#ifndef __CAKE_ERROR_H__
#define __CAKE_ERROR_H__

#include "def.h"
#include "socket.h"

const char *cake_get_ssl_error(int errorCode);
void cake_print_socket_error(cake_byte errorFrom, ulong errorCode);

inline void cake_print_client_socket_error(Cake_ClientSocket *client) {
	cake_print_socket_error(client->errorFrom, client->errorCode);
}

inline void cake_print_server_socket_error(Cake_ServerSocket *server) {
	cake_print_socket_error(server->errorFrom, server->errorCode);
}

#endif