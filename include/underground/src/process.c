#include "../process.h"

#include <stdio.h>

#ifdef PIKA_WINDOWS
pika_exit_code start_process(pika_char *command) {

    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFOW si = { 0 };
    si.cb = sizeof(si);

    size_t length = char_length(command);
    pika_char *copyCommand = (pika_char *) malloc(length * sizeof(pika_char) + sizeof(pika_char));
    memcpy(copyCommand, command, length * sizeof(pika_char));
    copyCommand[length] = 0;

    if(!CreateProcessW(
        NULL,
        copyCommand,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        free(copyCommand);
        return 69;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    free(copyCommand);

    pika_exit_code code;
    GetExitCodeProcess(pi.hProcess, &code);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return code;
}
#endif
