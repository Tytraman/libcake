/**
* @file strutf16.h
* @brief Fichier contenant le prototypes de tout ce qui touche aux chaînes de caractères UTF-16.
* @author Tytraman
*/

#ifndef __CAKE_UTF16_H__
#define __CAKE_UTF16_H__

/// @cond
#include "def.h"

#include <stddef.h>
/// @endcond

/** 
* @brief Chaines de caractères encodées en UTF-16.
*/
typedef struct cake_string_utf16 {
    ulonglong length;           ///< Le nombre de caractères contenus dans la chaîne.
    wchar_t *characteres;       ///< Adresse de la chaîne.
} Cake_String_UTF16;

#ifdef __cplusplus
extern "C" {
#endif


/* ===== Initialisation ===== */

void cake_create_strutf16(Cake_String_UTF16 *utf);
Cake_String_UTF16 *cake_strutf16(const wchar_t *value);

void cake_strutf16_copy(Cake_String_UTF16 *from, Cake_String_UTF16 *to);


/* ===== Setter ===== */

cake_bool cake_strutf16_set_value(Cake_String_UTF16 *utf, wchar_t *str);
ulonglong cake_strutf16_lower(Cake_String_UTF16 *utf);
ulonglong cake_strutf16_upper(Cake_String_UTF16 *utf);


/* ===== Ajout ===== */

cake_bool cake_strutf16_add_wchar_array(Cake_String_UTF16 *utf, const wchar_t *str);
cake_bool cake_strutf16_add_char(Cake_String_UTF16 *utf, wchar_t c);
cake_bool cake_strutf16_add_bytes(Cake_String_UTF16 *utf, uchar *bytes, ulonglong size);
cake_bool cake_strutf16_insert(Cake_String_UTF16 *utf, wchar_t *str);


/* ===== Suppression ===== */

cake_bool cake_strutf16_remove(Cake_String_UTF16 *utf, wchar_t *str);
cake_bool cake_strutf16_remove_from_index(Cake_String_UTF16 *utf, ulonglong index);
cake_bool cake_strutf16_remove_before_index(Cake_String_UTF16 *utf, ulonglong index);
cake_bool cake_strutf16_remove_part_from_end(Cake_String_UTF16 *utf, wchar_t delim);
ulonglong cake_strutf16_rtrim(Cake_String_UTF16 *utf, wchar_t charactere);
cake_bool cake_strutf16_remove_index(Cake_String_UTF16 *utf, ulonglong index);


/* ===== Remplacement ===== */

ulonglong cake_strutf16_replace_all_char(Cake_String_UTF16 *utf, wchar_t old, wchar_t replacement);
cake_bool cake_strutf16_replace(Cake_String_UTF16 *utf, wchar_t *old, wchar_t *replacement);
cake_bool cake_strutf16_replace_from_end(Cake_String_UTF16 *utf, wchar_t *old, wchar_t *replacement);


/* ===== Cleaner ===== */

void cake_clear_strutf16(Cake_String_UTF16 *utf);
void cake_strutf16_empty(Cake_String_UTF16 *utf);
void cake_free_strutf16(Cake_String_UTF16 *utf);


/* ===== Recherches ===== */

wchar_t *cake_strutf16_search(Cake_String_UTF16 *utf, const wchar_t *research);
wchar_t *cake_strutf16_search_from_end(Cake_String_UTF16 *utf, wchar_t *research);
wchar_t *cake_strutf16_find(Cake_String_UTF16 *utf, wchar_t research, ulonglong *index);
wchar_t *cake_strutf16_search_from(Cake_String_UTF16 *utf, wchar_t *research, ulonglong *index);
ulonglong cake_strutf16_number_of(Cake_String_UTF16 *utf, wchar_t charactere);
cake_bool cake_strutf16_start_with(Cake_String_UTF16 *utf, wchar_t *str);

ulonglong cake_wstr_count(const wchar_t *str);

void cake_char_array_to_strutf16(const char *source, Cake_String_UTF16 *dest);

#ifdef __cplusplus
}
#endif

#endif