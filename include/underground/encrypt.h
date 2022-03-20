#ifndef __CAKE_ENCRYPT_H__
#define __CAKE_ENCRYPT_H__

#include "def.h"
#include "strutf8.h"

#ifndef CAKE_ENCRYPT
#define CAKE_ENCRYPT 0
#endif

/*
    Génère un Cake_String_UTF8 de longueur length avec des caractères aléatoires,
    utilise RAND_bytes d'OpenSSL.
*/
Cake_String_UTF8 *cake_strutf8_random(int length);

void cake_strutf8_sha512(Cake_String_UTF8 *utf);

#endif