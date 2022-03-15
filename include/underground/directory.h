#ifndef __PIKA_DIRECTORY_H__
#define __PIKA_DIRECTORY_H__

#include "def.h"

#ifdef PIKA_WINDOWS

pika_byte change_directory(const uchar *path);
#else
#include <unistd.h>

#define change_directory(path) chdir(path)
#endif

#endif