/**
* @file def.h
* @brief Fichier contenant les types utilisés fréquemment par la librairie.
* @author Tytraman
*/

#ifndef __CAKE_DEF_H__
#define __CAKE_DEF_H__

/// @cond
#include <string.h>
/// @endcond

/*
        Certains typedef qui commencent par _ sont des wrappers des structures / fonctions
        des différents OS, par exemple, cake_char sous Windows correspond à wchar_t tandis que sous
        Linux à char.
        Ça permet d'avoir un code beaucoup plus portable.

        cake_bool est un typedef de char peu importe l'OS, ça permet d'avoir une cohésion
        lors d'appels de fonctions de la pi.
*/

#if defined(__x86_64)
#define CAKE_X86_64 1
#endif

#ifdef __cplusplus
#define CAKE_C extern "c"
#else
#define CAKE_C 
#endif



#define CAKE_CONCAT(a,b) a##b

#if defined(_WIN32)
#define CAKE_WINDOWS 1

#ifdef LIBCAKE_EXPORTS
#define CAKE_API __declspec(dllexport)
#else
#define CAKE_API __declspec(dllimport)
#endif

/// @cond
#include <stddef.h>
#include <WinSock2.h>
#include <windows.h>
/// @endcond

#define cake_sleep(__millis) Sleep(__millis)

// Sous Windows un cake_char est équivalent à un wchar_t.
typedef wchar_t cake_char;

typedef DWORD cake_exit_code;
typedef DWORD cake_size;

typedef HANDLE cake_fd;
typedef DWORD cake_mask;

/*
        Permet d'ajouter le L automatiquement devant la chaîne de caractères, ne le fait pas sous Linux,
        permet un code plus portable.
*/
#define CAKE_CHAR(value) CAKE_CONCAT(L, value)
#define CAKE_CHAR_LENGTH(s) wcslen(s)
#define CAKE_CHAR_CMP(s1, s2) wcscmp(s1,s2)

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

#elif defined(unix) || defined(__unix__) || defined(__unix)
#define CAKE_UNIX 1
#include <sys/types.h>

// Sous Unix un cake_char est équivalent à un char.
typedef char cake_char;

typedef int cake_exit_code;

typedef ssize_t cake_size;

/*
        Permet d'ajouter le L automatiquement devant la chaîne de caractères, ne le fait pas sous Linux,
        permet un code plus portable.
*/
#define CAKE_CHAR(value) value

#define FILE_SEPARATOR             '/'
#define FILE_SEPARATOR_REVERSE     '\\'
#define FILE_SEPARATOR_STR         "/"
#define FILE_SEPARATOR_REVERSE_STR "\\"

#define FILE_SEPARATOR_CHAR             '/'
#define FILE_SEPARATOR_REVERSE_CHAR     '\\'
#define FILE_SEPARATOR_CHAR_STR         "/"
#define FILE_SEPARATOR_REVERSE_CHAR_STR "\\"

#define STR_NULL_END '\0'
#define ASTERISK     '*'

#define CAKE_CHAR_LENGTH(s) strlen(s)
#define CAKE_CHAR_CMP(s1, s2) strcmp(s1, s2)

#endif


/* ===== Types ===== */

typedef char               cake_bool;   ///< Type sur 8 bits, utilisé principalement comme valeur de retour des fonctions pour indiquer si une erreur est survenue.
typedef unsigned char      uchar;
typedef unsigned short     ushort;
typedef unsigned int       uint;
typedef unsigned long      ulong;
typedef unsigned long long ulonglong;

typedef long long longlong;

typedef uchar cake_byte;                //< Type sur 8 bits.
typedef uchar cake_undefined_type;

typedef const char      *cchar_ptr;
typedef const short     *cshort_ptr;
typedef const int       *cint_ptr;
typedef const long      *clong_ptr;
typedef const long long *clonglong_ptr;

typedef const unsigned char      *cuchar_ptr;
typedef const unsigned short     *cushort_ptr;
typedef const unsigned int       *cuint_ptr;
typedef const unsigned long      *culong_ptr;
typedef const unsigned long long *culonglong_ptr;


/* ===== Defines ===== */

#define cake_true  1        ///< Constante désignant l'état vrai (1).
#define cake_false 0        ///< Constante désignant l'état faux (0).

#define CAKE_BUFF_SIZE 2048
#define CAKE_PTR(value) &value

#define CAKE_ERROR   -1
#define CAKE_NO_ERROR 0


/* ===== Structures ===== */
typedef struct cake_bytesbuffer {
    cake_byte *buffer;
    ulonglong size;
} Cake_BytesBuffer;

typedef struct cake_ulonglongarray {
    ulonglong *array;
    ulonglong length;
} Cake_UlonglongArray;

#endif