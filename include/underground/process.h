#ifndef __CAKE_PROCESS_H__
#define __CAKE_PROCESS_H__

#include "def.h"
#include "pipe.h"
#include "utf16.h"
#include "utf8.h"

#include <stdlib.h>

#define CAKE_CONCAT(a, b) a##b
#define EXPAND_CONCAT(a, b) CAKE_CONCAT(a, b)
#define L__SPAYCE L" "

/*
        Solution en attendant de trouver mieux,
        oui ça m'a prit du temps à faire.
*/
#define __CAKE_PROCESS_COMMAND_ARRAY_1(v0) { v0, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_2(v0, v1) { v0, v1, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_3(v0, v1, v2) { v0, v1, v2, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_4(v0, v1, v2, v3) { v0, v1, v2, v3, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_5(v0, v1, v2, v3, v4) { v0, v1, v2, v3, v4, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_6(v0, v1, v2, v3, v4, v5) { v0, v1, v2, v3, v4, v5, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_7(v0, v1, v2, v3, v4, v5, v6) { v0, v1, v2, v3, v4, v5, v6, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_8(v0, v1, v2, v3, v4, v5, v6, v7) { v0, v1, v2, v3, v4, v5, v6, v7, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_9(v0, v1, v2, v3, v4, v5, v6, v7, v8) { v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_10(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_11(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_12(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_13(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_14(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_15(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_16(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_17(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_18(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_19(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_20(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, NULL }
#define __CAKE_PROCESS_COMMAND_ARRAY_21(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, NULL }

// Maximum actuel : 21
#define CAKE_PROCESS_COMMAND_ARRAY(length, ...) CAKE_CONCAT(__CAKE_PROCESS_COMMAND_ARRAY_, length) (__VA_ARGS__)

// Ma santé mentale viens de baisser de 150 points...
// TODO: faire un programme qui crée les macros à ma place.

#define __CAKE_ULTRA_CONCAT_1(v0) v0
#define __CAKE_ULTRA_CONCAT_2(v0, v1)  v0 L__SPAYCE v1
#define __CAKE_ULTRA_CONCAT_3(v0, v1, v2) v0 L__SPAYCE v1 L__SPAYCE v2
#define __CAKE_ULTRA_CONCAT_4(v0, v1, v2, v3) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3
#define __CAKE_ULTRA_CONCAT_5(v0, v1, v2, v3, v4) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4
#define __CAKE_ULTRA_CONCAT_6(v0, v1, v2, v3, v4, v5) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5
#define __CAKE_ULTRA_CONCAT_7(v0, v1, v2, v3, v4, v5, v6) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6
#define __CAKE_ULTRA_CONCAT_8(v0, v1, v2, v3, v4, v5, v6, v7) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7
#define __CAKE_ULTRA_CONCAT_9(v0, v1, v2, v3, v4, v5, v6, v7, v8) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8
#define __CAKE_ULTRA_CONCAT_10(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9
#define __CAKE_ULTRA_CONCAT_11(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10
#define __CAKE_ULTRA_CONCAT_12(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11
#define __CAKE_ULTRA_CONCAT_13(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11 L__SPAYCE v12
#define __CAKE_ULTRA_CONCAT_14(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11 L__SPAYCE v12 L__SPAYCE v13
#define __CAKE_ULTRA_CONCAT_15(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11 L__SPAYCE v12 L__SPAYCE v13 L__SPAYCE v14
#define __CAKE_ULTRA_CONCAT_16(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11 L__SPAYCE v12 L__SPAYCE v13 L__SPAYCE v14 L__SPAYCE v15
#define __CAKE_ULTRA_CONCAT_17(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11 L__SPAYCE v12 L__SPAYCE v13 L__SPAYCE v14 L__SPAYCE v15 L__SPAYCE v16
#define __CAKE_ULTRA_CONCAT_18(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11 L__SPAYCE v12 L__SPAYCE v13 L__SPAYCE v14 L__SPAYCE v15 L__SPAYCE v16 L__SPAYCE v17
#define __CAKE_ULTRA_CONCAT_19(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11 L__SPAYCE v12 L__SPAYCE v13 L__SPAYCE v14 L__SPAYCE v15 L__SPAYCE v16 L__SPAYCE v17 L__SPAYCE v18
#define __CAKE_ULTRA_CONCAT_20(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11 L__SPAYCE v12 L__SPAYCE v13 L__SPAYCE v14 L__SPAYCE v15 L__SPAYCE v16 L__SPAYCE v17 L__SPAYCE v18 L__SPAYCE v19
#define __CAKE_ULTRA_CONCAT_21(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) v0 L__SPAYCE v1 L__SPAYCE v2 L__SPAYCE v3 L__SPAYCE v4 L__SPAYCE v5 L__SPAYCE v6 L__SPAYCE v7 L__SPAYCE v8 L__SPAYCE v9 L__SPAYCE v10 L__SPAYCE v11 L__SPAYCE v12 L__SPAYCE v13 L__SPAYCE v14 L__SPAYCE v15 L__SPAYCE v16 L__SPAYCE v17 L__SPAYCE v18 L__SPAYCE v19 L__SPAYCE v20

