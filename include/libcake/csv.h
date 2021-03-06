#ifndef __CAKE_CSV_H__
#define __CAKE_CSV_H__

#include "def.h"
#include "fdio.h"
#include "array.h"
#include "strutf8.h"

/*
        La structure contient une liste de listes de Cake_String_UTF8 étant donné que chaque ligne
        contient une liste de mots séparés par un délimiteur.
*/
typedef struct cake_csv {
    Cake_ArrayList data;
    Cake_List_String_UTF8 **utfList;
} Cake_CSV;

#ifdef __cplusplus
extern "C" {
#endif

// Initialise la structure.
Cake_CSV *cake_csv();

void cake_csv_add_line(Cake_CSV *csv, const char *line, char delim);

void cake_csv_parse_file(Cake_CSV *dest, cake_fd fd, char delim);

void cake_free_csv(Cake_CSV *csv);

cake_bool cake_csv_save(Cake_CSV *csv, const char *filename, char delim);

#define cake_csv_get(csv, row, column) csv->utfList[row]->list[column]

#ifdef __cplusplus
}
#endif

#endif