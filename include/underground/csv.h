#ifndef __PIKA_CSV_H__
#define __PIKA_CSV_H__

#include "def.h"
#include "fdio.h"
#include "array.h"
#include "utf8.h"

/*
        La structure contient une liste de listes de String_UTF8 étant donné que chaque ligne
        contient une liste de mots séparés par un délimiteur.
*/
typedef struct CSV {
    ArrayList data;
    List_String_UTF8 **utfList;
} CSV;

// Initialise la structure.
CSV *csv();

void csv_add_line(CSV *csv, const uchar *line, uchar delim);

void csv_parse_file(CSV *dest, pika_fd fd, uchar delim);

void free_csv(CSV *csv);

pika_bool csv_save(CSV *csv, const pika_char *filename, uchar delim);

#define csv_get(csv, row, column) csv->utfList[row]->list[column]

#endif