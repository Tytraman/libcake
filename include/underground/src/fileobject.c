#include "../fileobject.h"
#include "../fdio.h"

#include <stdio.h>

typedef struct file_object_print {
    Cake_List_FileObjectContainer *currentContainers;
    ulonglong iContainers;
} FileObjectPrint;

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
) {
    if(list_elements_found_callback != NULL)
        list_elements_found_callback(&obj->elements, listElementsArgs);
    if(list_strutf8_found_callback != NULL)
        list_strutf8_found_callback(obj->strList, listStrUtf8Args);

    // Si l'objet contient un container
    if(obj->containers.list != NULL) {
        FileObjectPrint *snapshots = (FileObjectPrint *) malloc(sizeof(FileObjectPrint));
        ulonglong length = 1;
        snapshots[0].currentContainers = &obj->containers;
        snapshots[0].iContainers = 0;

        while(1) {
            // On exécute le callback du container
            if(container_found_callback != NULL)
                container_found_callback(snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers], containerArgs);
            // Si le container possède d'autres containers
            if(snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->containers.list != NULL) {
                // On ajoute une snapshot
                snapshots = (FileObjectPrint *) realloc(snapshots, (length + 1) * sizeof(FileObjectPrint));
                snapshots[length].currentContainers = &snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->containers;
                snapshots[length].iContainers = 0;
                // On augmente iContainer car on a fini de traiter avec l'ancien container
                (snapshots[length - 1].iContainers)++;
                // Le nombre de snapshots augmente donc de 1
                length++;
            }else {
                if(container_end_callback != NULL)
                    container_end_callback(snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers], containerEndArgs);
                (snapshots[length - 1].iContainers)++;
                
                // On vérifie si les containers parents sont terminés
                while(snapshots[length - 1].iContainers == snapshots[length - 1].currentContainers->length) {
                    length--;
                    if(length == 0)
                        goto file_object_print_exit;
                    if(container_end_callback != NULL)
                        container_end_callback(snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers - 1], containerEndArgs);
                }
            }
            if(length == 0)
                break;
            
        }
    file_object_print_exit:
        free(snapshots);
    }
}

