#ifndef __PIKA_UTF8_H__
#define __PIKA_UTF8_H__

#include "def.h"
#include "utf16.h"

// Données internes utilisées par String_UTF8.
typedef struct UTF_Data {
    ulonglong length;
} UTF_Data;

typedef struct String_UTF8 {
    UTF_Data data;
    unsigned long long length;
    unsigned char *bytes;
} String_UTF8;

/* ===== Initialisation ===== */

/*
        Initialise toutes les valeurs de la structure String_UTF8.

        A utiliser une seule fois après chaque déclaration d'une variable String_UTF8,
        sauf cas exceptionnels où certaines fonctions le spécifie.
*/
void create_strutf8(String_UTF8 *utf);

String_UTF8 *strutf8(const uchar *value);

// Copie une chaîne UTF-8 vers une autre.
void strutf8_copy(String_UTF8 *dest, String_UTF8 *src);


/* ===== Ajout ===== */


/*
        Ajoute un caractère unicode dans la chaîne UTF-8.

        Retourne le nombre d'octets UTF-8 utilisés pour stocker le caractère.
*/
int8 strutf8_add_wchar(String_UTF8 *dest, wchar_t value);

/*
        Ajoute une chaîne de caractères unicode dans la chaîne UTF-8.
*/
char strutf8_add_wchar_array(String_UTF8 *dest, const wchar_t *str);

/*
        Ajoute une chaîne de caractères dans la chaîne UTF-8.
        La chaîne peut contenir des caractères UTF-8 voir même uniquement un caractère UTF-8.
*/
void strutf8_add_char_array(String_UTF8 *dest, const uchar *str);

/*
        Insère un caractère unicode dans la chaîne UTF-8.

        Retourne le nombre d'octets UTF-8 utilisés pour stocker le caractère.
*/
int8 strutf8_insert_wchar(String_UTF8 *utf, ulonglong index, wchar_t value);

pika_bool strutf8_insert_char_array(String_UTF8 *utf, ulonglong index, const uchar *str);


/* ===== Modification ===== */

void strutf8_reverse(String_UTF8 *utf);

/* ===== Suppression ===== */

/*
        Supprime le caractère UTF-8 stocké à l'index passé.

        Retourne pika_false si l'index est supérieur au nombre de caractères.
*/
pika_bool strutf8_remove_index(String_UTF8 *utf, ulonglong index);


/* ===== Conversion ===== */

// Convertit une chaîne UTF-8 en chaîne UTF-16.
void strutf8_to_utf16(String_UTF8 *src, String_UTF16 *dest);

/*
        Copie une chaîne de caractères dans une chaîne UTF-8,
        aucune conversion n'est effectuée, si la chaîne de caractères
        n'est pas de l'UTF-8, des effets indésirables peuvent survenir.
*/
void array_char_to_strutf8(const uchar *src, String_UTF8 *dest);

// Convertit une suite d'octets en une valeur numérique Unicode.
int strutf8_decode(const uchar *src, char bytes);

/*
        Convertit un caractère UTF-16 en suite d'octets UTF-8.

        Retourne le nombre d'octets utilisés pour stocker le caractère.
*/
char strutf8_wchar_to_byte(wchar_t value, uchar **buffer);

/*
        Version avancée de strutf8_wchar_to_byte.

        Convertit un caractère UTF-16 en suite d'octets UTF-8 et
        les stocks directement dans le buffer destination.
*/
void strutf8_wchar_to_byte_ext(wchar_t value, uchar **buffer, ulonglong *index);

// Convertit une chaîne UTF-16 en UTF-8.
void strutf16_to_strutf8(String_UTF16 *src, String_UTF8 *dest);

// Pas besoin d'utiliser create_strutf8.
// Par contre si cette fonction est utilisée plusieurs fois sur le même String_UTF8,
// alors il faut le free entre temps.
void wchar_array_to_strutf8(const wchar_t *src, String_UTF8 *dest);


/* ===== Cleaner ===== */

// Nettoie la chaîne UTF-8 en utilisant free et en remettant les valeurs à 0 et NULL.
void clear_strutf8(String_UTF8 *utf);


/*
        Libère la mémoire d'un String_UTF8 dynamique.

        Ne met pas les valeurs à 0 ni à NULL, la fonction n'effectue que des free.
*/
void free_strutf8(String_UTF8 *utf);


/* ===== Recherches ===== */

/*
        Fonction interne !
        Cette fonction est une des premières qui a été faite et nécessite d'être refaite pour une utilisation plus simple.

        Permet de trouver l'adresse dans une chaîne UTF-8 par rapport à un index, étant donné qu'un caractère peut avoir plusieurs octets,
        faire utf[6] peut avoir des effets indésirables, cette fonction parcourt la chaîne et compte les index.
*/
ulonglong strutf8_index_by_index(uchar *pArrayStart, uchar *pArrayEnd, ulonglong utfIndex, uchar **pStart, uchar **pEnd, int *bytes);

/*
        Recherche la sous-chaîne dans la chaîne UTF-8 à partir de la fin.

        Retourne l'adresse de la première occurence trouvée ou NULL en cas de non trouvaille.

        internalIndex sera égal à sa valeur - le nombre de recherches - 1.
*/
uchar *strutf8_search_from_end(String_UTF8 *utf, const uchar *research, ulonglong *internalIndex);

/*
        Recherche la sous-chaîne dans la chaîne UTF-8 à partir de internalIndex en allant vers la fin de la chaîne.

        Retourne l'adresse de la première occurence trouvée ou NULL en cas de non trouvaille.

        internalIndex sera égal à sa valeur + la longueur de la recherche + 1.
*/
uchar *strutf8_search(String_UTF8 *utf, const uchar *research, ulonglong *internalIndex);


/* ===== Vérificateurs ===== */

// Vérifie que la chaîne UTF-8 termine avec la sous-chaîne passée.
pika_bool strutf8_end_with(String_UTF8 *utf, const uchar *str);
// Vérifie que la chaîne UTF-8 commence avec la sous-chaîne passée.
pika_bool strutf8_start_with(String_UTF8 *utf, const uchar *research);

// Vérifie que la chaîne de caractères commence avec la sous-chaîne passée.
pika_bool str_starts_with(const uchar *src, const uchar *str);

// Vérifie que la chaîne UTF-8 soit exactement égale à la chaîne de caractères passée.
pika_bool strutf8_equals(String_UTF8 *utf, const uchar *compare);


/* ===== Création ===== */

/*
        TODO: strutf8_split_ptr -> refaire la fonction, copier les valeurs dans des sous String_UTF8
*/
ulonglong strutf8_split_ptr(String_UTF8 *utf, String_UTF8 **dest, const uchar *delim);

/* ===== Remplacement ===== */

/*
        Remplace toutes les occurences de la sous-chaîne dans la chaîne.

        Retourne le nombre d'occurences remplacées.
*/
ulonglong strutf8_replace_all(String_UTF8 *utf, const uchar *old, const uchar *replacement);


/* ===== Autres ===== */

unsigned long long strutf8_wchar_array_calc_size(const wchar_t *str);

// Calcule la longueur d'une chaîne UTF-8 avec un algorithme.
ulonglong strutf8_length(String_UTF8 *utf);

// Equivalent de strlen.
ulonglong str_count(const uchar *str);

#endif