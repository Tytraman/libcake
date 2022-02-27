#include "../csv.h"

#include <stdlib.h>

CSV *csv() {
    CSV *v = (CSV *) malloc(sizeof(CSV));
    v->data.length = 0;
    v->utfList = NULL;
    return v;
}

void csv_add_line(CSV *csv, const uchar *line, uchar delim) {
    String_UTF8 *copyLine = strutf8(line);

    ulonglong current = csv->data.length;
    array_resize((ArrayList *) csv, sizeof(List_String_UTF8 *), current + 1);
    csv->utfList[current] = list_strutf8();

    uchar *ptr = copyLine->bytes;
    uchar *lastPtr = ptr;
    ulonglong length;

    ulonglong kurrent;

    while(1) {
        str_search(lastPtr, delim, &ptr);
        if(ptr != NULL) {
            *ptr = '\0';
            kurrent = csv->utfList[current]->data.length;
            array_resize((ArrayList *) csv->utfList[current], sizeof(String_UTF8 *), kurrent + 1);
            csv_get(csv, current, kurrent) = strutf8(lastPtr);
            ptr++;
            lastPtr = ptr;
            ptr = NULL;
        }else
            break;
    }

    kurrent = csv->utfList[current]->data.length;
    array_resize((ArrayList *) csv->utfList[current], sizeof(String_UTF8 *), kurrent + 1);
    csv_get(csv, current, kurrent) = strutf8(lastPtr);
    free_strutf8(copyLine);
}

void csv_parse_file(CSV *dest, pika_fd fd, uchar delim) {
    String_UTF8 *utf = strutf8("");
    fdio_mem_copy_strutf8(utf, fd, 2048);

    uchar *lastPtr = utf->bytes;

    uchar ignored1 = '\r', ignored2 = '\n', ignored3 = '\0';

    ulonglong i;
    for(i = 0; i <= utf->data.length; ++i) {
        if(utf->bytes[i] == ignored1 || utf->bytes[i] == ignored2 || utf->bytes[i] == ignored3) {
            utf->bytes[i] = '\0';
            csv_add_line(dest, lastPtr, delim);
            i++;
            while(i < utf->data.length && (utf->bytes[i] == ignored1 || utf->bytes[i] == ignored2 || utf->bytes[i] == ignored3)) i++;
            lastPtr = &utf->bytes[i];
        }
    }

    free_strutf8(utf);
}

void free_csv(CSV *csv) {
    ulonglong i;
    for(i = 0; i < csv->data.length; ++i)
        free_list_strutf8(csv->utfList[i]);
    free(csv);
}

pika_bool csv_save(CSV *csv, const uchar *filename, uchar delim) {
    pika_fd fd = fdio_open_file(filename, FDIO_ACCESS_WRITE, 0, FDIO_OPEN_CREATE_ALWAYS, FDIO_ATTRIBUTE_NORMAL);
    if(fd == FDIO_ERROR_OPEN)
        return pika_false;
    ulonglong i, j;
    pika_size bytesWritten;
    ulonglong total;
    uchar lf = '\n';

    for(i = 0; i < csv->data.length; ++i) {
        for(j = 0; j < csv->utfList[i]->data.length; ++j) {
            total = 0;
            while(total < csv_get(csv, i, j)->data.length) {
                if(fdio_write(fd, (csv_get(csv, i, j)->data.length > PIKA_BUFF_SIZE ? PIKA_BUFF_SIZE : csv_get(csv, i, j)->data.length), bytesWritten, &csv_get(csv, i, j)->bytes[total]) == FDIO_ERROR_WRITE) {
                    fdio_close(fd);
                    return pika_false;
                }
                total += bytesWritten;
            }
            if(j != csv->utfList[i]->data.length - 1)
                fdio_write(fd, 1, bytesWritten, &delim);
        }
        fdio_write(fd, 1, bytesWritten, &lf);
    }
    fdio_close(fd);
}
