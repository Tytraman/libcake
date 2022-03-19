#include "../process.h"

#include <stdio.h>

cake_bool cake_start_process(cake_process_command command, cake_process *process, cake_fd pipeStdout[2], cake_fd pipeStderr[2], cake_fd pipeStdin[2]) {
    #ifdef CAKE_WINDOWS
    *process = NULL;

    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFOW si = { 0 };
    si.cb = sizeof(si);

    if(pipeStdout != NULL || pipeStderr != NULL || pipeStdin != NULL) {
        si.dwFlags |= STARTF_USESTDHANDLES;

        if(pipeStdout != NULL)
            si.hStdOutput = pipeStdout[1];
        else
            si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

        if(pipeStderr != NULL)
            si.hStdError = pipeStderr[1];
        else
            si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

        if(pipeStdin != NULL)
            si.hStdInput = pipeStdin[0];
        else
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    }

    if(!CreateProcessW(
        NULL,
        command,
        NULL,
        NULL,
        si.dwFlags & STARTF_USESTDHANDLES,
        0,
        NULL,
        NULL,
        &si,
        &pi
    ))
        return cake_false;

    if(si.dwFlags & STARTF_USESTDHANDLES) {
        if(pipeStdout != NULL) {
            CloseHandle(pipeStdout[1]);
            pipeStdout[1] = NULL;
        }
        if(pipeStderr != NULL) {
            CloseHandle(pipeStderr[1]);
            pipeStderr[1] = NULL;
        }
        if(pipeStdin != NULL) {
            CloseHandle(pipeStdin[0]);
            pipeStdin[0] = NULL;
        }
    }

    *process = pi.hProcess;
    CloseHandle(pi.hThread);
    #else
    *process = fork();

    // Processus enfant :
    if(*process == 0) {
        if(pipeStdout != NULL) {
            close(pipeStdout[0]);
            dup2(pipeStdout[1], STDOUT_FILENO);
        }
        if(pipeStderr != NULL) {
            close(pipeStderr[0]);
            dup2(pipeStderr[1], STDERR_FILENO);
        }
        if(pipeStdin != NULL) {
            close(pipeStdin[1]);
            dup2(pipeStdin[0], STDIN_FILENO);
        }
        execvp(command[0], command);
        exit(25);
    }else if(*process == -1)
        return cake_false;

    if(pipeStdout != NULL)
        close(pipeStdout[1]);
    if(pipeStderr != NULL)
        close(pipeStderr[1]);
    if(pipeStdin != NULL)
        close(pipeStdin[0]);
    #endif

    return cake_true;
}

#ifdef CAKE_WINDOWS
void __cake_process_wait(cake_process process, cake_exit_code *pExitCode) {
    WaitForSingleObject(process, INFINITE);
    GetExitCodeProcess(process, pExitCode);
    CloseHandle(process);
}

void CAKE_PROCESS_COMMAND_ADD_ARG(cake_process_command_dyn command, const wchar_t *arg) {
    cake_strutf16_add_char(command, L' ');
    cake_strutf16_add_wchar_array(command, arg);
}
#else
cake_process_command_dyn CAKE_PROCESS_COMMAND_DYN(const char *value) {
    cake_process_command_dyn dyn = (cake_process_command_dyn) malloc(sizeof(__cake_process_command_dyn));
    dyn->args = NULL;
    cake_array_resize((Cake_ArrayList *) dyn, sizeof(char *), 2);
    ulonglong length = cake_str_count(value);
    dyn->args[0] = (char *) malloc(length * sizeof(char) + sizeof(char));
    memcpy(dyn->args[0], value, length * sizeof(char));
    dyn->args[0][length] = '\0';
    dyn->args[1] = NULL;
    return dyn;
}

void CAKE_PROCESS_COMMAND_ADD_ARG(cake_process_command_dyn command, const char *arg) {
    ulonglong current = command->data.length - 1;
    cake_array_resize((Cake_ArrayList *) command, sizeof(char *), current + 2);
    ulonglong length = cake_str_count(arg);
    command->args[current] = (char *) malloc(length * sizeof(char) + sizeof(char));
    memcpy(command->args[current], arg, length * sizeof(char));
    command->args[current][length] = '\0';
    command->args[current + 1] = NULL;
}

void CAKE_PROCESS_COMMAND_FREE(cake_process_command_dyn command) {
    ulonglong i;
    for(i = 0; i < command->data.length - 1; ++i)
        free(command->args[i]);
    free(command->args);
    free(command);
}
#endif
