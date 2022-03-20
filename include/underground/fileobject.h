#ifndef __CAKE_FILE_OBJECT_H__
#define __CAKE_FILE_OBJECT_H__

#include "def.h"
#include "utf8.h"

typedef struct cake_file_object_element {
    Cake_String_UTF8 *key;
    Cake_String_UTF8 *value;
} Cake_FileObjectElement;

typedef struct cake_list_file_object_element {
    ulonglong length;
    Cake_FileObjectElement **list;
} Cake_List_FileObjectElement;

typedef struct cake_file_object_container Cake_FileObjectContainer;

typedef struct cake_list_file_object_container {
    ulonglong length;
    Cake_FileObjectContainer **list;
} Cake_List_FileObjectContainer;

typedef struct cake_file_object_container {
    Cake_String_UTF8 *key;
    Cake_List_FileObjectElement elements;
    Cake_List_FileObjectContainer containers;
} Cake_FileObjectContainer;

typedef struct cake_file_object {
    Cake_List_FileObjectElement elements;
    Cake_List_FileObjectContainer containers;
} Cake_FileObject;

typedef struct cake_file_object_snapshot {
    Cake_FileObjectContainer *actualContainer;
    Cake_List_FileObjectElement *currentElements;
    Cake_List_FileObjectContainer *currentContainers;
    ushort id;
} Cake_FileObjectSnapshot;

Cake_FileObject *cake_file_object_load(const uchar *filename);
void cake_free_file_object(Cake_FileObject *obj);

Cake_FileObjectContainer *cake_file_object_get_container(Cake_FileObject *obj, const uchar *key);
Cake_FileObjectElement *cake_file_object_get_element(Cake_FileObject *obj, const uchar *key);

void cake_list_file_object_element_add(Cake_List_FileObjectElement *elements, const uchar *key, const uchar *value);
void cake_free_list_file_object_element(Cake_List_FileObjectElement *elements);

// Ajoute un container à une liste.
Cake_FileObjectSnapshot *cake_list_file_object_container_add(Cake_List_FileObjectContainer *containers, const uchar *key);

Cake_String_UTF8 *cake_file_object_to_strutf8(Cake_FileObject *obj);

#endif