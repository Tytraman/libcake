#include "../array.h"

#include <stdio.h>

void cake_array_resize(Cake_ArrayList *array, ulonglong elementSize, ulonglong newLength) {
    // Récupération du pointeur vers la liste.
    void **src = (void **) array;
    void ***list = (void ***) &src;

    // Pointeur vers la liste :
    void ***ptr = (void ***) ((*list) + 1);

    // On augmente la taille de la liste
    array->length = newLength;
    *ptr = (void **) realloc(*ptr, newLength * elementSize);

    // La liste a juste été augmentée mais il n'y a pas
    // de zone réservée pour stocker les données des nouvelles éléments.
}