// Maximum actuel : 21
#define CAKE_ULTRA_CONCAT(length, ...) CAKE_CONCAT(__CAKE_ULTRA_CONCAT_, length) (__VA_ARGS__)

#ifdef CAKE_WINDOWS

typedef HANDLE cake_process;

typedef wchar_t cake_process_command[];
typedef Cake_String_UTF16 *cake_process_command_dyn;

// Max : 21
#define CAKE_PROCESS_COMMAND(length, ...) CAKE_ULTRA_CONCAT(length, __VA_ARGS__)


#define CAKE_PROCESS_COMMAND_DYN(value) cake_strutf16(value)


void CAKE_PROCESS_COMMAND_ADD_ARG(cake_process_command_dyn command, const wchar_t *arg);

#define CAKE_PROCESS_COMMAND_STR(command) command->characteres

#define CAKE_PROCESS_COMMAND_FREE(command) cake_free_strutf16(command)

void __cake_process_wait(cake_process process, cake_exit_code *pExitCode);

#define cake_process_wait(process, exitCode) __cake_process_wait(process, CAKE_PTR(exitCode))

#else
#include <sys/wait.h>

typedef pid_t cake_process;

#define cake_process_wait(process, exitCode) waitpid(process, CAKE_PTR(exitCode), 0); \
                                         exitCode = ((exitCode & 0xff00) >> 8)

// Max : 21
#define CAKE_PROCESS_COMMAND(length, ...) CAKE_PROCESS_COMMAND_ARRAY(length, __VA_ARGS__)

typedef char *cake_process_command[];

typedef struct {
    Cake_ArrayList data;
    char **args;
} *cake_process_command_dyn, __cake_process_command_dyn;

cake_process_command_dyn CAKE_PROCESS_COMMAND_DYN(const char *value);

void CAKE_PROCESS_COMMAND_ADD_ARG(cake_process_command_dyn command, const char *arg);

#define CAKE_PROCESS_COMMAND_STR(command) command->args

void CAKE_PROCESS_COMMAND_FREE(cake_process_command_dyn command);

#endif

#define CAKE_COMMAND_NOT_FOUND 1

/*
        Il y a 2 manière d'utiliser cette fonction,
        avec une commande stockée directement dans le code, ou alors allouée dynamiquement.

        Créer la commande :
        - Dans le code : CAKE_PROCESS_COMMAND(value)
        - Dynamiquement : utiliser cake_process_command_dyn et CAKE_PROCESS_COMMAND_DYN(value)
*/
cake_bool cake_start_process(cake_process_command command, cake_process *process, cake_fd pipeStdout[2], cake_fd pipeStderr[2], cake_fd pipeStdin[2]);

#endif