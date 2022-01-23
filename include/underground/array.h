#ifndef __PIKA_ARRAY_H__
#define __PIKA_ARRAY_H__

#include "def.h"
#include <stdlib.h>

/*
        Lorsqu'une structure utilise ArrayList, il faut que l'élément
        juste après soit un pointeur de pointeur de structure.
*/
typedef struct ArrayList {
    ulonglong length;
} ArrayList;


/*
        Change la taille d'une liste, pour l'augmenter ou la réduire.

        elementSize peut-être obtenu avec sizeof(<element>),
        newLength est le nouveau nombre d'éléments dans la liste.

        Note :
        Cette fonction permet de modifier la taille de n'importe quelle liste,
        mais c'est tout, aucune autre zone mémoire n'est créée.

        Exemple :
        String_UTF8 *str = strutf8("Hello World bonjour");

        List_String_UTF8 *list = strutf8_split(str, " ");

        array_resize((ArrayList *) list, sizeof(String_UTF8 *), 4);
        list->list[3] = strutf8("Test");

        printf("%llu\n", list->list[3]->data.length);
        free_list_strutf8(list);

        free_strutf8(str);
*/
void array_resize(ArrayList *array, ulonglong elementSize, ulonglong newLength);

#endif