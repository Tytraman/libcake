/**
* @file strutf8.h
* @brief Fichier contenant le prototypes de tout ce qui touche aux chaînes de caractères UTF-8.
* @author Tytraman
*/

#ifndef __CAKE_UTF8_H__
#define __CAKE_UTF8_H__

/// @cond
#include "def.h"
#include "strutf16.h"
#include "array.h"
/// @endcond

/**
 * @brief Chaînes de caractères encodées en UTF-8.
*/
typedef struct cake_string_utf8 {
    uchar *bytes;
    ulonglong size;
    ulonglong length;
} Cake_String_UTF8;

/**
 * @brief Tableau dynamique de chaînes de caractères encodées en UTF-8.
*/
typedef struct cake_list_string_utf8 {
    Cake_ArrayList data;
    Cake_String_UTF8 **list;
} Cake_List_String_UTF8;

/**
 * @brief Associe 2 chaînes de caractères encodées en UTF-8.
*/
typedef struct cake_string_utf8_pair {
    Cake_String_UTF8 *key;
    Cake_String_UTF8 *value;
} Cake_String_UTF8_Pair;

/**
 * @brief Listes liées de chaînes de caractères encodées en UTF-8.
*/
typedef struct cake_linkedlist_string_utf8_pair {
    Cake_String_UTF8_Pair *pair;
    struct cake_linkedlist_string_utf8_pair *next;
} Cake_LinkedList_String_UTF8_Pair;

/**
 * @brief Lit le contenu d'une chaîne de caractères encodées en UTF-8.
*/
typedef struct cake_strutf8_reader {
    Cake_String_UTF8 *utf;
    ulonglong pos;
} Cake_String_UTF8_Reader;

/* =============================== */
/* ========== FONCTIONS ========== */
/* =============================== */

/**
* @deprecated Utiliser \ref cake_strutf8 à la place.
*
* @brief Initialise un \ref Cake_String_UTF8.
*
* @param[out] utf - Chaîne à initialiser.
*/
CAKE_C CAKE_API void cake_create_strutf8(Cake_String_UTF8 *utf);

/**
* @brief Crée dynamiquement un \ref Cake_String_UTF8.
* @warning Ne pas oublier de libérer la mémoire avec \ref cake_free_strutf8.
*
* @param[in] value - Chaîne de caractères à copier.
* 
* @return \ref Cake_String_UTF8 * - La chaîne créée.
* @return `NULL` en cas d'erreur.
*/
CAKE_C CAKE_API Cake_String_UTF8 *cake_strutf8(const char *value);

/**
* @brief Libère la mémoire d'un \ref Cake_String_UTF8 alloué dynamiquement.
* 
* @param utf - La chaîne de caractères à faire disparaitre.
*/
CAKE_C CAKE_API void cake_free_strutf8(Cake_String_UTF8 *utf);


/**
 * @brief Crée un lecteur de \ref Cake_String_UTF8.
 * @param[out] dest - Pointeur dans lequel initialiser les données, ne doit pas être `NULL`.
 * @param[in] str - Chaîne de caractères à copier dans le lecteur.
*/
CAKE_C CAKE_API void cake_create_strutf8_reader_str(Cake_String_UTF8_Reader *dest, const char *str);

/**
 * @brief Crée un lecteur de \ref Cake_String_UTF8.
 * @param[out] dest - Pointeur dans lequel initialiser les données, ne doit pas être `NULL`.
 * @param[in] utf - Référence utilisée par le lecteur.
*/
CAKE_C CAKE_API void cake_create_strutf8_reader_utf(Cake_String_UTF8_Reader *dest, Cake_String_UTF8 *utf);

/**
 * @brief Saute tous les prochains caractères si égaux à `value`.
 * @param reader - Lecteur dans lequel sauter les caractères.
 * @param value - Caractère à sauter.
*/
CAKE_C CAKE_API void cake_strutf8_reader_skip_char(Cake_String_UTF8_Reader *reader, char value);

/**
 * @brief Saute tous les prochains caractères si égaux à un des caractères de `values`.
 * @param reader - Lecteur dans lequel sauter les caractères.
 * @param values - Caractères à sauter.
 * @param size - Nombre de caractères dans <c>values</c>.
*/
CAKE_C CAKE_API void cake_strutf8_reader_skip_achar(Cake_String_UTF8_Reader *reader, const char *values, ulonglong size);

