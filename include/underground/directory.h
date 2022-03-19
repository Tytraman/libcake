#ifndef __CAKE_DIRECTORY_H__
#define __CAKE_DIRECTORY_H__

#include "def.h"

#ifdef CAKE_WINDOWS

cake_byte cake_change_directory(const uchar *path);
#else
#include <unistd.h>

#define cake_change_directory(path) chdir(path)
#endif

#endif