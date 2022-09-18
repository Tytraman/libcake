#ifndef __CAKE_ERROR_H__
#define __CAKE_ERROR_H__

#include "def.h"

const char *cake_get_ssl_error(int errorCode);
void cake_print_socket_error(cake_byte errorFrom, ulong errorCode);

#endif