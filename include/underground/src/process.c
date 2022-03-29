#include "../process.h"

#include <stdio.h>

cake_bool cake_create_process(const uchar *command, Cake_Process *process, cake_fd pipeStdout[2], cake_fd pipeStderr[2], cake_fd pipeStdin[2]) {
    #ifdef CAKE_WINDOWS
    Cake_String_UTF16 command16;
    cake_create_strutf16(&command16);
    cake_char_array_to_strutf16(command, &command16);
    process->process = NULL;
    process->thread  = NULL;

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
        command16.characteres,
        NULL,
        NULL,
        si.dwFlags & STARTF_USESTDHANDLES,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        free(command16.characteres);
        return cake_false;
    }

    free(command16.characteres);

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

    process->process = pi.hProcess;
    process->thread  = pi.hThread;
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
        Cake_String_UTF8 *cmd = cake_strutf8(command);
        Cake_List_String_UTF8 *args = cake_strutf8_split(cmd, " ");
        char **passArgs = (char **) malloc(args->data.length * sizeof(uchar *) + sizeof(char *));
        ulonglong i;
        for(i = 0; i < args->data.length; ++i)
            passArgs[i] = args->list[i]->bytes;

        execvp(passArgs[0], passArgs);
        free(passArgs);
        cake_free_list_strutf8(args);
        cake_free_strutf8(cmd);
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
void __cake_process_wait(Cake_Process *process, cake_exit_code *pExitCode) {
    WaitForSingleObject(process->process, INFINITE);
    GetExitCodeProcess(process->process, pExitCode);
    CloseHandle(process->process);
}
#endif
