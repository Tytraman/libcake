#ifndef __PIKA_THREAD_H__
#define __PIKA_THREAD_H__

#include "def.h"

#ifdef PIKA_WINDOWS
#include <windows.h>
#include <process.h>

#define thread_exit(x) _endthreadex(x)
#else
#include <pthread.h>

#define thread_exit(x) pthread_exit(x)
#endif

#define THREAD_ERROR_ON_CREATE -2
#define THREAD_NO_RUN_CALLBACK -1
#define THREAD_OK               0

typedef int (*ThreadRunCallback)(void *args);
typedef void (*ThreadEndCallback)(void *args);

typedef struct _Thread {
    #ifdef PIKA_WINDOWS
    HANDLE hThread;
    #else
    pthread_t hThread;
    #endif
    void *args;
    ThreadRunCallback runCallback;
    ThreadEndCallback endCallback;
} Thread;


// Crée un thread sans le lancer.
void create_thread(Thread *thread, ThreadRunCallback runCallback, ThreadEndCallback endCallback);

/*
        Démarre le thread.

        Retourne :
        - THREAD_OK en cas de succès.
        - THREAD_NO_RUN_CALLBACK si il n'y a pas de function de démarrage.
*/
char thread_run(Thread *thread, void *args);

// Attend que le thread finisse son exécution.
void thread_wait(Thread *thread);

#ifdef PIKA_WINDOWS
// Libère la mémoire.
void free_thread(Thread *thread);
#endif

#endif