#ifndef __PIKA_ENCRYPT_H__
#define __PIKA_ENCRYPT_H__

#include "def.h"
#include "utf8.h"

#ifndef PIKA_ENCRYPT
#define PIKA_ENCRYPT 0
#endif

/*
    Génère un String_UTF8 de longueur length avec des caractères aléatoires,
    utilise RAND_bytes d'OpenSSL.
*/
String_UTF8 *strutf8_random(int length);

void strutf8_sha512(String_UTF8 *utf);

#endif