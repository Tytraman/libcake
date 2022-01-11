#include "../thread.h"

#ifdef PIKA_WINDOWS
unsigned int __stdcall internal_thread(void *pParam) 
#else
static void *internal_thread(void *pParam)
#endif
{
    Thread *thread = (Thread *) pParam;
    unsigned int returnValue = thread->runCallback(thread->args);
    if(thread->endCallback)
        thread->endCallback(thread->args);
    
    #ifdef PIKA_WINDOWS
    thread_exit(0);
    free_thread(thread);
    return returnValue;
    #else
    return (void *) (unsigned long long) returnValue;
    #endif
}

void thread_create(Thread *thread, ThreadRunCallback runCallback, ThreadEndCallback endCallback) {
    #ifdef PIKA_WINDOWS
    thread->hThread = NULL;
    #endif

    thread->runCallback = runCallback;
    thread->endCallback = endCallback;
}

char thread_run(Thread *thread, void *args) {
    if(!thread->runCallback) return THREAD_NO_RUN_CALLBACK;
    thread->args = args;
    #ifdef PIKA_WINDOWS
    thread->hThread = (HANDLE) _beginthreadex(NULL, 0, internal_thread, thread, 0, 0);
    #else
    if(pthread_create(&thread->hThread, NULL, internal_thread, thread) != 0) return THREAD_ERROR_ON_CREATE;
    #endif

    return THREAD_OK;
}

void thread_wait(Thread *thread) {
    if(!thread->hThread) return;
    #ifdef PIKA_WINDOWS
    WaitForSingleObject(thread->hThread, INFINITE);
    #else
    unsigned int *returnValue;
    pthread_join(thread->hThread, (void **) &returnValue);
    #endif
}

#ifdef PIKA_WINDOWS
void free_thread(Thread *thread) {
    CloseHandle(thread->hThread);
    thread->hThread = NULL;
}
#endif
