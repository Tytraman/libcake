#ifndef __CAKE_WIN_H__
#define __CAKE_WIN_H__

#include "def.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CAKE_WINDOWS

#include <windows.h>

cake_bool cake_is_double_clicked();

#endif

#ifdef __cplusplus
}
#endif

#endif