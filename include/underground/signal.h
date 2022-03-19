#ifndef __CAKE_SIGNAL_H__
#define __CAKE_SIGNAL_H__

#include "def.h"


#define CAKE_SIGNAL_OK                 1
#define CAKE_SIGNAL_NO                 2
// Renvoyer ce code peut amener à des résultats non désirés.
#define CAKE_SIGNAL_UNDEFINED_BEHAVIOR 3

typedef char (*SignalEvent)(void);

void cake_signal_init();

#ifdef CAKE_WINDOWS
#include <windows.h>

BOOL WINAPI cake_win32_handler_routine(DWORD dwCtrlType);
#else
#include <signal.h>

void cake_unix_ctrl_c_routine(int sig);
#endif


void cake_signal_set_ctrl_c_event(SignalEvent event);

#endif