#include "include/pikapi.h"

#include <stdio.h>

#ifdef PIKA_WINDOWS
int wmain(int argc, wchar_t **argv)
#else
int main(int argc, char **argv)
#endif
{
    #ifdef PIKA_WINDOWS
    UINT consoleOutputCP = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif

    printf("Au revoir\n");

    #ifdef PIKA_WINDOWS
    SetConsoleOutputCP(consoleOutputCP);
    #endif

    return 0;
}
