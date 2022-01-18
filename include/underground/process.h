#ifndef __PIKA_PROCESS_H__
#define __PIKA_PROCESS_H__

#include "def.h"
#include "pipe.h"
#include "utf16.h"
#include "utf8.h"

#include <stdlib.h>

#ifdef PIKA_WINDOWS
#include <windows.h>

pika_bool start_process(const uchar *command, pika_fd *fdProcess, pika_fd pipeStdout[2], pika_fd pipeStderr[2], pika_fd pipeStdin[2]);

#define process_get_exit_code(fd, pExitCode) GetExitCodeProcess(fd, pExitCode) 
#define process_wait(fd) WaitForSingleObject(fd, INFINITE)

#else
#define start_process(command) system(command)
#endif



#endif