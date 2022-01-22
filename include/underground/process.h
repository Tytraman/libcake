#ifndef __PIKA_PROCESS_H__
#define __PIKA_PROCESS_H__

#include "def.h"
#include "pipe.h"
#include "utf16.h"
#include "utf8.h"

#include <stdlib.h>

#define CONCAT(a, b) a##b
#define __SPAYCE " "

/*
        Solution en attendant de trouver mieux,
        oui ça m'a prit du temps à faire.
*/
#define __PROCESS_COMMAND_ARRAY_1(v0) { v0, NULL }
#define __PROCESS_COMMAND_ARRAY_2(v0, v1) { v0, v1, NULL }
#define __PROCESS_COMMAND_ARRAY_3(v0, v1, v2) { v0, v1, v2, NULL }
#define __PROCESS_COMMAND_ARRAY_4(v0, v1, v2, v3) { v0, v1, v2, v3, NULL }
#define __PROCESS_COMMAND_ARRAY_5(v0, v1, v2, v3, v4) { v0, v1, v2, v3, v4, NULL }
#define __PROCESS_COMMAND_ARRAY_6(v0, v1, v2, v3, v4, v5) { v0, v1, v2, v3, v4, v5, NULL }
#define __PROCESS_COMMAND_ARRAY_7(v0, v1, v2, v3, v4, v5, v6) { v0, v1, v2, v3, v4, v5, v6, NULL }
#define __PROCESS_COMMAND_ARRAY_8(v0, v1, v2, v3, v4, v5, v6, v7) { v0, v1, v2, v3, v4, v5, v6, v7, NULL }
#define __PROCESS_COMMAND_ARRAY_9(v0, v1, v2, v3, v4, v5, v6, v7, v8) { v0, v1, v2, v3, v4, v5, v6, v7, v8, NULL }
#define __PROCESS_COMMAND_ARRAY_10(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, NULL }
#define __PROCESS_COMMAND_ARRAY_11(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, NULL }
#define __PROCESS_COMMAND_ARRAY_12(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, NULL }
#define __PROCESS_COMMAND_ARRAY_13(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, NULL }
#define __PROCESS_COMMAND_ARRAY_14(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, NULL }
#define __PROCESS_COMMAND_ARRAY_15(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, NULL }
#define __PROCESS_COMMAND_ARRAY_16(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, NULL }
#define __PROCESS_COMMAND_ARRAY_17(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, NULL }
#define __PROCESS_COMMAND_ARRAY_18(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, NULL }
#define __PROCESS_COMMAND_ARRAY_19(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, NULL }
#define __PROCESS_COMMAND_ARRAY_20(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, NULL }
#define __PROCESS_COMMAND_ARRAY_21(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, NULL }

// Maximum actuel : 21
#define PROCESS_COMMAND_ARRAY(length, ...) CONCAT(__PROCESS_COMMAND_ARRAY_, length) (__VA_ARGS__)

// Ma santé mentale viens de baisser de 150 points...
// TODO: faire un programme qui crée les macros à ma place.

#define __ULTRA_CONCAT_1(v0) v0
#define __ULTRA_CONCAT_2(v0, v1)  v0 __SPAYCE v1
#define __ULTRA_CONCAT_3(v0, v1, v2) v0 __SPAYCE v1 __SPAYCE v2
#define __ULTRA_CONCAT_4(v0, v1, v2, v3) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3
#define __ULTRA_CONCAT_5(v0, v1, v2, v3, v4) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4
#define __ULTRA_CONCAT_6(v0, v1, v2, v3, v4, v5) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5
#define __ULTRA_CONCAT_7(v0, v1, v2, v3, v4, v5, v6) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6
#define __ULTRA_CONCAT_8(v0, v1, v2, v3, v4, v5, v6, v7) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7
#define __ULTRA_CONCAT_9(v0, v1, v2, v3, v4, v5, v6, v7, v8) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8
#define __ULTRA_CONCAT_10(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9
#define __ULTRA_CONCAT_11(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10
#define __ULTRA_CONCAT_12(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11
#define __ULTRA_CONCAT_13(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11 __SPAYCE v12
#define __ULTRA_CONCAT_14(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11 __SPAYCE v12 __SPAYCE v13
#define __ULTRA_CONCAT_15(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11 __SPAYCE v12 __SPAYCE v13 __SPAYCE v14
#define __ULTRA_CONCAT_16(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11 __SPAYCE v12 __SPAYCE v13 __SPAYCE v14 __SPAYCE v15
#define __ULTRA_CONCAT_17(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11 __SPAYCE v12 __SPAYCE v13 __SPAYCE v14 __SPAYCE v15 __SPAYCE v16
#define __ULTRA_CONCAT_18(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11 __SPAYCE v12 __SPAYCE v13 __SPAYCE v14 __SPAYCE v15 __SPAYCE v16 __SPAYCE v17
#define __ULTRA_CONCAT_19(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11 __SPAYCE v12 __SPAYCE v13 __SPAYCE v14 __SPAYCE v15 __SPAYCE v16 __SPAYCE v17 __SPAYCE v18
#define __ULTRA_CONCAT_20(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11 __SPAYCE v12 __SPAYCE v13 __SPAYCE v14 __SPAYCE v15 __SPAYCE v16 __SPAYCE v17 __SPAYCE v18 __SPAYCE v19
#define __ULTRA_CONCAT_21(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) v0 __SPAYCE v1 __SPAYCE v2 __SPAYCE v3 __SPAYCE v4 __SPAYCE v5 __SPAYCE v6 __SPAYCE v7 __SPAYCE v8 __SPAYCE v9 __SPAYCE v10 __SPAYCE v11 __SPAYCE v12 __SPAYCE v13 __SPAYCE v14 __SPAYCE v15 __SPAYCE v16 __SPAYCE v17 __SPAYCE v18 __SPAYCE v19 __SPAYCE v20

// Maximum actuel : 21
#define ULTRA_CONCAT(length, ...) CONCAT(__ULTRA_CONCAT_, length) (__VA_ARGS__)

#ifdef PIKA_WINDOWS

typedef HANDLE pika_process;

typedef const uchar *pika_process_command;

// Max : 21
#define PIKA_PROCESS_COMMAND(length, ...) ULTRA_CONCAT(length, __VA_ARGS__)

void process_wait(pika_process process, pika_exit_code *pExitCode);

#else
#include <sys/wait.h>

typedef pid_t pika_process;

#define process_wait(process, pExitCode) waitpid(process, pExitCode, 0)

// Max : 21
#define PIKA_PROCESS_COMMAND(length, ...) PROCESS_COMMAND_ARRAY(length, __VA_ARGS__)

typedef char *pika_process_command[];

#endif

pika_bool start_process(pika_process_command command, pika_process *process, pika_fd pipeStdout[2], pika_fd pipeStderr[2], pika_fd pipeStdin[2]);

#endif