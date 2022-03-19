#if PIKA_ENCRYPT > 0

#include "../encrypt.h"


#include <openssl/sha.h>
#include <openssl/rand.h>

Cake_String_UTF8 *cake_strutf8_random(int length) {
    uchar *buff = (uchar *) malloc(length * sizeof(uchar) + sizeof(uchar));
    buff[length] = '\0';
    RAND_bytes(buff, length);
    uchar val;
    int i;
    for(i = 0; i < length; ++i) {
        val = buff[i] % 40;
        if(val >= 0 && val <= 9)
            val += '0';
        else if(val > 9 && val < 36)
            val += 55;
        else {
            switch(val) {
                default: break;
                case 36:
                    val = '-';
                    break;
                case 37:
                    val = '#';
                    break;
                case 38:
                    val = '@';
                    break;
                case 39:
                    val = '&';
                    break;
            }
        }
        buff[i] = val;    
    }
    Cake_String_UTF8 *str = cake_strutf8(buff);
    free(buff);
    return str;
}

void cake_strutf8_sha512(Cake_String_UTF8 *utf) {
    uchar hash[SHA512_DIGEST_LENGTH];
    uchar hexHash[SHA512_DIGEST_LENGTH * 2 + 1];
    hexHash[sizeof(hexHash) - 1] = '\0';
    SHA512(utf->bytes, utf->data.length * sizeof(uchar), hash);
    uchar i;
    uchar *current = hexHash;
    for(i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
        cake_str_dec_to_hexchar(hash[i], current);
        current += 2;
    }
    cake_char_array_to_strutf8(hexHash, utf);
}
#endif