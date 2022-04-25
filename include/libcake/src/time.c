#include "../time.h"

#ifdef CAKE_WINDOWS
ulonglong cake_filetime_to_ularge(FILETIME *ft) {
    ULARGE_INTEGER uLarge;
    uLarge.HighPart = ft->dwHighDateTime;
    uLarge.LowPart  = ft->dwLowDateTime;
    return uLarge.QuadPart;
}
#else
#include <time.h>
#endif

ulonglong cake_get_current_time_millis() {
    #ifdef CAKE_WINDOWS
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return cake_filetime_to_ularge(&ft) / 10000 - 11644473600000;
    #else
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return (t.tv_sec * 1000) + (t.tv_nsec / 1e6);
    #endif
}
