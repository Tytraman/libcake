#ifndef __PIKA_PROCESS_H__
#define __PIKA_PROCESS_H__

#include "def.h"
#include <stdlib.h>

#ifdef PIKA_WINDOWS
#include <windows.h>

pika_exit_code start_process(pika_char *command);
#else
#define start_process(command) system(command)
#endif



#endif