#include "../thread.h"

#ifdef CAKE_WINDOWS
unsigned int __stdcall internal_thread(void *pParam) 
#else
static void *internal_thread(void *pParam)
#endif
{
    Cake_Thread *thread = (Cake_Thread *) pParam;
    unsigned int returnValue = thread->runCallback(thread->args);
    if(thread->endCallback)
        thread->endCallback(thread->args);
    
    #ifdef CAKE_WINDOWS
    thread_exit(0);
    cake_free_thread(thread);
    return returnValue;
    #else
    return (void *) (unsigned long long) returnValue;
    #endif
}

void cake_create_thread(Cake_Thread *thread, ThreadRunCallback runCallback, ThreadEndCallback endCallback) {
    #ifdef CAKE_WINDOWS
    thread->hThread = NULL;
    #endif

    thread->runCallback = runCallback;
    thread->endCallback = endCallback;
}

char cake_thread_run(Cake_Thread *thread, void *args) {
    if(!thread->runCallback) return CAKE_THREAD_NO_RUN_CALLBACK;
    thread->args = args;
    #ifdef CAKE_WINDOWS
    thread->hThread = (HANDLE) _beginthreadex(NULL, 0, internal_thread, thread, 0, 0);
    #else
    if(pthread_create(&thread->hThread, NULL, internal_thread, thread) != 0) return CAKE_THREAD_ERROR_ON_CREATE;
    #endif

    return CAKE_THREAD_OK;
}

void cake_thread_wait(Cake_Thread *thread) {
    if(!thread->hThread) return;
    #ifdef CAKE_WINDOWS
    WaitForSingleObject(thread->hThread, INFINITE);
    #else
    unsigned int *returnValue;
    pthread_join(thread->hThread, (void **) &returnValue);
    #endif
}

#ifdef CAKE_WINDOWS
void cake_free_thread(Cake_Thread *thread) {
    CloseHandle(thread->hThread);
    thread->hThread = NULL;
}
#endif
