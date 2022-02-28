#ifndef __PIKA_HTML_H__
#define __PIKA_HTML_H__

#include "utf8.h"

typedef String_UTF8 HTML;

// Même fonction que css_load
HTML *html_load(const uchar *filename);
#define free_html(html) free_strutf8(html)

void html_optimize(HTML *html);

#endif