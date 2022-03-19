#ifndef __CAKE_PIPE_H__
#define __CAKE_PIPE_H__

#include "def.h"
#include "fdio.h"

#ifdef CAKE_WINDOWS

char __cake_create_pipe(cake_fd pipefd[2], cake_byte mode);


/*
        Crée un pipe standard, par exemple pour STDOUT ou STDERR.

        pipefd[0] = stream de lecture
        pipefd[1] = stream d'écriture
*/
#define cake_create_pipe(pipefd) __cake_create_pipe(pipefd, 0)

/*
        Crée un pipe pour STDIN.

        pipefd[0] = stream de lecture
        pipefd[1] = stream d'écriture
*/
#define cake_create_pipe_stdin(pipefd) __cake_create_pipe(pipefd, 1)

#else
#define cake_create_pipe(pipefd) pipe(pipefd)

#endif



#endif