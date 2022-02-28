#include "../signal.h"
#include "../thread.h"

#include "../def.h"

#include <stdio.h>
#include <stdlib.h>

static SignalEvent s_CtrlCEvent = NULL;
static pika_bool s_CtrlCEventTrigerred = pika_false;
static pika_bool s_CtrlCEventUndefinedBehavior = pika_false;

void signal_init() {
    #ifdef PIKA_WINDOWS
    SetConsoleCtrlHandler(win32_handler_routine, TRUE);
    #else
    signal(SIGINT, unix_ctrl_c_routine);
    #endif
}

int ctrl_c_callback(void *pArgs) {
    switch(s_CtrlCEvent()) {
        default: break;
        case SIGNAL_OK:
            free(pArgs);
            #ifdef PIKA_WINDOWS
            ExitProcess(42);
            #else
            exit(42);
            #endif
            break;
        case SIGNAL_NO:
            s_CtrlCEventTrigerred = pika_false;
            s_CtrlCEventUndefinedBehavior = pika_false;
            break;
        case SIGNAL_UNDEFINED_BEHAVIOR:
            s_CtrlCEventTrigerred = pika_false;
            s_CtrlCEventUndefinedBehavior = pika_true;
            printf("\n");
            break;
    }
    free(pArgs);
}


#ifdef PIKA_WINDOWS
BOOL WINAPI win32_handler_routine(DWORD dwCtrlType){

    switch(dwCtrlType) {
        default: return pika_false;
        case CTRL_C_EVENT:{
            if(s_CtrlCEvent != NULL) {
                if(s_CtrlCEventTrigerred == pika_false) {
                    if(s_CtrlCEventUndefinedBehavior == pika_false) {
                        s_CtrlCEventTrigerred = pika_true;
                        Thread *thread = (Thread *) malloc(sizeof(Thread));
                        create_thread(thread, ctrl_c_callback, NULL);
                        thread_run(thread, thread);
                    }
                    s_CtrlCEventUndefinedBehavior = pika_false;
                }
            }
            return pika_true;
        }
    }
}
#else
void unix_ctrl_c_routine(int sig) {
    if(s_CtrlCEvent != NULL) {
        if(s_CtrlCEventTrigerred == pika_false) {
            if(s_CtrlCEventUndefinedBehavior == pika_false) {
                printf("\b\b  \b\b");
                s_CtrlCEventTrigerred = pika_true;
                Thread *thread = (Thread *) malloc(sizeof(Thread));
                create_thread(thread, ctrl_c_callback, NULL);
                thread_run(thread, thread);
            }
            s_CtrlCEventUndefinedBehavior = pika_false;
        }
    }
}
#endif

void signal_set_ctrl_c_event(SignalEvent event) {
    s_CtrlCEvent = event;
}
