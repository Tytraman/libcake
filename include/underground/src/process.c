#include "../process.h"

#include <stdio.h>

#ifdef PIKA_WINDOWS
pika_bool start_process(const uchar *command, pika_fd *fdProcess, pika_fd pipeStdout[2], pika_fd pipeStderr[2], pika_fd pipeStdin[2]) {
    *fdProcess = NULL;

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

    *fdProcess = pi.hProcess;
    CloseHandle(pi.hThread);

    return pika_true;
}
#endif
