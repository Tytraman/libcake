#include "../html.h"

#include "../fdio.h"

HTML *html_load(const uchar *filename) {
    pika_fd fd = fdio_open_file(filename, FDIO_ACCESS_READ, FDIO_SHARE_READ, FDIO_OPEN_IF_EXISTS, FDIO_ATTRIBUTE_NORMAL);
    if(fd == FDIO_ERROR_OPEN)
        return NULL;
    HTML *html = strutf8("");
    fdio_mem_copy_strutf8(html, fd, PIKA_BUFF_SIZE);
    fdio_close(fd);
    return html;
}

void html_optimize(HTML *html) {
    strutf8_remove_all(html, "\r");
    ulonglong i, savePos;

    // On enlève d'abord les espaces
    for(i = 1; i < html->data.length; ++i) {
        if(html->bytes[i] == ' ' || html->bytes[i] == '\t') {
            if(html->bytes[i - 1] == '>' || html->bytes[i - 1] == '\n') {
                savePos = i;
                i++;
                while(i < html->data.length && (html->bytes[i] == ' ' || html->bytes[i] == '\t'))
                    i++;
                strutf8_remove_from_to_internal(html, savePos, i);
                i = savePos;
            }
        }
    }
    strutf8_remove_all(html, "\n");

    // On enlève les commentaires
    for(i = 3; i < html->data.length; ++i) {
        if(html->bytes[i - 3] == '<' && html->bytes[i - 2] == '!' && html->bytes[i - 1] == '-' && html->bytes[i] == '-') {
            savePos = i - 3;
            i += 3;
            while(1) {
                if(i >= html->data.length) {
                    i = html->data.length - 1;
                    break;
                }else if(html->bytes[i - 2] == '-' && html->bytes[i - 1] == '-' && html->bytes[i] == '>') {
                    i++;
                    break;
                }
                i++;
            }
            if(i > html->data.length)
                i = html->data.length;
            strutf8_remove_from_to_internal(html, savePos, i);
            i = savePos;
        }
    }
}
