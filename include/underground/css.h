#ifndef __PIKA_CSS_H__
#define __PIKA_CSS_H__

#include "utf8.h"

typedef String_UTF8 CSS;

CSS *css_load(const uchar *filename);
#define free_css(css) free_strutf8(css)

void css_optimize(CSS *css);

#endif