/**
 * @brief Retourne la prochaine ligne du lecteur.
 * @param reader - Lecteur dans lequel chercher la prochaine ligne.
 * @return \ref Cake_String_UTF8 * - La ligne trouvée.
 * @return `NULL` s'il n'y a plus de caractères à lire.
*/
CAKE_C CAKE_API Cake_String_UTF8 *cake_strutf8_reader_read_line(Cake_String_UTF8_Reader *reader);

/**
 * @brief Crée dynamiquement un \ref Cake_String_UTF8_Pair.
 * @warning Ne pas oublier de libérer la mémoire avec \ref cake_free_strutf8_pair.
 * 
 * @param[in] key - Chaîne 1 à copier.
 * @param[in] value - Chaîne 2 à copier.
 * @return \ref Cake_String_UTF8_Pair * - La paire de chaînes créée.
 */
CAKE_C CAKE_API Cake_String_UTF8_Pair *cake_strutf8_pair(const char *key, const char *value);

/**
 * @brief Libère la mémoire d'un \ref Cake_String_UTF8_Pair alloué dynamiquement.
 * 
 * @param pair - La paire de chaînes à libérer.
 */
CAKE_C CAKE_API void cake_free_strutf8_pair(Cake_String_UTF8_Pair *pair);

/**
 * @brief Crée un \ref Cake_String_UTF8 dynamiquement et pré-alloue le buffer interne de la taille spécifiée.
 * 
 * @warning Ne pas oublier de libérer la mémoire avec \ref cake_free_strutf8.
 * 
 * @param size - Taille du buffer interne.
 * 
 * @return \ref Cake_String_UTF8 * - La chaîne pré-allouée.
 * @return `NULL` en cas d'erreur.
*/
CAKE_C CAKE_API Cake_String_UTF8 *cake_strutf8_pre_alloc(ulonglong size);

/**
 * @brief Crée dynamiquement une `Cake_List_String_UTF8`, ne pas oublier de free.
 * 
 * @return La liste créée.
 */
CAKE_C CAKE_API Cake_List_String_UTF8 *cake_list_strutf8();

/**
 * @brief Copie une chaîne vers une autre. Memory leak si dest a déjà une chaine de caractères.
 * 
 * @param dest Chaîne destination.
 * @param src Chaîne source.
 */
CAKE_C CAKE_API cake_bool cake_strutf8_copy(Cake_String_UTF8 *dest, Cake_String_UTF8 *src);

/**
 * @brief Ajoute un caractère unicode dans la chaîne UTF-8.
 * 
 * @param dest La chaîne destination.
 * @param value Le caractère unicode.
 * @return Le nombre d'octets UTF-8 utilisés pour stocker le caractère.
 */
CAKE_C CAKE_API uchar cake_strutf8_add_wchar(Cake_String_UTF8 *dest, wchar_t value);

/**
 * @brief Ajoute une chaîne de caractères unicode dans la chaîne UTF-8.
 * 
 * @param dest La chaîne destination.
 * @param str La chaîne unicode à ajouter.
 */
CAKE_C CAKE_API void cake_strutf8_add_wchar_array(Cake_String_UTF8 *dest, const wchar_t *str);

/**
 * @brief Ajoute une chaîne de caractères dans la chaîne UTF-8.
 * 
 * @param dest 
 * @param str 
 */
CAKE_C CAKE_API cake_bool cake_strutf8_add_char_array(Cake_String_UTF8 *dest, const char *str);

CAKE_C CAKE_API cake_bool cake_strutf8_add_bytes(Cake_String_UTF8 *dest, const cake_byte *bytes, ulonglong size);


/**
 * @brief Insère un caractère unicode dans la chaîne UTF-8.
 * 
 * @param utf La chaîne destination.
 * @param index L'index auquel insérer le caractère.
 * @param value Le caractère unicode à insérer.
 * @return Le nombre d'octets UTF-8 utilisés pour stocker le caractère.
 */
