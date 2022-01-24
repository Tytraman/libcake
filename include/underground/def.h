#ifndef __PIKA_DEF_H__
#define __PIKA_DEF_H__

#include <string.h>

/*
        Certains typedef qui commencent par pika_ sont des wrappers des structures / fonctions
        des différents OS, par exemple, pika_char sous Windows correspond à wchar_t tandis que sous
        Linux à char.
        Ça permet d'avoir un code beaucoup plus portable.

        pika_bool est un typedef de char peu importe l'OS, ça permet d'avoir une cohésion
        lors d'appels de fonctions de la pikapi.
*/

#if defined(unix) || defined(__unix__) || defined(__unix)
#define PIKA_UNIX 1

#include <sys/types.h>

// Sous Unix un pika_char est équivalent à un char.
typedef char pika_char;

typedef int pika_exit_code;

typedef ssize_t pika_size;

/*
        Permet d'ajouter le L automatiquement devant la chaîne de caractères, ne le fait pas sous Linux,
        permet un code plus portable.
*/
#define PIKA_CHAR(value) value

#define FILE_SEPARATOR             '/'
#define FILE_SEPARATOR_REVERSE     '\\'
#define FILE_SEPARATOR_STR         "/"
#define FILE_SEPARATOR_REVERSE_STR "\\"

#define STR_NULL_END '\0'
#define ASTERISK     '*'

#define char_length(s) strlen(s)
#define char_cmp(s1, s2) strcmp(s1, s2)

#endif

#if defined(_WIN32)
#define PIKA_WINDOWS 1
#include <stddef.h>
#include <windows.h>

// Sous Windows un pika_char est équivalent à un wchar_t.
typedef wchar_t pika_char;

typedef DWORD pika_exit_code;
typedef DWORD pika_size;

/*
        Permet d'ajouter le L automatiquement devant la chaîne de caractères, ne le fait pas sous Linux,
        permet un code plus portable.
*/
#define PIKA_CHAR(value) CONCAT(L, value)

#define FILE_SEPARATOR             L'\\'
#define FILE_SEPARATOR_REVERSE     L'/'
#define FILE_SEPARATOR_STR         L"\\"
#define FILE_SEPARATOR_REVERSE_STR L"/"

#define FILE_SEPARATOR_CHAR             '\\'
#define FILE_SEPARATOR_REVERSE_CHAR     '/'
#define FILE_SEPARATOR_CHAR_STR         "\\"
#define FILE_SEPARATOR_REVERSE_CHAR_STR "/"

#define STR_NULL_END L'\0'
#define ASTERISK     L'*'

#define char_length(s) wcslen(s)
#define char_cmp(s1, s2) wcscmp(s1, s2)

#endif

typedef char pika_bool;

#define pika_true  1
#define pika_false 0

#define PIKA_BUFF_SIZE 2048

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;

typedef signed char   int8;
typedef unsigned char uint8;

typedef unsigned char pika_byte;

#define PIKA_PTR(value) &value

#endif