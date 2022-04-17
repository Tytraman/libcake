#ifndef __CAKE_FILE_OBJECT_H__
#define __CAKE_FILE_OBJECT_H__

#include "def.h"
#include "strutf8.h"

typedef struct cake_fileobject_element {
    Cake_String_UTF8 *key;
    Cake_String_UTF8 *value;
} Cake_FileObjectElement;

typedef struct cake_list_fileobject_element {
    ulonglong length;
    Cake_FileObjectElement **list;
} Cake_List_FileObjectElement;

typedef struct cake_fileobject_container Cake_FileObjectContainer;

typedef struct cake_list_fileobject_container {
    ulonglong length;
    Cake_FileObjectContainer **list;
} Cake_List_FileObjectContainer;

typedef struct cake_fileobject_container {
    Cake_List_FileObjectElement elements;
    Cake_List_String_UTF8 *strList;
    Cake_List_FileObjectContainer containers;
    Cake_String_UTF8 *key;
} Cake_FileObjectContainer;

typedef struct cake_fileobject {
    Cake_List_FileObjectElement elements;
    Cake_List_String_UTF8 *strList;
    Cake_List_FileObjectContainer containers;
} Cake_FileObject;

typedef struct cake_fileobject_snapshot {
    Cake_FileObjectContainer *actualContainer;
    Cake_List_FileObjectElement *currentElements;
    Cake_List_FileObjectContainer *currentContainers;
    Cake_List_String_UTF8 *currentStrList;
    ushort id;
} Cake_FileObjectSnapshot;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Charge un fichier objet si existant.
 * 
 * @param filename Chemin du fichier à charger.
 * @return NULL si le fichier n'existe pas ou s'il n'a pas pu être chargé.
 */
Cake_FileObject *cake_fileobject_load(const char *filename);

Cake_FileObject *cake_fileobject();

/**
 * @brief Libère la mémoire d'un Cake_FileObject.
 * 
 * @param obj Cake_FileObject à libérer
 */
void cake_free_fileobject(Cake_FileObject *obj);

/**
 * @brief Retourne le pointeur vers le container recherché si existant.
 * 
 * @param obj Objet dans lequel chercher.
 * @param key Nom du container à rechercher, pour chercher un container dans un autre container, le délimiteur . doit être utilisé, ex : "test.oui"
 * @return NULL si le container n'a pas été trouvé.
 */
Cake_FileObjectContainer *cake_fileobject_get_container(Cake_FileObject *obj, const char *key);

/**
 * @brief Retourne le pointeur vers l'élément recherché si existant.
 * 
 * @param obj Objet dans lequel chercher.
 * @param key Nom de l'élément à rechercher, pour chercher un élément dans un container, le délimiteur . doit être utilisé, ex : "test.oui"
 * @return NULL si l'élément n'a pas été trouvé.
 */
Cake_FileObjectElement *cake_fileobject_get_element(Cake_FileObject *obj, const char *key);

Cake_FileObjectElement *cake_fileobject_get_element_from(Cake_FileObjectContainer *container, const char *key);

/**
 * @brief Ajoute un élément dans une liste d'éléments.
 * 
 * @param elements Liste d'éléments dans laquelle ajouter?
 * @param key Clé de l'élément à ajouter.
 * @param value Valeur de l'élément à ajouter.
 */
Cake_FileObjectElement *cake_list_fileobject_element_add(Cake_List_FileObjectElement *elements, const char *key, const char *value);

/**
 * @brief Retire un élément dans une liste d'éléments.
 * 
 * @param elements 
 * @param key 
 * @return cake_bool 
 */
cake_bool cake_list_fileobject_element_remove(Cake_List_FileObjectElement *elements, const char *key);

/**
 * @brief Libère la mémoire d'une liste d'éléments. Surtout utilisée en interne par la libcake.
 * 
 * @param elements Liste à libérer.
 */
void cake_free_list_fileobject_element(Cake_List_FileObjectElement *elements);

/**
 * @brief Ajoute un container dans une liste de container.
 * @warning Utilisation déconseillée.
 * 
 * @param containers Liste de containers dans laquelle ajouter.
 * @param key Nom du container à ajouter.
 * @return La nouvelle snapshot. Utilisée en interne par la libcake.
 */
Cake_FileObjectSnapshot *cake_list_fileobject_container_add(Cake_List_FileObjectContainer *containers, const char *key);

/**
 * @brief Convertie un Cake_FileObject en Cake_String_UTF8.
 * 
 * @param obj Objet à convertir.
 * @return Le Cake_String_UTF8 créé.
 */
Cake_String_UTF8 *cake_fileobject_to_strutf8(Cake_FileObject *obj);

/**
 * @brief Permet d'énumérer tous les composants d'un Cake_FileObject et d'exécuter une fonction de callback pour chacun d'entre eux.
 * @warning Utilisation déconseillée, surtout utilisée par la libcake.
 * 
 * @param obj 
 * @param list_elements_found_callback 
 * @param list_strutf8_found_callback 
 * @param container_found_callback 
 * @param container_end_callback 
 * @param listElementsArgs 
 * @param listStrUtf8Args 
 * @param containerArgs 
 * @param containerEndArgs 
 */
void cake_fileobject_enum(
    Cake_FileObject *obj,
    void (*list_elements_found_callback)(Cake_List_FileObjectElement *elements, void *args),
    void (*list_strutf8_found_callback)(Cake_List_String_UTF8 *list, void *args),
    void (*container_found_callback)(Cake_FileObjectContainer *container, void *args),
    void (*container_end_callback)(Cake_FileObjectContainer *container, void *args),
    void *listElementsArgs,
    void *listStrUtf8Args,
    void *containerArgs,
    void *containerEndArgs
);

/**
 * @brief Ajoute un élément dans un Cake_FileObject.
 * 
 * @param obj 
 * @param key 
 * @param value 
 * @return Cake_FileObjectElement* 
 */
Cake_FileObjectElement *cake_fileobject_add_element(Cake_FileObject *obj, const char *key, const char *value);

/**
 * @brief Supprime un élément dans un Cake_FileObject.
 * 
 * @param obj 
 * @param key 
 * @return cake_bool 
 */
cake_bool cake_fileobject_remove_element(Cake_FileObject *obj, const char *key);

Cake_FileObjectContainer *cake_fileobject_get_container_from(Cake_FileObjectContainer *container, const char *key);

#ifdef __cplusplus
}
#endif

#endif