CAKE_C CAKE_API uchar cake_strutf8_insert_wchar(Cake_String_UTF8 *utf, ulonglong index, wchar_t value);

/**
 * @brief Insère une chaîne de caractères dans la chaîne UTF-8.
 * 
 * @param utf La chaîne destination.
 * @param index L'index auquel insérer la chaîne de caractères.
 * @param str La chaîne de caractères à insérer.
 * @return `cake_true` si la chaîne à bien été insérée.
 */
CAKE_C CAKE_API cake_bool cake_strutf8_insert_char_array(Cake_String_UTF8 *utf, ulonglong index, const char *str);

/**
 * @brief Ajoute une chaîne de caractères dans une `Cake_List_String_UTF8`.
 * 
 * @param list La liste destination.
 * @param str La chaîne de caractères à ajouter.
 * 
 * @return cake_bool
 */
CAKE_C CAKE_API cake_bool cake_list_strutf8_add_char_array(Cake_List_String_UTF8 *list, const char *str);

/**
 * @brief Supprime une chaîne de caractères dans la liste à l'index spécifié.
 * 
 * @param list 
 * @param index 
 * @return cake_bool 
 */
CAKE_C CAKE_API cake_bool cake_list_strutf8_remove(Cake_List_String_UTF8 *list, ulonglong index);

/**
 * @brief Insert une chaîne de caractères dans une Cake_List_String_UTF8.
 * 
 * @param list 
 * @param index 
 * @param str La chaîne de caractères à insérer.
 * @return Retourne cake_false si l'index est supérieur au nombre d'éléments.
 */
CAKE_C CAKE_API cake_bool cake_list_strutf8_insert(Cake_List_String_UTF8 *list, ulonglong index, const char *str);


/* ===== Modification ===== */

/**
 * @brief Inverse une chaîne UTF-8, par exemple "Hello World" deviendra "dlroW olleH".
 * 
 * @param utf La chaîne à inverser.
 */
CAKE_C CAKE_API void cake_strutf8_reverse(Cake_String_UTF8 *utf);

/* ===== Suppression ===== */

/**
 * @brief Supprime le caractère UTF-8 stocké à l'index passé.
 * 
 * @param utf La chaîne destination.
 * @param index L'index auquel supprimer le caractère.
 * @return `cake_false` si l'index est supérieur au nombre de caractères.
 */
CAKE_C CAKE_API cake_bool cake_strutf8_remove_index(Cake_String_UTF8 *utf, ulonglong index);

/**
 * @brief Supprime toutes les occurences de la sous-chaîne passée.
 * 
 * @param utf La chaîne destination.
 * @param value La sous-chaîne à supprimer.
 * @return Le nombre d'occurences supprimées.
 */
CAKE_C CAKE_API ulonglong cake_strutf8_remove_all(Cake_String_UTF8 *utf, const char *value);

/**
 * @brief Supprime le début d'une chaîne UTF-8 si celle-ci commence par la sous-chaîne passée.
 * 
 * @param utf La chaîne UTF-8.
 * @param value Sous-chaîne à enlever.
 * @return `cake_true` si la sous-chaîne a été supprimée.
 */
CAKE_C CAKE_API cake_bool cake_strutf8_remove_start(Cake_String_UTF8 *utf, const char *value);

/**
 * @brief Supprime tous les caractères entre 2 positions.
 * 
 * @param utf La Chaîne UTF-8.
 * @param fromIndex Position de départ (compris).
 * @param toIndex Position de fin (non compris).
 * @return `cake_false` si un des deux index est supérieur au nombre de caractères ou que `fromIndex` >= `toIndex`.
 */
CAKE_C CAKE_API cake_bool cake_strutf8_remove_from_to(Cake_String_UTF8 *utf, ulonglong fromIndex, ulonglong toIndex);

/**
 * @brief Supprime tous les caractères entre 2 positions internes.
 * @warning Cette fonction est notamment utilisée par `libcake`.
 * 
 * @param utf La chaîne UTF-8.
 * @param fromIndex Position de départ (compris).
 * @param toIndex Position de fin (non compris).
 * @return `cake_false` si un des deux index est supérieur au nombre de caractères ou que `fromIndex` >= `toIndex`.
 */
