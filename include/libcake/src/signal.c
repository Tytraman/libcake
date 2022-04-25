#include "../signal.h"
#include "../thread.h"

#include "../def.h"

#include <stdio.h>
#include <stdlib.h>

static SignalEvent s_CtrlCEvent = NULL;
static cake_bool s_CtrlCEventTrigerred = cake_false;
static cake_bool s_CtrlCEventUndefinedBehavior = cake_false;

void signal_init() {
    #ifdef CAKE_WINDOWS
    SetConsoleCtrlHandler(cake_win32_handler_routine, TRUE);
    #else
    signal(SIGINT, cake_unix_ctrl_c_routine);
    #endif
}

int ctrl_c_callback(void *pArgs) {
    switch(s_CtrlCEvent()) {
        default: break;
        case CAKE_SIGNAL_OK:
            free(pArgs);
            #ifdef CAKE_WINDOWS
            ExitProcess(42);
            #else
            exit(42);
            #endif
            break;
        case CAKE_SIGNAL_NO:
            s_CtrlCEventTrigerred = cake_false;
            s_CtrlCEventUndefinedBehavior = cake_false;
            break;
        case CAKE_SIGNAL_UNDEFINED_BEHAVIOR:
            s_CtrlCEventTrigerred = cake_false;
            s_CtrlCEventUndefinedBehavior = cake_true;
            printf("\n");
            break;
    }
    free(pArgs);
    return 1;
}


#ifdef CAKE_WINDOWS
BOOL WINAPI cake_win32_handler_routine(DWORD dwCtrlType){

    switch(dwCtrlType) {
        default: return cake_false;
        case CTRL_C_EVENT:{
            if(s_CtrlCEvent != NULL) {
                if(s_CtrlCEventTrigerred == cake_false) {
                    if(s_CtrlCEventUndefinedBehavior == cake_false) {
                        s_CtrlCEventTrigerred = cake_true;
                        Cake_Thread *thread = (Cake_Thread *) malloc(sizeof(Cake_Thread));
                        cake_create_thread(thread, ctrl_c_callback, NULL);
                        cake_thread_run(thread, thread);
                    }
                    s_CtrlCEventUndefinedBehavior = cake_false;
                }
            }
            return cake_true;
        }
    }
}
#else
void cake_unix_ctrl_c_routine(int sig) {
    if(s_CtrlCEvent != NULL) {
        if(s_CtrlCEventTrigerred == cake_false) {
            if(s_CtrlCEventUndefinedBehavior == cake_false) {
                printf("\b\b  \b\b");
                s_CtrlCEventTrigerred = cake_true;
                Cake_Thread *thread = (Cake_Thread *) malloc(sizeof(Cake_Thread));
                cake_create_thread(thread, ctrl_c_callback, NULL);
                cake_thread_run(thread, thread);
            }
            s_CtrlCEventUndefinedBehavior = cake_false;
        }
    }
}
#endif

void cake_signal_set_ctrl_c_event(SignalEvent event) {
    s_CtrlCEvent = event;
}
