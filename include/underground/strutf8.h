#ifndef __CAKE_UTF8_H__
#define __CAKE_UTF8_H__

#include "def.h"
#include "strutf16.h"
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

/**
 * @brief Crée dynamiquement un `Cake_String_UTF8_Pair`.
 * @warning Ne pas oublier de free avec `cake_free_strutf8_pair`.
 * 
 * @param key Chaîne 1 à copier.
 * @param value Chaîne 2 à copier.
 * @return La paire de chaînes créée.
 */
Cake_String_UTF8_Pair *cake_strutf8_pair(const uchar *key, const uchar *value);

/**
 * @brief Libère la mémoire d'un `Cake_String_UTF8_Pair`.
 * 
 * @param pair La paire de chaînes à libérer.
 */
void cake_free_strutf8_pair(Cake_String_UTF8_Pair *pair);

/*
        Initialise toutes les valeurs de la structure Cake_String_UTF8.

        A utiliser une seule fois après chaque déclaration d'une variable Cake_String_UTF8,
        sauf cas exceptionnels où certaines fonctions le spécifie.
*/

/**
 * @deprecated Utiliser cake_strutf8 à la place.
 * 
 * @brief Initialise un `Cake_String_UTF8`.
 * 
 * @param utf Chaîne à initialiser.
 */
void cake_create_strutf8(Cake_String_UTF8 *utf);

/**
 * @brief Crée dynamiquement un `Cake_String_UTF8`.
 * @warning Ne pas oublier de free avec `cake_free_strutf8`.
 * 
 * @param value Chaîne à copier.
 * @return La chaîne créée.
 */
Cake_String_UTF8 *cake_strutf8(const uchar *value);

/**
 * @brief Crée dynamiquement une `Cake_List_String_UTF8`, ne pas oublier de free.
 * 
 * @return La liste créée.
 */
Cake_List_String_UTF8 *cake_list_strutf8();

/**
 * @brief Copie une chaîne vers une autre.
 * 
 * @param dest Chaîne destination.
 * @param src Chaîne source.
 */
void cake_strutf8_copy(Cake_String_UTF8 *dest, Cake_String_UTF8 *src);


/* ===== Ajout ===== */

/**
 * @brief Ajoute un caractère unicode dans la chaîne UTF-8.
 * 
 * @param dest La chaîne destination.
 * @param value Le caractère unicode.
 * @return Le nombre d'octets UTF-8 utilisés pour stocker le caractère.
 */
uchar cake_strutf8_add_wchar(Cake_String_UTF8 *dest, wchar_t value);

/**
 * @brief Ajoute une chaîne de caractères unicode dans la chaîne UTF-8.
 * 
 * @param dest La chaîne destination.
 * @param str La chaîne unicode à ajouter.
 */
void cake_strutf8_add_wchar_array(Cake_String_UTF8 *dest, const wchar_t *str);

/**
 * @brief Ajoute une chaîne de caractères dans la chaîne UTF-8.
 * 
 * @param dest 
 * @param str 
 */
void cake_strutf8_add_char_array(Cake_String_UTF8 *dest, const uchar *str);

/*
        

        
*/

/**
 * @brief Insère un caractère unicode dans la chaîne UTF-8.
 * 
 * @param utf La chaîne destination.
 * @param index L'index auquel insérer le caractère.
 * @param value Le caractère unicode à insérer.
 * @return Le nombre d'octets UTF-8 utilisés pour stocker le caractère.
 */
uchar cake_strutf8_insert_wchar(Cake_String_UTF8 *utf, ulonglong index, wchar_t value);

/**
 * @brief Insère une chaîne de caractères dans la chaîne UTF-8.
 * 
 * @param utf La chaîne destination.
 * @param index L'index auquel insérer la chaîne de caractères.
 * @param str La chaîne de caractères à insérer.
 * @return `cake_true` si la chaîne à bien été insérée.
 */
cake_bool cake_strutf8_insert_char_array(Cake_String_UTF8 *utf, ulonglong index, const uchar *str);

/**
 * @brief Ajoute une chaîne de caractères dans une `Cake_List_String_UTF8`.
 * 
 * @param list La liste destination.
 * @param str La chaîne de caractères à ajouter.
 */
void cake_list_strutf8_add_char_array(Cake_List_String_UTF8 *list, const uchar *str);


/* ===== Modification ===== */

/**
 * @brief Inverse une chaîne UTF-8, par exemple "Hello World" deviendra "dlroW olleH".
 * 
 * @param utf La chaîne à inverser.
 */
void cake_strutf8_reverse(Cake_String_UTF8 *utf);

/* ===== Suppression ===== */

/**
 * @brief Supprime le caractère UTF-8 stocké à l'index passé.
 * 
 * @param utf La chaîne destination.
 * @param index L'index auquel supprimer le caractère.
 * @return `cake_false` si l'index est supérieur au nombre de caractères.
 */