CAKE_C CAKE_API cake_bool cake_strutf8_remove_from_to_internal(Cake_String_UTF8 *utf, ulonglong fromIndex, ulonglong toIndex);


/* ===== Conversion ===== */


/**
 * @brief Convertit une chaîne UTF-8 en chaîne UTF-16.
 * 
 * @param src Chaîne UTF-8 à convertir.
 * @param dest Chaîne destination.
 */
CAKE_C CAKE_API void cake_strutf8_to_utf16(Cake_String_UTF8 *src, Cake_String_UTF16 *dest);

/**
 * @brief Copie une chaîne de caractères dans une chaîne UTF-8,
 * aucune conversion n'est effectuée,
 * si la chaîne de caractères n'est pas de l'UTF-8, des effets indésirables peuvent survenir.
 * 
 * @param src Chaîne de caractères à copier.
 * @param dest Chaîne destination.
 */
CAKE_C CAKE_API cake_bool cake_char_array_to_strutf8(const char *src, Cake_String_UTF8 *dest);

/**
 * @brief Convertit une suite d'octets en une valeur numérique unicode.
 * 
 * @param src La suite d'octets à convertir.
 * @param bytes Le nombre d'octets à convertir.
 * @return La valeur obtenue après conversion.
 */
CAKE_C CAKE_API int cake_strutf8_decode(const uchar *src, char bytes);

/**
 * @brief Convertit un caractère UTF-16 en suite d'octets UTF-8.
 * @warning `buffer` est dynamiquement alloué avec malloc, ne pas oublier de free quand il ne sert plus.
 * 
 * @param value La valeur à convertir.
 * @param buffer Buffer qui stockera la suite d'octets.
 * @return Le nombre d'octets utilisés pour stocker la suite.
 */
CAKE_C CAKE_API uchar cake_strutf8_wchar_to_byte(wchar_t value, uchar **buffer);

/*
        Version avancée de cake_strutf8_wchar_to_byte.

        Convertit un caractère UTF-16 en suite d'octets UTF-8 et
        les stocks directement dans le buffer destination.
*/
CAKE_C CAKE_API void cake_strutf8_wchar_to_byte_ext(wchar_t value, uchar **buffer, ulonglong *index);

// Convertit une chaîne UTF-16 en UTF-8.
CAKE_C CAKE_API void cake_strutf16_to_strutf8(Cake_String_UTF16 *src, Cake_String_UTF8 *dest);

// Pas besoin d'utiliser cake_create_strutf8.
// Par contre si cette fonction est utilisée plusieurs fois sur le même Cake_String_UTF8,
// alors il faut le free entre temps.
CAKE_C CAKE_API void cake_wchar_array_to_strutf8(const wchar_t *src, Cake_String_UTF8 *dest);

CAKE_C CAKE_API void cake_wchar_array_to_strutf8_len(Cake_String_UTF8 *dest, const wchar_t *src, ulonglong len);


CAKE_C CAKE_API ulonglong cake_strutf8_to_ulonglong(Cake_String_UTF8 *utf);

CAKE_C CAKE_API void cake_ulonglong_to_char_array(ulonglong value, char *buffer);

CAKE_C CAKE_API char *cake_ulonglong_to_char_array_dyn(ulonglong value);

CAKE_C CAKE_API float cake_strutf8_to_float(Cake_String_UTF8 *utf, char decimalSeparator);

/* ===== Cleaner ===== */

// Nettoie la chaîne UTF-8 en utilisant free et en remettant les valeurs à 0 et NULL.
CAKE_C CAKE_API void cake_clear_strutf8(Cake_String_UTF8 *utf);

CAKE_C CAKE_API void cake_free_list_strutf8(Cake_List_String_UTF8 *list);


/* ===== Recherches ===== */

/*
        Fonction interne !
        Cette fonction est une des premières qui a été faite et nécessite d'être refaite pour une utilisation plus simple.

        Permet de trouver l'adresse dans une chaîne UTF-8 par rapport à un index, étant donné qu'un caractère peut avoir plusieurs octets,
        faire utf[6] peut avoir des effets indésirables, cette fonction parcourt la chaîne et compte les index.
*/
CAKE_C CAKE_API ulonglong cake_strutf8_index_by_index(const uchar *pArrayStart, uchar *pArrayEnd, ulonglong utfIndex, uchar **pStart, uchar **pEnd, cake_byte *bytes);

