#ifndef __CAKE_OPTION_H__
#define __CAKE_OPTION_H__

#include "def.h"
#include "fdio.h"
#include "strutf8.h"


/*
        Obsolète !
        Ancienne version de Cake_FileObject !
*/



typedef struct cake_option {
    Cake_String_UTF8 *key;
    Cake_String_UTF8 *value;
} Cake_Option;


/*
    Structure liée à Cake_FileOption.
*/
typedef struct cake_fileoptionelement {
    Cake_Option *opt;
    // Index du début de la clé dans la source.
    ulonglong keyIndex;
    // Index du début de la valeur dans la source.
    ulonglong valueIndex;
} Cake_FileOptionElement;


/*
        Les éléments internes à cette structure ne doivent pas être modifiés.
*/
typedef struct cake_fileoption {
    cake_fd fd;
    Cake_String_UTF8 *fileCopy;
    uchar delim;
} Cake_FileOption;


/*
        Charge l'option de la clé spécifiée (clé : valeur) délimitée par delim.
        delim ne doit pas être un caractère de plus de 1 octet.

        Retourne NULL si le fichier n'existe pas ou si la clé n'a pas été trouvée.

        Ne pas oublier de cake_free_option lorsque la structure ne sert plus.
*/
Cake_Option *__cake_load_option(Cake_String_UTF8 *source, const char *key, char delim, ulonglong *keyIndex, ulonglong *valueIndex);

#define cake_load_option(source, key, delim) __cake_load_option(source, key, delim, NULL, NULL)
void cake_free_option(Cake_Option *opt);

/* ===== Cake_FileOption ===== */

Cake_FileOption *cake_file_option_load(const char *filename, char delim);
Cake_FileOptionElement *cake_file_option_get(Cake_FileOption *fileOpt, const char *key);
void cake_free_file_option(Cake_FileOption *fileOpt);

#define cake_free_file_option_element(e) cake_free_option(e->opt)

#endif