#include "../pipe.h"

#ifdef CAKE_WINDOWS
char __cake_create_pipe(cake_fd pipefd[2], cake_byte mode) {
    SECURITY_ATTRIBUTES sa;
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    if(!CreatePipe(&pipefd[0], &pipefd[1], &sa, CAKE_BUFF_SIZE))
        return -1;

    // Le stream de lecture de STDOUT et STDERR ne doit pas être hérité.
    // Le stream d'écriture de STDIN ne doit pas être hérité.
    cake_fd modify = mode == 0 ? pipefd[0] : pipefd[1];
    if(!SetHandleInformation(modify, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(pipefd[0]);
        CloseHandle(pipefd[1]);
        return -1;
    }
    
    return 0;
}
#endif
