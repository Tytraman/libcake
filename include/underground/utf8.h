#ifndef __CAKE_UTF8_H__
#define __CAKE_UTF8_H__

#include "def.h"
#include "utf16.h"
#include "array.h"

// Données internes utilisées par Cake_String_UTF8.
typedef struct cake_utf_data {
    ulonglong length;
} Cake_UTF_Data;

typedef struct cake_string_utf8 {
    Cake_UTF_Data data;
    ulonglong length;
    uchar *bytes;
} Cake_String_UTF8;

typedef struct cake_list_string_utf8 {
    Cake_ArrayList data;
    Cake_String_UTF8 **list;
} Cake_List_String_UTF8;

typedef struct cake_string_utf8_pair {
    Cake_String_UTF8 *key;
    Cake_String_UTF8 *value;
} Cake_String_UTF8_Pair;

typedef struct cake_linkedlist_string_utf8_pair {
    Cake_String_UTF8_Pair *pair;
    struct cake_linkedlist_string_utf8_pair *next;
} Cake_LinkedList_String_UTF8_Pair;

/* ===== Initialisation ===== */

Cake_String_UTF8_Pair *cake_strutf8_pair(const uchar *key, const uchar *value);
void cake_free_strutf8_pair(Cake_String_UTF8_Pair *pair);

/*
        Initialise toutes les valeurs de la structure Cake_String_UTF8.

        A utiliser une seule fois après chaque déclaration d'une variable Cake_String_UTF8,
        sauf cas exceptionnels où certaines fonctions le spécifie.
*/
void cake_create_strutf8(Cake_String_UTF8 *utf);

Cake_String_UTF8 *cake_strutf8(const uchar *value);

Cake_List_String_UTF8 *cake_list_strutf8();

// Copie une chaîne UTF-8 vers une autre.
void cake_strutf8_copy(Cake_String_UTF8 *dest, Cake_String_UTF8 *src);


/* ===== Ajout ===== */


/*
        Ajoute un caractère unicode dans la chaîne UTF-8.

        Retourne le nombre d'octets UTF-8 utilisés pour stocker le caractère.
*/
uchar cake_strutf8_add_wchar(Cake_String_UTF8 *dest, wchar_t value);

/*
        Ajoute une chaîne de caractères unicode dans la chaîne UTF-8.
*/
void cake_strutf8_add_wchar_array(Cake_String_UTF8 *dest, const wchar_t *str);

/*
        Ajoute une chaîne de caractères dans la chaîne UTF-8.
        La chaîne peut contenir des caractères UTF-8 voir même uniquement un caractère UTF-8.
*/
void cake_strutf8_add_char_array(Cake_String_UTF8 *dest, const uchar *str);

/*
        Insère un caractère unicode dans la chaîne UTF-8.

        Retourne le nombre d'octets UTF-8 utilisés pour stocker le caractère.
*/
uchar cake_strutf8_insert_wchar(Cake_String_UTF8 *utf, ulonglong index, wchar_t value);

cake_bool cake_strutf8_insert_char_array(Cake_String_UTF8 *utf, ulonglong index, const uchar *str);

void cake_list_strutf8_add_char_array(Cake_List_String_UTF8 *list, const uchar *str);


/* ===== Modification ===== */

void cake_strutf8_reverse(Cake_String_UTF8 *utf);

/* ===== Suppression ===== */

/*
        Supprime le caractère UTF-8 stocké à l'index passé.

        Retourne cake_false si l'index est supérieur au nombre de caractères.
*/
cake_bool cake_strutf8_remove_index(Cake_String_UTF8 *utf, ulonglong index);

ulonglong cake_strutf8_remove_all(Cake_String_UTF8 *utf, const uchar *value);

cake_bool cake_strutf8_remove_start(Cake_String_UTF8 *utf, const uchar *value);

/*
        Supprime tous les caractères à partir de fromIndex (compris) jusqu'à toIndex (non compris).

        Retourne cake_false si un des deux index est supérieur au nombre de caractères.
*/
cake_bool cake_strutf8_remove_from_to(Cake_String_UTF8 *utf, ulonglong fromIndex, ulonglong toIndex);

cake_bool cake_strutf8_remove_from_to_internal(Cake_String_UTF8 *utf, ulonglong fromIndex, ulonglong toIndex);


/* ===== Conversion ===== */

// Convertit une chaîne UTF-8 en chaîne UTF-16.
void cake_strutf8_to_utf16(Cake_String_UTF8 *src, Cake_String_UTF16 *dest);

/*
        Copie une chaîne de caractères dans une chaîne UTF-8,
        aucune conversion n'est effectuée, si la chaîne de caractères
        n'est pas de l'UTF-8, des effets indésirables peuvent survenir.
*/
void cake_char_array_to_strutf8(const uchar *src, Cake_String_UTF8 *dest);

// Convertit une suite d'octets en une valeur numérique Unicode.
int cake_strutf8_decode(const uchar *src, char bytes);

/*
        Convertit un caractère UTF-16 en suite d'octets UTF-8.

        Retourne le nombre d'octets utilisés pour stocker le caractère.
*/
uchar cake_strutf8_wchar_to_byte(wchar_t value, uchar **buffer);

/*
        Version avancée de cake_strutf8_wchar_to_byte.

        Convertit un caractère UTF-16 en suite d'octets UTF-8 et
        les stocks directement dans le buffer destination.
*/
void cake_strutf8_wchar_to_byte_ext(wchar_t value, uchar **buffer, ulonglong *index);

