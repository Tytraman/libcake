#include "../win.h"

#ifdef CAKE_WINDOWS

cake_bool cake_is_double_clicked() {
    HWND consoleWnd = GetConsoleWindow();
    DWORD processID;
    GetWindowThreadProcessId(consoleWnd, &processID);
    if(GetCurrentProcessId() == processID)
        return cake_true;
    return cake_false;
}

#endif
