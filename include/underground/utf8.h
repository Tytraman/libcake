#ifndef __PIKA_UTF8_H__
#define __PIKA_UTF8_H__

#include "def.h"
#include "utf16.h"

typedef struct UTF_Data {
    unsigned long long length;
    unsigned char byteSize;
} UTF_Data;

typedef struct String_UTF8 {
    UTF_Data data;
    unsigned long long length;
    unsigned char *bytes;
} String_UTF8;

/* ===== Initialisation ===== */

void create_strutf8(String_UTF8 *utf);
void strutf8_copy(String_UTF8 *dest, String_UTF8 *src);


/* ===== Ajout ===== */

int8 strutf8_add_wchar(String_UTF8 *dest, wchar_t value);
char strutf8_add_wchar_array(String_UTF8 *dest, wchar_t *str);
void strutf8_add_char(String_UTF8 *dest, char value, pika_bool increaseLength);
void strutf8_add_array_char(String_UTF8 *dest, char *str);
int8 strutf8_insert_wchar(String_UTF8 *utf, ulonglong index, wchar_t value);
void strutf8_insert_char(String_UTF8 *utf, ulonglong index, char value);
void strutf8_insert_char_internal(String_UTF8 *utf, ulonglong internalIndex, char value, pika_bool increaseLength);


/* ===== Suppression ===== */

pika_bool strutf8_remove_index(String_UTF8 *utf, ulonglong index);


/* ===== Conversion ===== */

// Convertit une chaîne UTF-16 en chaîne UTF-8.
void strutf8_to_utf16(String_UTF8 *src, String_UTF16 *dest);

/*
        Copie une chaîne de caractères dans une chaîne UTF-8,
        aucune conversion n'est effectuée, si la chaîne de caractères
        n'est pas de l'UTF-8, des effets indésirables peuvent survenir.
*/
void array_char_to_strutf8(char *src, String_UTF8 *dest);

// Convertit une suite d'octets en une valeur numérique Unicode.
int strutf8_decode(const unsigned char *src, char bytes);

/*
        Convertit un caractère UTF-16 en suite d'octets UTF-8.

        Retourne le nombre d'octets utilisés pour stocker le caractère.
*/
char strutf8_wchar_to_byte(wchar_t value, unsigned char **buffer);

/*
        Version avancée de strutf8_wchar_to_byte.

        Convertit un caractère UTF-16 en suite d'octets UTF-8 et
        les stocks directement dans le buffer destination.
*/
void strutf8_wchar_to_byte_ext(wchar_t value, unsigned char **buffer, unsigned long long *index);

// Convertit une chaîne UTF-16 en UTF-8.
void strutf16_to_strutf8(String_UTF16 *src, String_UTF8 *dest);

// Pas besoin d'utiliser create_strutf8.
// Par contre si cette fonction est utilisée plusieurs fois sur le même String_UTF8,
// alors il faut le free entre temps.
void wchar_array_to_strutf8(const wchar_t *src, String_UTF8 *dest);


/* ===== Cleaner ===== */

void clear_strutf8(String_UTF8 *utf);


/* ===== Recherches ===== */

/*
        Fonction interne !
        Cette fonction est une des premières qui a été faite et nécessite d'être refaite pour une utilisation plus simple.

        Permet de trouver l'adresse dans une chaîne UTF-8 par rapport à un index, étant donné qu'un caractère peut avoir plusieurs octets,
        faire utf[6] peut avoir des effets indésirables, cette fonction parcourt la chaîne et compte les index.
*/
ulonglong strutf8_index_by_index(unsigned char *pArrayStart, unsigned char *pArrayEnd, unsigned long long utfIndex, unsigned char **pStart, unsigned char **pEnd, int *bytes);

/*
        Recherche la sous-chaîne dans la chaîne UTF-8.

        Retourne l'adresse trouvée en cas de trouvaille,
        sinon NULL.
*/
unsigned char *strutf8_search_from_end(String_UTF8 *utf, unsigned char *research);

/*
        Recherche la sous-chaîne dans la chaîne UTF-8.

        Retourne l'adresse trouvée en cas de trouvaille,
        sinon NULL.

        internalIndex sera égal à la longueur de la recherche + 1.
*/
uchar *strutf8_search(String_UTF8 *utf, uchar *research, ulonglong*internalIndex);

pika_bool strutf8_equals(String_UTF8 *utf, char *compare);


/* ===== Vérificateurs ===== */

pika_bool strutf8_end_with(String_UTF8 *utf, unsigned char *str);
pika_bool strutf8_start_with(String_UTF8 *utf, unsigned char *research);
pika_bool str_starts_with(const char *src, const char *str);


/* ===== Création ===== */

unsigned long long strutf8_split_ptr(String_UTF8 *utf, String_UTF8 **dest, unsigned char *delim);

/* ===== Remplacement ===== */

/*
        Remplace toutes les occurences de la sous-chaîne dans la chaîne.

        Retourne le nombre d'occurences remplacées.
*/
ulonglong strutf8_replace_all(String_UTF8 *utf, uchar *old, uchar *replacement);


/* ===== Autres ===== */

unsigned long long strutf8_wchar_array_calc_size(wchar_t *str);
unsigned long long strutf8_length(String_UTF8 *utf);
unsigned long long str_count(char *str);

#endif