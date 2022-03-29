#ifndef __CAKE_PROCESS_H__
#define __CAKE_PROCESS_H__

#include "def.h"
#include "pipe.h"
#include "strutf16.h"
#include "strutf8.h"

#include <stdlib.h>
#ifdef CAKE_WINDOWS

typedef struct cake_process {
    HANDLE process;
    HANDLE thread;
} Cake_Process;

void __cake_process_wait(Cake_Process *process, cake_exit_code *pExitCode);

#define cake_process_start(process) ResumeThread(process.thread)

#define cake_process_wait(pProcess, exitCode) __cake_process_wait(pProcess, CAKE_PTR(exitCode))

#else

#include <sys/wait.h>

typedef pid_t Cake_Process;

#define cake_process_wait(process, exitCode) waitpid(process, CAKE_PTR(exitCode), 0); \
                                         exitCode = ((exitCode & 0xff00) >> 8)

#endif

#define CAKE_COMMAND_NOT_FOUND 1

cake_bool cake_create_process(const uchar *command, Cake_Process *process, cake_fd pipeStdout[2], cake_fd pipeStderr[2], cake_fd pipeStdin[2]);



#endif