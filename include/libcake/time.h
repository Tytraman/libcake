#ifndef __CAKE_TIME_H__
#define __CAKE_TIME_H__

#include "def.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CAKE_WINDOWS
ulonglong cake_filetime_to_ularge(FILETIME *ft);
#else

#endif

ulonglong cake_get_current_time_millis();

#ifdef __cplusplus
}
#endif

#endif