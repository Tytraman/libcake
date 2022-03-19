#ifndef __CAKE_HTML_H__
#define __CAKE_HTML_H__

#include "utf8.h"

typedef Cake_String_UTF8 Cake_HTML;

// Même fonction que cake_css_load
Cake_HTML *cake_html_load(const uchar *filename);
#define cake_free_html(html) cake_free_strutf8(html)

void cake_html_optimize(Cake_HTML *html);

#endif