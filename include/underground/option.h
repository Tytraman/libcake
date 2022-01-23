#ifndef __PIKA_OPTION_H__
#define __PIKA_OPTION_H__

#include "def.h"
#include "utf8.h"

typedef struct Option {
    String_UTF8 *key;
    String_UTF8 *value;
} Option;


/*
        Charge l'option de la clé spécifiée (clé : valeur) délimitée par delim.
        delim ne doit pas être un caractère de plus de 1 octet.

        Retourne NULL si le fichier n'existe pas ou si la clé n'a pas été trouvée.

        Ne pas oublier de free_option lorsque la structure ne sert plus.
*/
Option *load_option(String_UTF8 *source, const uchar *key, uchar delim);


/*
        Attribut une clé : valeur à une option.

        Si *opt est NULL, il sera créé avec malloc.
*/
void set_option(Option **opt, const uchar *key, const uchar *value);

void free_option(Option *opt);

#endif