cake_bool cake_strutf8_remove_index(Cake_String_UTF8 *utf, ulonglong index);

/**
 * @brief Supprime toutes les occurences de la sous-chaîne passée.
 * 
 * @param utf La chaîne destination.
 * @param value La sous-chaîne à supprimer.
 * @return Le nombre d'occurences supprimées.
 */
ulonglong cake_strutf8_remove_all(Cake_String_UTF8 *utf, const uchar *value);

/**
 * @brief Supprime le début d'une chaîne UTF-8 si celle-ci commence par la sous-chaîne passée.
 * 
 * @param utf La chaîne UTF-8.
 * @param value Sous-chaîne à enlever.
 * @return `cake_true` si la sous-chaîne a été supprimée.
 */
cake_bool cake_strutf8_remove_start(Cake_String_UTF8 *utf, const uchar *value);

/**
 * @brief Supprime tous les caractères entre 2 positions.
 * 
 * @param utf La Chaîne UTF-8.
 * @param fromIndex Position de départ (compris).
 * @param toIndex Position de fin (non compris).
 * @return `cake_false` si un des deux index est supérieur au nombre de caractères ou que `fromIndex` >= `toIndex`.
 */
cake_bool cake_strutf8_remove_from_to(Cake_String_UTF8 *utf, ulonglong fromIndex, ulonglong toIndex);

/**
 * @brief Supprime tous les caractères entre 2 positions internes.
 * @warning Cette fonction est notamment utilisée par `libcake`.
 * 
 * @param utf La chaîne UTF-8.
 * @param fromIndex Position de départ (compris).
 * @param toIndex Position de fin (non compris).
 * @return `cake_false` si un des deux index est supérieur au nombre de caractères ou que `fromIndex` >= `toIndex`.
 */
cake_bool cake_strutf8_remove_from_to_internal(Cake_String_UTF8 *utf, ulonglong fromIndex, ulonglong toIndex);


/* ===== Conversion ===== */


/**
 * @brief Convertit une chaîne UTF-8 en chaîne UTF-16.
 * 
 * @param src Chaîne UTF-8 à convertir.
 * @param dest Chaîne destination.
 */
void cake_strutf8_to_utf16(Cake_String_UTF8 *src, Cake_String_UTF16 *dest);

/*
        
*/

/**
 * @brief Copie une chaîne de caractères dans une chaîne UTF-8,
 * aucune conversion n'est effectuée,
 * si la chaîne de caractères n'est pas de l'UTF-8, des effets indésirables peuvent survenir.
 * 
 * @param src Chaîne de caractères à copier.
 * @param dest Chaîne destination.
 */
void cake_char_array_to_strutf8(const uchar *src, Cake_String_UTF8 *dest);

/**
 * @brief Convertit une suite d'octets en une valeur numérique unicode.
 * 
 * @param src La suite d'octets à convertir.
 * @param bytes Le nombre d'octets à convertir.
 * @return La valeur obtenue après conversion.
 */
int cake_strutf8_decode(const uchar *src, char bytes);

/**
 * @brief Convertit un caractère UTF-16 en suite d'octets UTF-8.
 * @warning `buffer` est dynamiquement alloué avec malloc, ne pas oublier de free quand il ne sert plus.
 * 
 * @param value La valeur à convertir.
 * @param buffer Buffer qui stockera la suite d'octets.
 * @return Le nombre d'octets utilisés pour stocker la suite.
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

/**
 * @brief Recherche la sous-chaîne dans la chaîne UTF-8 à partir de `internalIndex` en allant vers le début de la chaîne.
 * 
 * @param utf Chaîne UTF-8 dans laquelle chercher.
 * @param research Sous-chaîne de caractères à rechercher.
 * @param internalIndex Pointeur vers la valeur du début de la recherche, après traitement, sera égale à sa valeur - la longueur de la recherche - 1.
 * @return L'adresse de la première occurence trouvée ou NULL en cas de non trouvaille.
 */
uchar *cake_strutf8_search_from_end(Cake_String_UTF8 *utf, const uchar *research, ulonglong *internalIndex);

/**
 * @brief Recherche la sous-chaîne dans la chaîne UTF-8 à partir de `internalIndex` en allant vers la fin de la chaîne.
 * 
 * @param utf Chaîne UTF-8 dans laquelle chercher.
 * @param research Sous-chaîne de caractères à rechercher.
 * @param internalIndex Pointeur vers la valeur du début de la recherche, après traitement, sera égale à sa valeur + la longueur de la recherche + 1.
 * @return L'adresse de la première occurence trouvée ou NULL en cas de non trouvaille.
 */
uchar *cake_strutf8_search_from_start(Cake_String_UTF8 *utf, const uchar *research, ulonglong *internalIndex);

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


