#ifndef __CAKE_THREAD_H__
#define __CAKE_THREAD_H__

#include "def.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CAKE_WINDOWS
#include <windows.h>
#include <process.h>

#define cake_thread_exit(x) _endthreadex(x)

#define cake_thread_alloc_storage() TlsAlloc()
#else
#include <pthread.h>

#define cake_thread_exit(x) pthread_exit(x)
#endif

#define CAKE_THREAD_ERROR_ON_CREATE -2
#define CAKE_THREAD_NO_RUN_CALLBACK -1
#define CAKE_THREAD_OK               0

typedef int (*ThreadRunCallback)(void *args);
typedef void (*ThreadEndCallback)(void *args);

typedef struct cake_thread {
    #ifdef CAKE_WINDOWS
    HANDLE hThread;
    #else
    pthread_t hThread;
    #endif
    void *args;
    ThreadRunCallback runCallback;
    ThreadEndCallback endCallback;
} Cake_Thread;


// Crée un thread sans le lancer.
void cake_create_thread(Cake_Thread *thread, ThreadRunCallback runCallback, ThreadEndCallback endCallback);

/*
        Démarre le thread.

        Retourne :
        - CAKE_THREAD_OK en cas de succès.
        - CAKE_THREAD_NO_RUN_CALLBACK si il n'y a pas de function de démarrage.
*/
char cake_thread_run(Cake_Thread *thread, void *args);

// Attend que le thread finisse son exécution.
void cake_thread_wait(Cake_Thread *thread);

#ifdef CAKE_WINDOWS
// Libère la mémoire.
void cake_free_thread(Cake_Thread *thread);
#endif

#ifdef __cplusplus
}
#endif

#endif