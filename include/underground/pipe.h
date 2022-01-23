#ifndef __PIKA_PIPE_H__
#define __PIKA_PIPE_H__

#include "def.h"
#include "fdio.h"

#ifdef PIKA_WINDOWS

char __create_pipe(pika_fd pipefd[2], pika_byte mode);


/*
        Crée un pipe standard, par exemple pour STDOUT ou STDERR.

        pipefd[0] = stream de lecture
        pipefd[1] = stream d'écriture
*/
#define create_pipe(pipefd) __create_pipe(pipefd, 0)

/*
        Crée un pipe pour STDIN.

        pipefd[0] = stream de lecture
        pipefd[1] = stream d'écriture
*/
#define create_pipe_stdin(pipefd) __create_pipe(pipefd, 1)

#else
#define create_pipe(pipefd) pipe(pipefd)

#endif



#endif