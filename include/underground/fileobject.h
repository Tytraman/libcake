#ifndef __PIKA_FILE_OBJECT_H__
#define __PIKA_FILE_OBJECT_H__

#include "def.h"
#include "utf8.h"

typedef struct file_object_element {
    String_UTF8 *key;
    String_UTF8 *value;
} FileObjectElement;

typedef struct list_file_object_element {
    ulonglong length;
    FileObjectElement **list;
} ListFileObjectElement;

typedef struct file_object_container FileObjectContainer;

typedef struct list_file_object_container {
    ulonglong length;
    FileObjectContainer **list;
} ListFileObjectContainer;

typedef struct file_object_container {
    String_UTF8 *key;
    ListFileObjectElement elements;
    ListFileObjectContainer containers;
} FileObjectContainer;

typedef struct file_object {
    ListFileObjectElement elements;
    ListFileObjectContainer containers;
} FileObject;

typedef struct file_object_snapshot {
    FileObjectContainer *actualContainer;
    ListFileObjectElement *currentElements;
    ListFileObjectContainer *currentContainers;
    ushort id;
} FileObjectSnapshot;

FileObject *file_object_load(const uchar *filename);
void free_file_object(FileObject *obj);

FileObjectContainer *file_object_get_container(FileObject *obj, const uchar *key);
FileObjectElement *file_object_get_element(FileObject *obj, const uchar *key);

void list_file_object_element_add(ListFileObjectElement *elements, const uchar *key, const uchar *value);
void free_list_file_object_element(ListFileObjectElement *elements);

// Ajoute un container à une liste.
FileObjectSnapshot *list_file_object_container_add(ListFileObjectContainer *containers, const uchar *key);

String_UTF8 *file_object_to_strutf8(FileObject *obj);

#endif