Cake_FileObject *cake_file_object_load(const uchar *filename) {
    cake_fd fd = cake_fdio_open_file(filename, CAKE_FDIO_ACCESS_READ, CAKE_FDIO_SHARE_READ, CAKE_FDIO_OPEN_IF_EXISTS, CAKE_FDIO_ATTRIBUTE_NORMAL);
    if(fd == CAKE_FDIO_ERROR_OPEN)
        return NULL;
    Cake_String_UTF8 *copy = cake_strutf8("");
    cake_fdio_mem_copy_strutf8(copy, fd, CAKE_BUFF_SIZE);
    cake_fdio_close(fd);

    Cake_FileObject *obj = (Cake_FileObject *) malloc(sizeof(Cake_FileObject));
    obj->elements.length = 0;
    obj->elements.list   = NULL;
    obj->containers.length = 0;
    obj->containers.list   = NULL;
    obj->strList = cake_list_strutf8();

    ushort id = 0;

    Cake_FileObjectSnapshot *snapshots = (Cake_FileObjectSnapshot *) malloc(sizeof(Cake_FileObjectSnapshot));
    snapshots[0].actualContainer   = NULL;
    snapshots[0].currentElements   = &obj->elements;
    snapshots[0].currentContainers = &obj->containers;
    snapshots[0].currentStrList    = obj->strList;
    snapshots[0].id = id;
    ulonglong length = 1;

    id++;

    ulonglong i = 0;
    uchar *keyPtr;
    uchar *valuePtr;

    // On ignore les espaces du début du fichier
    while(i != copy->data.length && (copy->bytes[i] == ' ' || copy->bytes[i] == '\t'))
        i++;

    keyPtr = &copy->bytes[i];
    valuePtr = keyPtr;

    while(1) {
        if(i == copy->data.length)
            break;
        // Si on trouve le délimiteur
        if(copy->bytes[i] == ':') {
            // On ignore les espaces situés après la clé
            uchar *test = &copy->bytes[i - 1];
            while(test > copy->bytes && *(test - 1) != '\n' && (*test == ' ' || *test == '\t'))
                test--;
            *(test + 1) = '\0';
            i++;

            // On ignore les espaces situés après le délimiteur
            while(i != copy->data.length && (copy->bytes[i] == ' ' || copy->bytes[i] == '\t'))
                i++;
            valuePtr = &copy->bytes[i];

            while(1) {
                if(i >= copy->data.length)
                    goto file_object_jump;
                if(copy->bytes[i] == '\r' || copy->bytes[i] == '\n') {
                    copy->bytes[i] = '\0';
                    i++;
                file_object_jump:
                    cake_list_file_object_element_add(snapshots[length - 1].currentElements, keyPtr, valuePtr);
                    break;
                }
                i++;
            }

            // On ignore tous les espaces et les sauts de lignes suivants
            while(i != copy->data.length && (copy->bytes[i] == ' ' || copy->bytes[i] == '\t' || copy->bytes[i] == '\r' || copy->bytes[i] == '\n'))
                i++;
            keyPtr = &copy->bytes[i];

        // Si on trouve le début d'un container
        }else if(copy->bytes[i] == '{') {
            // On ignore les espaces situés après la clé
            uchar *test = &copy->bytes[i - 1];
            while(test > copy->bytes && *(test - 1) != '\n' && (*test == ' ' || *test == '\t'))
                test--;
            *(test + 1) = '\0';
            i++;

            Cake_FileObjectSnapshot *snapshot = cake_list_file_object_container_add(snapshots[length - 1].currentContainers, keyPtr);
            snapshots = (Cake_FileObjectSnapshot *) realloc(snapshots, (length + 1) * sizeof(Cake_FileObjectSnapshot));
            snapshots[length].actualContainer = snapshot->actualContainer;
            snapshots[length].currentElements = snapshot->currentElements;
            snapshots[length].currentContainers = snapshot->currentContainers;
            snapshots[length].currentStrList    = snapshot->currentStrList;
            free(snapshot);
            snapshots[length].id = id;
            id++;
            length++;

            // On ignore les espaces et les sauts de lignes
            while(i != copy->data.length && (copy->bytes[i] == ' ' || copy->bytes[i] == '\t' || copy->bytes[i] == '\r' || copy->bytes[i] == '\n'))
                i++;
            keyPtr = &copy->bytes[i];
        }else if(copy->bytes[i] == '}') {
            length--;

            i++;
            // On ignore les espaces et les sauts de lignes
            while(i != copy->data.length && (copy->bytes[i] == ' ' || copy->bytes[i] == '\t' || copy->bytes[i] == '\r' || copy->bytes[i] == '\n'))
                i++;
            keyPtr = &copy->bytes[i];
        
        }else if(
            (copy->bytes[i] == '-') &&
            (
                i == 0                     ||
                copy->bytes[i - 1] == '\n' ||
                copy->bytes[i - 1] == '\r' ||
                copy->bytes[i - 1] == ' '  ||
                copy->bytes[i - 1] == '\t' ||
                copy->bytes[i - 1] == '{'  ||
                copy->bytes[i - 1] == '}'
            )
        ) {
            i++;
            // On ignore les espaces situés après le tiret
            while(i != copy->data.length && (copy->bytes[i] == ' ' || copy->bytes[i] == '\t'))
                i++;
            keyPtr = &copy->bytes[i];

            while(1) {
                if(i >= copy->data.length)
                    goto file_object_jumpk;
                if(copy->bytes[i] == '\r' || copy->bytes[i] == '\n') {
                    copy->bytes[i] = '\0';
                    i++;
                file_object_jumpk:
                    cake_list_strutf8_add_char_array(snapshots[length - 1].currentStrList, keyPtr);
                    break;
                }
                i++;
            }

            // On ignore tous les espaces et les sauts de lignes suivants
            while(i != copy->data.length && (copy->bytes[i] == ' ' || copy->bytes[i] == '\t' || copy->bytes[i] == '\r' || copy->bytes[i] == '\n'))
                i++;
            keyPtr = &copy->bytes[i];
        }else
            ++i;
        
    }
    free(snapshots);
    cake_free_strutf8(copy);

    return obj;
}

void cake_list_file_object_element_add(Cake_List_FileObjectElement *elements, const uchar *key, const uchar *value) {
    elements->list = (Cake_FileObjectElement **) realloc(elements->list, (elements->length + 1) * sizeof(Cake_FileObjectElement *));
    elements->list[elements->length] = (Cake_FileObjectElement *) malloc(sizeof(Cake_FileObjectElement));
    elements->list[elements->length]->key = cake_strutf8(key);
    elements->list[elements->length]->value = cake_strutf8(value);
    (elements->length)++;
}

