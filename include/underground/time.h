#ifndef __PIKA_TIME_H__
#define __PIKA_TIME_H__

#include "def.h"

#ifdef PIKA_WINDOWS
ulonglong filetime_to_ularge(FILETIME *ft);
ulonglong get_current_time_millis();
#else

#endif

#endif