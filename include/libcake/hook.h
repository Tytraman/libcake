#ifndef __CAKE_HOOK_H__
#define __CAKE_HOOK_H__

#include "def.h"

/*
        Fonctions de test, ça ne sera sûrement jamais plus développé que ça.
*/

typedef struct Cake_hookrelay {
    uchar *ptr;
    void *targetFunc;
    uchar *originaleBytes;
    ushort originaleBytesLength;
} Cake_HookRelay;

#ifdef __cplusplus
extern "C" {
#endif

#define CAKE_GET_PAGE_OF_ADDR(pAddr, pageSize) ((unsigned long long) pAddr) & ~(pageSize - 1)

/**
 * @brief Remplace la première instruction d'une fonction par un saut vers un relay. Le relay quant à lui fait un saut vers une adresse en 64 bits.
 * 
 * @param relay 
 * @param destFunc Fonction à exécuter à la place de la fonction ciblée.
 * @param targetFunc Fonction à remplacer.
 * @param pageSize La taille d'une page mémoire.
 * @return cake_true en cas de succès.
 */
cake_bool cake_attach_hookrelay(Cake_HookRelay *relay, void *destFunc, void *targetFunc, ushort pageSize, ushort bytesRequired);

/**
 * @brief Permet de restaurer la première instruction d'une fonction pour ainsi enlever le saut vers le relay.
 * 
 * @param relay 
 * @return cake_true en cas de succès.
 */
cake_bool cake_remove_hookrelay(Cake_HookRelay *relay);

void cake_free_hookrelay(Cake_HookRelay *relay);

#ifdef CAKE_WINDOWS
#include <windows.h>

#endif

#ifdef __cplusplus
}
#endif

#endif