/**
 * @brief Sépare les éléments d'une chaîne de caractères.
 * 
 * @param utf Chaîne de caractères.
 * @param delim Délimiteur des éléments.
 * @return La liste des éléments séparés.
 */
Cake_List_String_UTF8 *cake_strutf8_split(Cake_String_UTF8 *utf, const uchar *delim);

/* ===== Remplacement ===== */


/**
 * @brief Remplace toutes les occurences de la sous-chaîne dans la chaîne.
 * 
 * @param utf Chaîne de caractères.
 * @param old Sous-chaîne à remplacer.
 * @param replacement Nouvelle sous-chaîne.
 * @return Le nombre de sous-chaînes remplacées.
 */
ulonglong cake_strutf8_replace_all(Cake_String_UTF8 *utf, const uchar *old, const uchar *replacement);

/**
 * @brief Remplace le début de la chaîne par `replacement` si elle commence par `old`.
 * 
 * @param utf Chaîne de caractères.
 * @param old Sous-chaîne à remplacer.
 * @param replacement Nouvelle sous-chaîne.
 * @return cake_true si la sous-chaîne a été modifiée.
 */
cake_bool cake_strutf8_replace_start(Cake_String_UTF8 *utf, const uchar *old, const uchar *replacement);

/**
 * @brief Remplace la fin de la chaîne par `replacement` si elle fini par `old`.
 * 
 * @param utf Chaîne de caractères.
 * @param old Sous-chaîne à remplacer.
 * @param replacement Nouvelle sous-chaîne.
 * @return cake_true si la sous-chaîne a été modifiée.
 */
cake_bool cake_strutf8_replace_end(Cake_String_UTF8 *utf, const uchar *old, const uchar *replacement);

/**
 * @brief Remplace la première occurence de la chaîne de caractères passée.
 * 
 * @param utf La chaîne UTF-8.
 * @param old Sous-chaîne à remplacer.
 * @param replacement Nouvelle sous-chaîne.
 * @return `cake_true` si la sous-chaîne a été remplacée.
 */
cake_bool cake_strutf8_replace_from_start(Cake_String_UTF8 *utf, const uchar *old, const uchar *replacement);


cake_bool cake_strutf8_replace_from_end(Cake_String_UTF8 *utf, const uchar *old, const uchar *replacement);

/**
 * @brief Tous les caractères de la chaîne deviennent des minuscules.
 * 
 * @param utf Chaîne de caractères.
 */
void cake_strutf8_to_lower(Cake_String_UTF8 *utf);


/* ===== Autres ===== */

ulonglong cake_strutf8_wchar_array_calc_size(const wchar_t *str);

/**
 * @brief Calcule le nombre de caractères d'une chaîne UTF-8.
 * 
 * @param utf La chaîne UTF-8.
 * @return Le nombre de caractères de la chaîne.
 */
ulonglong cake_strutf8_length(Cake_String_UTF8 *utf);

// Equivalent de strlen.
ulonglong cake_str_count(const uchar *str);

uint cake_str_hex_to_uint(const uchar *str);

void cake_strutf8_decode_url(Cake_String_UTF8 *utf);

/**
 * @brief Convertie une valeur décimale en chaîne de caractères.
 * @warning `buffer` est dynamiquement alloué avec malloc, ne pas oublier de free quand il ne sert plus.
 * 
 * @param value Valeur à convertir.
 * @param buffer Buffer qui stockera la valeur convertie.
 * @return Le nombre d'octets utilisés pour stocker la valeur.
 */
uchar cake_strutf8_dec_to_char(uint value, uchar **buffer);

/*
        
*/

/**
 * @brief Stock une valeur décimale convertie en hexadécimale dans un buffer.
 * @warning Aucun caractère NULL n'est mis à la fin !
 * 
 * @param value Valeur à convertir.
 * @param dest Buffer destination.
 */
void cake_str_dec_to_hexchar(uchar value, uchar dest[2]);

/**
 * @brief Vérifie qu'une chaîne de caractères soit strictement égale à une autre.
 * 
 * @param str1 Chaîne 1.
 * @param str2 Chaîne 2.
 * @return `cake_true` si les chaînes de caractères sont identiques.
 */
cake_bool cake_str_equals(const uchar *str1, const uchar *str2);

/**
 * @brief Compte le nombre de fois qu'un caractère apparaît dans une chaîne de caractères.
 * 
 * @param str Chaîne à parcourir.
 * @param value Caractère à compter.
 * @return Le nombre de fois que le caractère à été trouvé.
 */
ulonglong cake_str_number_of(const uchar *str, uchar value);


void cake_strutf8_vector_delete_callback(void *args);
void cake_strutf8_vector_delete_callback_ptr(void *args);

Cake_String_UTF8 *cake_strutf8_substring(Cake_String_UTF8 *from, ulonglong startIndex, ulonglong endIndex);

#endif