#include "../win.h"

#ifdef PIKA_WINDOWS

pika_bool is_double_clicked() {
    HWND consoleWnd = GetConsoleWindow();
    DWORD processID;
    GetWindowThreadProcessId(consoleWnd, &processID);
    if(GetCurrentProcessId() == processID)
        return pika_true;
    return pika_false;
}

#endif