CAKE_C CAKE_API ulonglong cake_strutf8_index_by_index_reverse(Cake_String_UTF8 *utf, ulonglong utfIndex, cake_byte *bytes);

/**
 * @brief Recherche la sous-chaîne dans la chaîne UTF-8 à partir de `internalIndex` en allant vers le début de la chaîne.
 * 
 * @param utf Chaîne UTF-8 dans laquelle chercher.
 * @param research Sous-chaîne de caractères à rechercher.
 * @param internalIndex Pointeur vers la valeur du début de la recherche, après traitement, sera égale à sa valeur - la longueur de la recherche - 1.
 * @return L'adresse de la première occurence trouvée ou NULL en cas de non trouvaille.
 */
CAKE_C CAKE_API uchar *cake_strutf8_search_from_end(Cake_String_UTF8 *utf, const char *research, ulonglong *internalIndex);

/**
 * @brief Recherche la sous-chaîne dans la chaîne UTF-8 à partir de `internalIndex` en allant vers la fin de la chaîne.
 * 
 * @param utf Chaîne UTF-8 dans laquelle chercher.
 * @param research Sous-chaîne de caractères à rechercher.
 * @param internalIndex Pointeur vers la valeur du début de la recherche, après traitement, sera égale à sa valeur + la longueur de la recherche + 1.
 * @return L'adresse de la première occurence trouvée ou NULL en cas de non trouvaille.
 */
CAKE_C CAKE_API uchar *cake_strutf8_search_from_start(Cake_String_UTF8 *utf, const char *research, ulonglong *internalIndex);

CAKE_C CAKE_API ulonglong cake_str_search(const char *str, char value, uchar **ptr);

CAKE_C CAKE_API uchar *cake_str_search_array(const char *str, const char *value);

/* ===== Vérificateurs ===== */

// Vérifie que la chaîne UTF-8 termine avec la sous-chaîne passée.
CAKE_C CAKE_API cake_bool cake_strutf8_end_with(Cake_String_UTF8 *utf, const char *str);
// Vérifie que la chaîne UTF-8 commence avec la sous-chaîne passée.
CAKE_C CAKE_API cake_bool cake_strutf8_start_with(Cake_String_UTF8 *utf, const char *research);

// Vérifie que la chaîne de caractères commence avec la sous-chaîne passée.
CAKE_C CAKE_API cake_bool cake_str_starts_with(const char *src, const char *str);

// Vérifie que la chaîne UTF-8 soit exactement égale à la chaîne de caractères passée.
CAKE_C CAKE_API cake_bool cake_strutf8_equals(const Cake_String_UTF8 *utf, const char *compare);


/* ===== Création ===== */


/**
 * @brief Sépare les éléments d'une chaîne de caractères.
 * 
 * @param utf Chaîne de caractères.
 * @param delim Délimiteur des éléments.
 * @return La liste des éléments séparés.
 */
CAKE_C CAKE_API Cake_List_String_UTF8 *cake_strutf8_split(Cake_String_UTF8 *utf, const char *delim);

/* ===== Remplacement ===== */


/**
 * @brief Remplace toutes les occurences de la sous-chaîne dans la chaîne.
 * 
 * @param utf Chaîne de caractères.
 * @param old Sous-chaîne à remplacer.
 * @param replacement Nouvelle sous-chaîne.
 * @return Le nombre de sous-chaînes remplacées.
 */
CAKE_C CAKE_API ulonglong cake_strutf8_replace_all(Cake_String_UTF8 *utf, const char *old, const char *replacement);

/**
 * @brief Remplace le début de la chaîne par `replacement` si elle commence par `old`.
 * 
 * @param utf Chaîne de caractères.
 * @param old Sous-chaîne à remplacer.
 * @param replacement Nouvelle sous-chaîne.
 * @return cake_true si la sous-chaîne a été modifiée.
 */
CAKE_C CAKE_API cake_bool cake_strutf8_replace_start(Cake_String_UTF8 *utf, const char *old, const char *replacement);

