#ifndef __PIKA_SIGNAL_H__
#define __PIKA_SIGNAL_H__

#include "def.h"


#define SIGNAL_OK                 1
#define SIGNAL_NO                 2
// Renvoyer ce code peut amener à des résultats non désirés.
#define SIGNAL_UNDEFINED_BEHAVIOR 3

typedef char (*SignalEvent)(void);

void signal_init();

#ifdef PIKA_WINDOWS
#include <windows.h>

BOOL WINAPI win32_handler_routine(DWORD dwCtrlType);
#else
#include <signal.h>

void unix_ctrl_c_routine(int sig);
#endif


void signal_set_ctrl_c_event(SignalEvent event);

#endif