#ifndef __PIKA_OPTION_H__
#define __PIKA_OPTION_H__

#include "def.h"
#include "fdio.h"
#include "utf8.h"

typedef struct Option {
    String_UTF8 *key;
    String_UTF8 *value;
} Option;


/*
    Structure liée à FileOption.
*/
typedef struct FileOptionElement {
    Option *opt;
    // Index du début de la clé dans la source.
    ulonglong keyIndex;
    // Index du début de la valeur dans la source.
    ulonglong valueIndex;
} FileOptionElement;


/*
        Les éléments internes à cette structure ne doivent pas être modifiés.
*/
typedef struct FileOption {
    pika_fd fd;
    String_UTF8 *fileCopy;
    uchar delim;
} FileOption;


/*
        Charge l'option de la clé spécifiée (clé : valeur) délimitée par delim.
        delim ne doit pas être un caractère de plus de 1 octet.

        Retourne NULL si le fichier n'existe pas ou si la clé n'a pas été trouvée.

        Ne pas oublier de free_option lorsque la structure ne sert plus.
*/
Option *__load_option(String_UTF8 *source, const uchar *key, uchar delim, ulonglong *keyIndex, ulonglong *valueIndex);

#define load_option(source, key, delim) __load_option(source, key, delim, NULL, NULL)
void free_option(Option *opt);

/* ===== FileOption ===== */

FileOption *file_option_load(const uchar *filename, uchar delim);
FileOptionElement *file_option_get(FileOption *fileOpt, const uchar *key);
void free_file_option(FileOption *fileOpt);

#define free_file_option_element(e) free_option(e->opt)

#endif