Cake_FileObjectSnapshot *cake_list_file_object_container_add(Cake_List_FileObjectContainer *containers, const uchar *key) {
    Cake_FileObjectSnapshot *snapshot = (Cake_FileObjectSnapshot *) malloc(sizeof(Cake_FileObjectSnapshot));
    containers->list = (Cake_FileObjectContainer **) realloc(containers->list, (containers->length + 1) * sizeof(Cake_FileObjectContainer *));
    snapshot->actualContainer = containers->list[containers->length] = (Cake_FileObjectContainer *) malloc(sizeof(Cake_FileObjectContainer));
    snapshot->currentContainers = &containers->list[containers->length]->containers;
    snapshot->currentElements   = &containers->list[containers->length]->elements;
    containers->list[containers->length]->key = cake_strutf8(key);

    containers->list[containers->length]->elements.list   = NULL;
    containers->list[containers->length]->elements.length = 0;

    containers->list[containers->length]->containers.list   = NULL;
    containers->list[containers->length]->containers.length = 0;

    snapshot->currentStrList = containers->list[containers->length]->strList = cake_list_strutf8();

    (containers->length)++;
    return snapshot;
}



void cake_free_list_file_object_element(Cake_List_FileObjectElement *elements) {
    ulonglong i;
    for(i = 0; i < elements->length; ++i) {
        cake_free_strutf8(elements->list[i]->key);
        cake_free_strutf8(elements->list[i]->value);
        free(elements->list[i]);
    }
    free(elements->list);
}

Cake_FileObjectContainer *cake_file_object_get_container(Cake_FileObject *obj, const uchar *key) {
    Cake_String_UTF8 *copy = cake_strutf8(key);
    ulonglong dots = cake_str_number_of(key, '.') + 1;
    uchar *ptr = copy->bytes, *tempPtr;
    ulonglong i = 0, j;
    Cake_List_FileObjectContainer *containers = &obj->containers;
    Cake_FileObjectContainer *cont = NULL;
file_object_get_container_start:
    while(dots > 0 && i != copy->data.length) {
        if(containers->list == NULL) {
            cont = NULL;
            goto file_object_get_container_end;
        }
        if(i == copy->data.length - 1)
            goto skip;
        if(copy->bytes[i] == '.') {
            copy->bytes[i] = '\0';
        skip:
            tempPtr = ptr;
            i++;
            ptr = &copy->bytes[i];
            for(j = 0; j < containers->length; ++j) {
                if(cake_strutf8_equals(containers->list[j]->key, tempPtr)) {
                    cont = containers->list[j];
                    containers = &containers->list[j]->containers;
                    dots--;
                    goto file_object_get_container_start;
                }
            }
            cont = NULL;
            goto file_object_get_container_end;
        }
        i++;
    }
file_object_get_container_end:
    cake_free_strutf8(copy);
    return cont;
}

Cake_FileObjectElement *cake_file_object_get_element(Cake_FileObject *obj, const uchar *key) {
    Cake_String_UTF8 *copy = cake_strutf8(key);
    ulonglong dots = cake_str_number_of(key, '.');
    uchar *ptr = copy->bytes, *tempPtr;
    ulonglong i = 0, j;
    Cake_List_FileObjectElement *elements = &obj->elements;
    Cake_List_FileObjectContainer *containers = &obj->containers;
file_object_get_element_start:
    while(dots > 0 && i != copy->data.length) {
        if(containers->list == NULL)
            goto file_object_get_element_end;
        if(i == copy->data.length - 1)
            goto skip;
        if(copy->bytes[i] == '.') {
            copy->bytes[i] = '\0';
        skip:
            tempPtr = ptr;
            i++;
            ptr = &copy->bytes[i];
            for(j = 0; j < containers->length; ++j) {
                if(cake_strutf8_equals(containers->list[j]->key, tempPtr)) {
                    elements = &containers->list[j]->elements;
                    containers = &containers->list[j]->containers;
                    dots--;
                    goto file_object_get_element_start;
                }
            }
            goto file_object_get_element_end;
        }
        i++;
    }
    for(j = 0; j < elements->length; ++j)
        if(cake_strutf8_equals(elements->list[j]->key, ptr)) {
            cake_free_strutf8(copy);
            return elements->list[j];
        }
file_object_get_element_end:
    cake_free_strutf8(copy);
    return NULL;
}

// TODO: faire une fonction générique
Cake_List_String_UTF8 *cake_file_object_get_str_list(Cake_FileObject *obj, const uchar *key) {
    Cake_String_UTF8 *copy = cake_strutf8(key);
    ulonglong dots = cake_str_number_of(key, '.') + 1;
    uchar *ptr = copy->bytes, *tempPtr;
    ulonglong i = 0, j;
    Cake_List_FileObjectContainer *containers = &obj->containers;
    Cake_FileObjectContainer *cont = NULL;
file_object_get_str_list_start:
    while(dots > 0 && i != copy->data.length) {
        if(containers->list == NULL) {
            cont = NULL;
            goto file_object_get_str_list_end;
        }
        if(i == copy->data.length - 1)
            goto skip;
        if(copy->bytes[i] == '.') {
            copy->bytes[i] = '\0';
        skip:
            tempPtr = ptr;
            i++;
            ptr = &copy->bytes[i];
            for(j = 0; j < containers->length; ++j) {
                if(cake_strutf8_equals(containers->list[j]->key, tempPtr)) {
                    cont = containers->list[j];
                    containers = &containers->list[j]->containers;
                    dots--;
                    goto file_object_get_str_list_start;
                }
            }
            cont = NULL;
            goto file_object_get_str_list_end;
        }
        i++;
    }
file_object_get_str_list_end:
    cake_free_strutf8(copy);
    if(cont != NULL)
        return cont->strList;
    return NULL;
}

