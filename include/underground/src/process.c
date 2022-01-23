#include "../process.h"

#include <stdio.h>

pika_bool start_process(pika_process_command command, pika_process *process, pika_fd pipeStdout[2], pika_fd pipeStderr[2], pika_fd pipeStdin[2]) {
    #ifdef PIKA_WINDOWS
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

    String_UTF16 cmd;
    create_strutf16(&cmd);
    String_UTF8 *utf8 = strutf8(command);
    strutf8_to_utf16(utf8, &cmd);
    free_strutf8(utf8);

    if(!CreateProcessW(
        NULL,
        cmd.characteres,
        NULL,
        NULL,
        si.dwFlags & STARTF_USESTDHANDLES,
        0,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        free(cmd.characteres);
        return pika_false;
    }

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
    free(cmd.characteres);

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
        return pika_false;

    if(pipeStdout != NULL)
        close(pipeStdout[1]);
    if(pipeStderr != NULL)
        close(pipeStderr[1]);
    if(pipeStdin != NULL)
        close(pipeStdin[0]);
    #endif

    return pika_true;
}

#ifdef PIKA_WINDOWS
void process_wait(pika_process process, pika_exit_code *pExitCode) {
    WaitForSingleObject(process, INFINITE);
    GetExitCodeProcess(process, pExitCode);
    CloseHandle(process);
}
#endif
