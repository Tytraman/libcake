#include "../html.h"

#include "../fdio.h"

Cake_HTML *cake_html_load(const char *filename) {
    cake_fd fd = cake_fdio_open_file(filename, CAKE_FDIO_ACCESS_READ, CAKE_FDIO_SHARE_READ, CAKE_FDIO_OPEN_IF_EXISTS, CAKE_FDIO_ATTRIBUTE_NORMAL);
    if(fd == CAKE_FDIO_ERROR_OPEN)
        return NULL;
    Cake_HTML *html = cake_strutf8("");
    cake_fdio_mem_copy_strutf8(html, fd, CAKE_BUFF_SIZE);
    cake_fdio_close(fd);
    return html;
}

void cake_html_optimize(Cake_HTML *html) {
    cake_strutf8_remove_all(html, "\r");
    ulonglong i, savePos;

    // On enlève d'abord les espaces
    for(i = 1; i < html->size; ++i) {
        if(html->bytes[i] == ' ' || html->bytes[i] == '\t') {
            if(html->bytes[i - 1] == '>' || html->bytes[i - 1] == '\n') {
                savePos = i;
                i++;
                while(i < html->size && (html->bytes[i] == ' ' || html->bytes[i] == '\t'))
                    i++;
                cake_strutf8_remove_from_to_internal(html, savePos, i);
                i = savePos;
            }
        }
    }
    cake_strutf8_remove_all(html, "\n");

    // On enlève les commentaires
    for(i = 3; i < html->size; ++i) {
        if(html->bytes[i - 3] == '<' && html->bytes[i - 2] == '!' && html->bytes[i - 1] == '-' && html->bytes[i] == '-') {
            savePos = i - 3;
            i += 3;
            while(1) {
                if(i >= html->size) {
                    i = html->size - 1;
                    break;
                }else if(html->bytes[i - 2] == '-' && html->bytes[i - 1] == '-' && html->bytes[i] == '>') {
                    i++;
                    break;
                }
                i++;
            }
            if(i > html->size)
                i = html->size;
            cake_strutf8_remove_from_to_internal(html, savePos, i);
            i = savePos;
        }
    }
}
