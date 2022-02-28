#include "../css.h"
#include "../def.h"
#include "../utf16.h"
#include "../fdio.h"

#include <stdio.h>
#include <stdlib.h>

CSS *css_load(const uchar *filename) {
    pika_fd fd = fdio_open_file(filename, FDIO_ACCESS_READ, FDIO_SHARE_READ, FDIO_OPEN_IF_EXISTS, FDIO_ATTRIBUTE_NORMAL);
    if(fd == FDIO_ERROR_OPEN)
        return NULL;
    CSS *css = strutf8("");
    fdio_mem_copy_strutf8(css, fd, PIKA_BUFF_SIZE);
    fdio_close(fd);
    return css;
}

void css_optimize(CSS *css) {
    strutf8_remove_all(css, "\r");
    strutf8_remove_all(css, "\n");
    ulonglong i;
    ulonglong savePos;

    // On enlève d'abord les espaces
    for(i = 1; i < css->data.length; ++i) {
        if(css->bytes[i] == ' ' || css->bytes[i] == '\t') {
            if(
                !(
                    (css->bytes[i - 1] >= 'a' && css->bytes[i - 1] <= 'z') ||
                    (css->bytes[i - 1] >= 'A' && css->bytes[i - 1] <= 'Z') ||
                    (css->bytes[i - 1] >= '0' && css->bytes[i - 1] <= '9')
                )
            ) {
                savePos = i;
                i++;
                while(i < css->data.length && (css->bytes[i] == ' ' || css->bytes[i] == '\t'))
                    i++;
                strutf8_remove_from_to_internal(css, savePos, i);
                i = savePos;
            }
        }
    }

    // On enlève les commentaires
    for(i = 1; i < css->data.length; ++i) {
        if(css->bytes[i - 1] == '/' && css->bytes[i] == '*') {
            savePos = i - 1;
            i += 2;
            while(1) {
                if(i >= css->data.length) {
                    i = css->data.length - 1;
                    break;
                }else if(css->bytes[i - 1] == '*' && css->bytes[i] == '/') {
                    i++;
                    break;
                }
                i++;
            }
            if(i > css->data.length)
                i = css->data.length;
            strutf8_remove_from_to_internal(css, savePos, i);
            i = savePos;
        }
    }
}