/**
 * @brief Remplace la fin de la chaîne par `replacement` si elle fini par `old`.
 * 
 * @param utf Chaîne de caractères.
 * @param old Sous-chaîne à remplacer.
 * @param replacement Nouvelle sous-chaîne.
 * @return cake_true si la sous-chaîne a été modifiée.
 */
CAKE_C CAKE_API cake_bool cake_strutf8_replace_end(Cake_String_UTF8 *utf, const char *old, const char *replacement);

/**
 * @brief Remplace la première occurence de la chaîne de caractères passée.
 * 
 * @param utf La chaîne UTF-8.
 * @param old Sous-chaîne à remplacer.
 * @param replacement Nouvelle sous-chaîne.
 * @return `cake_true` si la sous-chaîne a été remplacée.
 */
CAKE_C CAKE_API cake_bool cake_strutf8_replace_from_start(Cake_String_UTF8 *utf, const char *old, const char *replacement);


CAKE_C CAKE_API cake_bool cake_strutf8_replace_from_end(Cake_String_UTF8 *utf, const char *old, const char *replacement);

/**
 * @brief Tous les caractères de la chaîne deviennent des minuscules.
 * 
 * @param utf Chaîne de caractères.
 */
CAKE_C CAKE_API void cake_strutf8_to_lower(Cake_String_UTF8 *utf);


/* ===== Autres ===== */

CAKE_C CAKE_API ulonglong cake_strutf8_wchar_array_calc_size(const wchar_t *str);

/**
 * @brief Calcule le nombre de caractères d'une chaîne UTF-8.
 * 
 * @param utf La chaîne UTF-8.
 * @return Le nombre de caractères de la chaîne.
 */
CAKE_C CAKE_API ulonglong cake_strutf8_length(Cake_String_UTF8 *utf);

// Equivalent de strlen.
CAKE_C CAKE_API ulonglong cake_str_count(const char *str);

CAKE_C CAKE_API uint cake_str_hex_to_uint(const uchar *str);

CAKE_C CAKE_API void cake_strutf8_decode_url(Cake_String_UTF8 *utf);

/**
 * @brief Convertie une valeur décimale en chaîne de caractères.
 * @warning `buffer` est dynamiquement alloué avec malloc, ne pas oublier de free quand il ne sert plus.
 * 
 * @param value Valeur à convertir.
 * @param buffer Buffer qui stockera la valeur convertie.
 * @return Le nombre d'octets utilisés pour stocker la valeur.
 */
CAKE_C CAKE_API uchar cake_strutf8_dec_to_char(uint value, uchar **buffer);

/**
 * @brief Stock une valeur décimale convertie en hexadécimale dans un buffer.
 * @warning Aucun caractère NULL n'est mis à la fin !
 * 
 * @param value Valeur à convertir.
 * @param dest Buffer destination.
 */
CAKE_C CAKE_API void cake_str_dec_to_hexchar(uchar value, uchar dest[2]);

/**
 * @brief Vérifie qu'une chaîne de caractères soit strictement égale à une autre.
 * 
 * @param str1 Chaîne 1.
 * @param str2 Chaîne 2.
 * @return `cake_true` si les chaînes de caractères sont identiques.
 */
CAKE_C CAKE_API cake_bool cake_str_equals(const char *str1, const char *str2);

/**
 * @brief Compte le nombre de fois qu'un caractère apparaît dans une chaîne de caractères.
 * 
 * @param str Chaîne à parcourir.
 * @param value Caractère à compter.
 * @return Le nombre de fois que le caractère à été trouvé.
 */
CAKE_C CAKE_API ulonglong cake_str_number_of(const char *str, char value);

CAKE_C CAKE_API cake_bool cake_str_contains(const char *str, char value);


CAKE_C CAKE_API void cake_strutf8_vector_delete_callback(void *args);
CAKE_C CAKE_API void cake_strutf8_vector_delete_callback_ptr(void *args);

CAKE_C CAKE_API Cake_String_UTF8 *cake_strutf8_substring(Cake_String_UTF8 *from, ulonglong startIndex, ulonglong endIndex);

CAKE_C CAKE_API cake_bool cake_strutf8_set(Cake_String_UTF8 *dest, const char *value);


#endif