struct cake_strutf8_number {
    Cake_String_UTF8 *utf;
    ushort number;
};

void __cake_fileobject_utf8_list_elements_callback(Cake_List_FileObjectElement *elements, void *args) {
    struct cake_strutf8_number *fusion = (struct cake_strutf8_number *) args;
    ulonglong i;
    ushort j;
    for(i = 0; i < elements->length; ++i) {
        for(j = 0; j < fusion->number; ++j)
            cake_strutf8_add_char_array(fusion->utf, " ");
        cake_strutf8_add_char_array(fusion->utf, elements->list[i]->key->bytes);
        cake_strutf8_add_char_array(fusion->utf, ":");
        cake_strutf8_add_char_array(fusion->utf, elements->list[i]->value->bytes);
        cake_strutf8_add_char_array(fusion->utf, "\n");
    }
}

void __cake_fileobject_utf8_list_strutf8_callback(Cake_List_String_UTF8 *list, void *args) {
    struct cake_strutf8_number *fusion = (struct cake_strutf8_number *) args;
    ulonglong i;
    ushort j;
    for(i = 0; i < list->data.length; ++i) {
        for(j = 0; j < fusion->number; ++j)
            cake_strutf8_add_char_array(fusion->utf, " ");
        cake_strutf8_add_char_array(fusion->utf, "- ");
        cake_strutf8_add_char_array(fusion->utf, list->list[i]->bytes);
        cake_strutf8_add_char_array(fusion->utf, "\n");
    }
}

void __cake_fileobject_utf8_container_callback(Cake_FileObjectContainer *container, void *args) {
    struct cake_strutf8_number *fusion = (struct cake_strutf8_number *) args;
    ushort j;
    for(j = 0; j < fusion->number; ++j)
        cake_strutf8_add_char_array(fusion->utf, " ");
    cake_strutf8_add_char_array(fusion->utf, container->key->bytes);
    cake_strutf8_add_char_array(fusion->utf, " {\n");
    (fusion->number) += 2;
    __cake_fileobject_utf8_list_elements_callback(&container->elements, args);
    __cake_fileobject_utf8_list_strutf8_callback(container->strList, args);
}

void __cake_fileobject_utf8_container_end_callback(Cake_FileObjectContainer *container, void *args) {
    struct cake_strutf8_number *fusion = (struct cake_strutf8_number *) args;
    if(fusion->number > 1)
        (fusion->number) -= 2;
    ushort j;
    for(j = 0; j < fusion->number; ++j)
        cake_strutf8_add_char_array(fusion->utf, " ");
    cake_strutf8_add_char_array(fusion->utf, "}\n");
}

Cake_String_UTF8 *cake_fileobject_to_strutf8(Cake_FileObject *obj) {
    struct cake_strutf8_number fusion;
    fusion.utf = cake_strutf8("");
    fusion.number = 0;

    cake_fileobject_enum(
        obj,
        __cake_fileobject_utf8_list_elements_callback,
        __cake_fileobject_utf8_list_strutf8_callback,
        __cake_fileobject_utf8_container_callback,
        __cake_fileobject_utf8_container_end_callback,
        &fusion,
        &fusion,
        &fusion,
        &fusion
    );

    return fusion.utf;
}


void __cake_free_fileobject_list_elements_callback(Cake_List_FileObjectElement *elements, void *args) {
    cake_free_list_file_object_element(elements);
}

void __cake_free_fileobject_list_strutf8_callback(Cake_List_String_UTF8 *list, void *args) {
    cake_free_list_strutf8(list);
}

void __cake_free_fileobject_container_callback(Cake_FileObjectContainer *container, void *args) {
    cake_free_list_file_object_element(&container->elements);
    cake_free_list_strutf8(container->strList);
    cake_free_strutf8(container->key);
    free(container->containers.list);
    free(container);
}

void cake_free_fileobject(Cake_FileObject *obj) {
    cake_fileobject_enum(
        obj,
        __cake_free_fileobject_list_elements_callback,
        __cake_free_fileobject_list_strutf8_callback,
        NULL,
        __cake_free_fileobject_container_callback,
        NULL,
        NULL,
        NULL,
        NULL
    );
    free(obj);
}