// Convertit une chaîne UTF-16 en UTF-8.
void cake_strutf16_to_strutf8(Cake_String_UTF16 *src, Cake_String_UTF8 *dest);

// Pas besoin d'utiliser cake_create_strutf8.
// Par contre si cette fonction est utilisée plusieurs fois sur le même Cake_String_UTF8,
// alors il faut le free entre temps.
void cake_wchar_array_to_strutf8(const wchar_t *src, Cake_String_UTF8 *dest);


ulonglong cake_strutf8_to_ulonglong(Cake_String_UTF8 *utf);

void cake_ulonglong_to_char_array(ulonglong value, uchar *buffer);

/* ===== Cleaner ===== */

// Nettoie la chaîne UTF-8 en utilisant free et en remettant les valeurs à 0 et NULL.
void cake_clear_strutf8(Cake_String_UTF8 *utf);


/*
        Libère la mémoire d'un Cake_String_UTF8 dynamique.

        Ne met pas les valeurs à 0 ni à NULL, la fonction n'effectue que des free.
*/
void cake_free_strutf8(Cake_String_UTF8 *utf);

void cake_free_list_strutf8(Cake_List_String_UTF8 *list);


/* ===== Recherches ===== */

/*
        Fonction interne !
        Cette fonction est une des premières qui a été faite et nécessite d'être refaite pour une utilisation plus simple.

        Permet de trouver l'adresse dans une chaîne UTF-8 par rapport à un index, étant donné qu'un caractère peut avoir plusieurs octets,
        faire utf[6] peut avoir des effets indésirables, cette fonction parcourt la chaîne et compte les index.
*/
ulonglong cake_strutf8_index_by_index(const uchar *pArrayStart, uchar *pArrayEnd, ulonglong utfIndex, uchar **pStart, uchar **pEnd, int *bytes);

ulonglong cake_strutf8_index_by_index_reverse(Cake_String_UTF8 *utf, ulonglong utfIndex, cake_byte *bytes);

/*
        Recherche la sous-chaîne dans la chaîne UTF-8 à partir de la fin.

        Retourne l'adresse de la première occurence trouvée ou NULL en cas de non trouvaille.

        internalIndex sera égal à sa valeur - le nombre de recherches - 1.
*/
uchar *cake_strutf8_search_from_end(Cake_String_UTF8 *utf, const uchar *research, ulonglong *internalIndex);

/*
        Recherche la sous-chaîne dans la chaîne UTF-8 à partir de internalIndex en allant vers la fin de la chaîne.

        Retourne l'adresse de la première occurence trouvée ou NULL en cas de non trouvaille.

        internalIndex sera égal à sa valeur + la longueur de la recherche + 1.
*/
uchar *cake_strutf8_search(Cake_String_UTF8 *utf, const uchar *research, ulonglong *internalIndex);

ulonglong cake_str_search(const uchar *str, uchar value, uchar **ptr);

uchar *cake_str_search_array(const uchar *str, const uchar *value);

/* ===== Vérificateurs ===== */

// Vérifie que la chaîne UTF-8 termine avec la sous-chaîne passée.
cake_bool cake_strutf8_end_with(Cake_String_UTF8 *utf, const uchar *str);
// Vérifie que la chaîne UTF-8 commence avec la sous-chaîne passée.
cake_bool cake_strutf8_start_with(Cake_String_UTF8 *utf, const uchar *research);

// Vérifie que la chaîne de caractères commence avec la sous-chaîne passée.
cake_bool cake_str_starts_with(const uchar *src, const uchar *str);

// Vérifie que la chaîne UTF-8 soit exactement égale à la chaîne de caractères passée.
cake_bool cake_strutf8_equals(const Cake_String_UTF8 *utf, const uchar *compare);


/* ===== Création ===== */


Cake_List_String_UTF8 *cake_strutf8_split(Cake_String_UTF8 *utf, const uchar *delim);

/* ===== Remplacement ===== */

/*
        Remplace toutes les occurences de la sous-chaîne dans la chaîne.

        Retourne le nombre d'occurences remplacées.
*/
ulonglong cake_strutf8_replace_all(Cake_String_UTF8 *utf, const uchar *old, const uchar *replacement);

cake_bool cake_strutf8_replace_from_end(Cake_String_UTF8 *utf, const uchar *old, const uchar *replacement);

void cake_strutf8_to_lower(Cake_String_UTF8 *utf);


/* ===== Autres ===== */

ulonglong cake_strutf8_wchar_array_calc_size(const wchar_t *str);

// Calcule la longueur d'une chaîne UTF-8 avec un algorithme.
ulonglong cake_strutf8_length(Cake_String_UTF8 *utf);

// Equivalent de strlen.
ulonglong cake_str_count(const uchar *str);

uint cake_str_hex_to_uint(const uchar *str);

void cake_strutf8_decode_url(Cake_String_UTF8 *utf);

/*
        Convertie une valeur décimale en chaîne de caractères.

        buffer est dynamiquement alloué grâce à malloc, ne pas oublier de free quand il ne sert plus.
*/
uchar cake_strutf8_dec_to_char(uint value, uchar **buffer);

/*
        Stock une valeur décimale convertie en hexadécimale dans un buffer,
        aucun caractère NULL n'est mis à la fin !
*/
void cake_str_dec_to_hexchar(uchar value, uchar dest[2]);

cake_bool cake_str_equals(const uchar *str1, const uchar *str2);

ulonglong cake_str_number_of(const uchar *str, uchar value);


void cake_strutf8_vector_delete_callback(void *args);
void cake_strutf8_vector_delete_callback_ptr(void *args);

#endif