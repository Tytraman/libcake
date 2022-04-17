#ifndef __CAKE_CSS_H__
#define __CAKE_CSS_H__

#include "strutf8.h"

typedef Cake_String_UTF8 Cake_CSS;

#ifdef __cplusplus
extern "C" {
#endif

Cake_CSS *cake_css_load(const uchar *filename);
#define cake_free_css(css) cake_free_strutf8(css)

void cake_css_optimize(Cake_CSS *css);

#ifdef __cplusplus
}
#endif

#endif