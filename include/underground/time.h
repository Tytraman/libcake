#ifndef __CAKE_TIME_H__
#define __CAKE_TIME_H__

#include "def.h"

#ifdef CAKE_WINDOWS
ulonglong cake_filetime_to_ularge(FILETIME *ft);
#else

#endif

ulonglong cake_get_current_time_millis();

#endif