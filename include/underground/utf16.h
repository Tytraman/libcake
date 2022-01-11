#ifndef __PIKA_UTF16_H__
#define __PIKA_UTF16_H__

#include "def.h"

#include <stddef.h>


typedef struct String_UTF16 {
    unsigned long long length;
    wchar_t *characteres;
} String_UTF16;

typedef struct String_UTF16_Reader {
    String_UTF16 *utf;
    String_UTF16 *lastPtr;
    unsigned long long index;
} String_UTF16_Reader;


/* ===== Initialisation ===== */

void create_strutf16(String_UTF16 *utf);
void strutf16_copy(String_UTF16 *from, String_UTF16 *to);
char strutf16_copy_between(String_UTF16 *from, String_UTF16 *to, unsigned long long begin, unsigned long long end);

void init_strutf16_reader(String_UTF16_Reader *reader, String_UTF16 *utf);
String_UTF16 *strutf16_getline(String_UTF16_Reader *reader);


/* ===== Setter ===== */

void strutf16_set_value(String_UTF16 *utf, wchar_t *str);
unsigned long long strutf16_lower(String_UTF16 *utf);
unsigned long long strutf16_upper(String_UTF16 *utf);



/* ===== Ajout ===== */

void strutf16_add_wchar_array(String_UTF16 *utf, wchar_t *str);
void strutf16_add_char(String_UTF16 *utf, wchar_t c);
void strutf16_add_bytes(String_UTF16 *utf, unsigned char *bytes, unsigned long long size);
void strutf16_insert(String_UTF16 *utf, wchar_t *str);



/* ===== Suppression ===== */

pika_bool strutf16_remove(String_UTF16 *utf, wchar_t *str);
pika_bool strutf16_remove_from_index(String_UTF16 *utf, unsigned long long index);
pika_bool strutf16_remove_before_index(String_UTF16 *utf, unsigned long long index);
pika_bool strutf16_remove_part_from_end(String_UTF16 *utf, wchar_t delim);
unsigned long long strutf16_rtrim(String_UTF16 *utf, wchar_t charactere);
pika_bool strutf16_remove_index(String_UTF16 *utf, unsigned long long index);



/* ===== Remplacement ===== */

unsigned long long strutf16_replace_all_char(String_UTF16 *utf, wchar_t old, wchar_t replacement);
pika_bool strutf16_replace(String_UTF16 *utf, wchar_t *old, wchar_t *replacement);
pika_bool strutf16_replace_from_end(String_UTF16 *utf, wchar_t *old, wchar_t *replacement);



/* ===== Cleaner ===== */

void clear_strutf16(String_UTF16 *utf);
void strutf16_empty(String_UTF16 *utf);
void free_strutf16(String_UTF16 *utf);



/* ===== Recherches ===== */

wchar_t *strutf16_search(String_UTF16 *utf, const wchar_t *research);
wchar_t *strutf16_search_from_end(String_UTF16 *utf, wchar_t *research);
wchar_t *strutf16_find(String_UTF16 *utf, wchar_t research, unsigned long long *index);
wchar_t *strutf16_search_from(String_UTF16 *utf, wchar_t *research, unsigned long long *index);
pika_bool strutf16_key_value(const wchar_t *key, String_UTF16 *src, String_UTF16 *dest);
unsigned long long strutf16_number_of(String_UTF16 *utf, wchar_t charactere);
char strutf16_start_with(String_UTF16 *utf, wchar_t *str);

#endif