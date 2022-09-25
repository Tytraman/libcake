#include "../csv.h"

#include <stdlib.h>

Cake_CSV *csv() {
    Cake_CSV *v = (Cake_CSV *) malloc(sizeof(Cake_CSV));
    v->data.length = 0;
    v->utfList = NULL;
    return v;
}

void cake_csv_add_line(Cake_CSV *csv, const char *line, char delim) {
    Cake_String_UTF8 *copyLine = cake_strutf8(line);

    ulonglong current = csv->data.length;
    cake_array_resize((Cake_ArrayList *) csv, sizeof(Cake_List_String_UTF8 *), current + 1);
    csv->utfList[current] = cake_list_strutf8();

    uchar *ptr = copyLine->bytes;
    uchar *lastPtr = ptr;

    ulonglong kurrent;

    while(1) {
        cake_str_search((cchar_ptr) lastPtr, delim, &ptr);
        if(ptr != NULL) {
            *ptr = '\0';
            kurrent = csv->utfList[current]->data.length;
            cake_array_resize((Cake_ArrayList *) csv->utfList[current], sizeof(Cake_String_UTF8 *), kurrent + 1);
            cake_csv_get(csv, current, kurrent) = cake_strutf8((char *) lastPtr);
            ptr++;
            lastPtr = ptr;
            ptr = NULL;
        }else
            break;
    }

    kurrent = csv->utfList[current]->data.length;
    cake_array_resize((Cake_ArrayList *) csv->utfList[current], sizeof(Cake_String_UTF8 *), kurrent + 1);
    cake_csv_get(csv, current, kurrent) = cake_strutf8((char *) lastPtr);
    cake_free_strutf8(copyLine);
}

void cake_csv_parse_file(Cake_CSV *dest, cake_fd fd, char delim) {
    Cake_String_UTF8 *utf = cake_strutf8("");
    cake_fdio_mem_copy_strutf8(utf, fd, 2048);

    uchar *lastPtr = utf->bytes;

    uchar ignored1 = '\r', ignored2 = '\n', ignored3 = '\0';

    ulonglong i;
    for(i = 0; i <= utf->size; ++i) {
        if(utf->bytes[i] == ignored1 || utf->bytes[i] == ignored2 || utf->bytes[i] == ignored3) {
            utf->bytes[i] = '\0';
            cake_csv_add_line(dest, (char *) lastPtr, delim);
            i++;
            while(i < utf->size && (utf->bytes[i] == ignored1 || utf->bytes[i] == ignored2 || utf->bytes[i] == ignored3)) i++;
            lastPtr = &utf->bytes[i];
        }
    }

    cake_free_strutf8(utf);
}

void cake_free_csv(Cake_CSV *csv) {
    ulonglong i;
    for(i = 0; i < csv->data.length; ++i)
        cake_free_list_strutf8(csv->utfList[i]);
    free(csv);
}

cake_bool cake_csv_save(Cake_CSV *csv, const char *filename, char delim) {
    cake_fd fd = cake_fdio_open_file(filename, CAKE_FDIO_ACCESS_WRITE, 0, CAKE_FDIO_OPEN_CREATE_ALWAYS, CAKE_FDIO_ATTRIBUTE_NORMAL);
    if(fd == CAKE_FDIO_ERROR_OPEN)
        return cake_false;
    ulonglong i, j;
    cake_size bytesWritten;
    ulonglong total;
    uchar lf = '\n';

    for(i = 0; i < csv->data.length; ++i) {
        for(j = 0; j < csv->utfList[i]->data.length; ++j) {
            total = 0;
            while(total < cake_csv_get(csv, i, j)->size) {
                if(cake_fdio_write(fd, (cake_csv_get(csv, i, j)->size > CAKE_BUFF_SIZE ? CAKE_BUFF_SIZE : cake_csv_get(csv, i, j)->size), bytesWritten, &cake_csv_get(csv, i, j)->bytes[total]) == CAKE_FDIO_ERROR_WRITE) {
                    cake_fdio_close(fd);
                    return cake_false;
                }
                total += bytesWritten;
            }
            if(j != csv->utfList[i]->data.length - 1)
                cake_fdio_write(fd, 1, bytesWritten, &delim);
        }
        cake_fdio_write(fd, 1, bytesWritten, &lf);
    }
    cake_fdio_close(fd);
    return cake_true;
}
