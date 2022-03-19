#ifndef __CAKE_ARRAY_H__
#define __CAKE_ARRAY_H__

#include "def.h"
#include <stdlib.h>

/*
        Lorsqu'une structure utilise Cake_ArrayList, il faut que l'élément
        juste après soit un pointeur de pointeur de structure.
*/
typedef struct cake_arraylist {
    ulonglong length;
} Cake_ArrayList;


/*
        Change la taille d'une liste, pour l'augmenter ou la réduire.

        elementSize peut-être obtenu avec sizeof(<element>),
        newLength est le nouveau nombre d'éléments dans la liste.

        Note :
        Cette fonction permet de modifier la taille de n'importe quelle liste,
        mais c'est tout, aucune autre zone mémoire n'est créée.

        Exemple :
        Cake_String_UTF8 *str = cake_strutf8("Hello World bonjour");

        Cake_List_String_UTF8 *list = cake_strutf8_split(str, " ");

        cake_array_resize((Cake_ArrayList *) list, sizeof(Cake_String_UTF8 *), 4);
        list->list[3] = cake_strutf8("Test");

        printf("%llu\n", list->list[3]->data.length);
        cake_free_list_strutf8(list);

        cake_free_strutf8(str);
*/
void cake_array_resize(Cake_ArrayList *array, ulonglong elementSize, ulonglong newLength);

#endif