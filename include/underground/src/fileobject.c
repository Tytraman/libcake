#include "../fileobject.h"
#include "../fdio.h"

#include <stdio.h>

typedef struct fileobject_print {
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
                        goto fileobject_print_exit;
                    if(container_end_callback != NULL)
                        container_end_callback(snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers - 1], containerEndArgs);
                }
            }
            if(length == 0)
                break;
            
        }
    fileobject_print_exit:
        free(snapshots);
    }
}

Cake_FileObject *cake_fileobject_load(const char *filename) {
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

    // On ignore les espaces et les sauts de lignes du début du fichier
    while(
        i != copy->data.length &&
        (
            copy->bytes[i] == '\r' ||
            copy->bytes[i] == '\n' ||
            copy->bytes[i] == ' '  ||
            copy->bytes[i] == '\t' 
        )
    )
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
                    goto fileobject_jump;
                if(copy->bytes[i] == '\r' || copy->bytes[i] == '\n') {
                    copy->bytes[i] = '\0';
                    i++;
                fileobject_jump:
                    cake_list_fileobject_element_add(snapshots[length - 1].currentElements, (cchar_ptr) keyPtr, (cchar_ptr) valuePtr);
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

            Cake_FileObjectSnapshot *snapshot = cake_list_fileobject_container_add(snapshots[length - 1].currentContainers, (cchar_ptr) keyPtr);
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
                    goto fileobject_jumpk;
                if(copy->bytes[i] == '\r' || copy->bytes[i] == '\n') {
                    copy->bytes[i] = '\0';
                    i++;
                fileobject_jumpk:
                    cake_list_strutf8_add_char_array(snapshots[length - 1].currentStrList, (cchar_ptr) keyPtr);
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

Cake_FileObjectElement *cake_list_fileobject_element_add(Cake_List_FileObjectElement *elements, const char *key, const char *value) {
    ulonglong i;
    for(i = 0; i < elements->length; ++i)
        if(cake_strutf8_equals(elements->list[i]->key, key))
            return NULL;
    elements->list = (Cake_FileObjectElement **) realloc(elements->list, (elements->length + 1) * sizeof(Cake_FileObjectElement *));
    Cake_FileObjectElement *element = elements->list[elements->length] = (Cake_FileObjectElement *) malloc(sizeof(Cake_FileObjectElement));
    elements->list[elements->length]->key = cake_strutf8(key);
    elements->list[elements->length]->value = cake_strutf8(value);
    (elements->length)++;
    return element;
}

cake_bool cake_list_fileobject_element_remove(Cake_List_FileObjectElement *elements, const char *key) {
    if(elements->length == 0)
        return cake_false;
    ulonglong i;
    for(i = 0; i < elements->length; ++i) {
        if(cake_strutf8_equals(elements->list[i]->key, key)) {
            cake_free_strutf8(elements->list[i]->key);
            cake_free_strutf8(elements->list[i]->value);
            free(elements->list[i]);
            if(i < elements->length - 1)
                memcpy(&elements->list[i], &elements->list[i + 1], (elements->length - i - 1) * sizeof(Cake_FileObjectElement *));
            if(elements->length - 1 > 0) {
                Cake_FileObjectElement **testPtr = (Cake_FileObjectElement **) realloc(elements->list, (elements->length - 1) * sizeof(Cake_FileObjectElement *));
                if(testPtr != NULL) {   
                    elements->list = testPtr;
                    (elements->length)--;
                    return cake_true;
                }else
                    return cake_false;
            }else {
                elements->length = 0;
                free(elements->list);
                elements->list = NULL;
                return cake_true;
            }
        }
    }
    return cake_false;
}

Cake_FileObjectSnapshot *cake_list_fileobject_container_add(Cake_List_FileObjectContainer *containers, const char *key) {
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

void cake_free_list_fileobject_element(Cake_List_FileObjectElement *elements) {
    ulonglong i;
    for(i = 0; i < elements->length; ++i) {
        cake_free_strutf8(elements->list[i]->key);
        cake_free_strutf8(elements->list[i]->value);
        free(elements->list[i]);
    }
    free(elements->list);
}
 
void *__cake_fileobject_get(
    Cake_FileObject *obj,
    const char *key,
    void *(*container_callback)(Cake_FileObjectContainer *container, Cake_List_FileObjectContainer **dest, const char *value, void *args),
    void *(*element_callback)(Cake_FileObjectElement *element, const char *value, void *args),
    void *containerArgs,
    void *elementArgs
) {
    void *ret = NULL;
    Cake_String_UTF8 *copy = cake_strutf8(key);
    uchar *lastPtr = copy->bytes, *ptr;
    ulonglong internalIndex = 0;
    ulonglong i;
    Cake_List_FileObjectContainer *containers = &obj->containers;
    Cake_List_FileObjectElement   *elements   = &obj->elements;

    Cake_FileObjectContainer *tempContainer;

    cake_bool loop = cake_true;
    while(loop) {
        if(containers->list == NULL && element_callback == NULL)
            break;
        ptr = cake_strutf8_search_from_start(copy, ".", &internalIndex);
        if(ptr != NULL)
            *ptr = '\0';
        else {
            loop = cake_false;
            if(element_callback != NULL) {
                if(elements->length == 0) {
                    ret = NULL;
                    goto cancel;
                }
                for(i = 0; i < elements->length; ++i) {
                    ret = element_callback(elements->list[i], (cchar_ptr) lastPtr, elementArgs);
                    if(ret != NULL)
                        goto cancel;
                }
                goto cancel;
            }
        }
        for(i = 0; i < containers->length; ++i) {
            tempContainer = containers->list[i];
            // Lorsqu'un container est trouvé, le callback est appelé,
            // si celui-ci retourne autre chose que NULL, la fonction se termine
            // et retourne ce qu'a retourné le callback.
            ret = container_callback(containers->list[i], &containers, (cchar_ptr) lastPtr, containerArgs);
            if(ret != NULL) {
                if(element_callback == NULL && ptr != NULL && containers->length == 0)
                    goto nothing;
                goto ok;
            }
        }
    nothing:
        ret = NULL;
        goto cancel;
    ok:
        elements = &tempContainer->elements;
        lastPtr = ptr + 1;
    }

cancel:
    cake_free_strutf8(copy);
    return ret;
}

void *__cake_fileobject_get_container_callback(Cake_FileObjectContainer *container, Cake_List_FileObjectContainer **dest, const char *value, void *args) {
    if(cake_strutf8_equals(container->key, value)) {
        *dest = &container->containers;
        return container;
    }
    return NULL;
}

void *__cake_fileobject_get_container_callback_ext(Cake_FileObjectContainer *container, Cake_List_FileObjectContainer **dest, const char *value, void *args) {
    if(cake_strutf8_equals(container->key, value)) {
        Cake_List_FileObjectContainer **fromList = (Cake_List_FileObjectContainer **) args;
        *fromList = *dest;
        *dest = &container->containers;
        return container;
    }
    return NULL;
}

Cake_FileObjectContainer *cake_fileobject_get_container(Cake_FileObject *obj, const char *key) {
    return (Cake_FileObjectContainer *) __cake_fileobject_get(
        obj,
        key,
        __cake_fileobject_get_container_callback,
        NULL,
        NULL,
        NULL
    );
}

Cake_FileObjectContainer *cake_fileobject_get_container_ext(Cake_FileObject *obj, const char *key, Cake_List_FileObjectContainer **fromList) {
    return (Cake_FileObjectContainer *) __cake_fileobject_get(
        obj,
        key,
        __cake_fileobject_get_container_callback_ext,
        NULL,
        fromList,
        NULL
    );
}

void *__cake_fileobject_get_element_callback(Cake_FileObjectElement *element, const char *value, void *args) {
    if(cake_strutf8_equals(element->key, value)) {
        return element;
    }
    return NULL;
}

Cake_FileObjectElement *cake_fileobject_get_element(Cake_FileObject *obj, const char *key) {
    return (Cake_FileObjectElement *) __cake_fileobject_get(
        obj,
        key,
        __cake_fileobject_get_container_callback,
        __cake_fileobject_get_element_callback,
        NULL,
        NULL
    );
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
        cake_strutf8_add_char_array(fusion->utf, (cchar_ptr) elements->list[i]->key->bytes);
        cake_strutf8_add_char_array(fusion->utf, ":");
        cake_strutf8_add_char_array(fusion->utf, (cchar_ptr) elements->list[i]->value->bytes);
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
        cake_strutf8_add_char_array(fusion->utf, (cchar_ptr) list->list[i]->bytes);
        cake_strutf8_add_char_array(fusion->utf, "\n");
    }
}

void __cake_fileobject_utf8_container_callback(Cake_FileObjectContainer *container, void *args) {
    struct cake_strutf8_number *fusion = (struct cake_strutf8_number *) args;
    ushort j;
    for(j = 0; j < fusion->number; ++j)
        cake_strutf8_add_char_array(fusion->utf, " ");
    cake_strutf8_add_char_array(fusion->utf, (cchar_ptr) container->key->bytes);
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
    if(elements->list != NULL)
        cake_free_list_fileobject_element(elements);
}

void __cake_free_fileobject_list_strutf8_callback(Cake_List_String_UTF8 *list, void *args) {
    cake_free_list_strutf8(list);
}

void __cake_free_fileobject_container_callback(Cake_FileObjectContainer *container, void *args) {
    if(container->elements.list != NULL) {
        cake_free_list_fileobject_element(&container->elements);
    }
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

void cake_free_fileobject_stack(Cake_FileObject *obj) {
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
}

Cake_FileObject *cake_fileobject() {
    Cake_FileObject *obj = (Cake_FileObject *) malloc(sizeof(Cake_FileObject));
    if(obj != NULL) {
        obj->containers.length = 0;
        obj->containers.list = NULL;
        obj->elements.length = 0;
        obj->elements.list = NULL;
        obj->strList = cake_list_strutf8();
    }
    return obj;
}

Cake_FileObjectElement *cake_fileobject_add_element(Cake_FileObject *obj, const char *key, const char *value) {
    Cake_String_UTF8 *copy = cake_strutf8(key);

    uchar *lastPtr= copy->bytes;
    uchar *ptr = copy->bytes;
    ulonglong i;
    Cake_List_FileObjectContainer *current = &obj->containers;
    Cake_List_FileObjectElement *listElements = &obj->elements;
    Cake_FileObjectElement *element = NULL;

    while(1) {
        if(ptr == &copy->bytes[copy->data.length]) {
            element = cake_list_fileobject_element_add(listElements, (cchar_ptr) lastPtr, value);
            break;
        }
        if(*ptr == '.') {
            *ptr = '\0';
            for(i = 0; i < current->length; ++i) {
                if(cake_strutf8_equals(current->list[i]->key, (cchar_ptr) lastPtr)) {
                    listElements = &current->list[i]->elements;
                    current = &current->list[i]->containers;
                    goto skip_add_cont;
                }
            }
            // Si le container n'existe pas
            current->list = (Cake_FileObjectContainer **) realloc(current->list, (current->length + 1) * sizeof(Cake_FileObjectContainer *));
            current->list[current->length] = (Cake_FileObjectContainer *) malloc(sizeof(Cake_FileObjectContainer));
            current->list[current->length]->key = cake_strutf8((cchar_ptr) lastPtr);
            current->list[current->length]->strList = cake_list_strutf8();
            listElements = &current->list[current->length]->elements;
            current->list[current->length]->elements.length = 0;
            current->list[current->length]->elements.list = NULL;
            current->list[current->length]->containers.length = 0;
            current->list[current->length]->containers.list = NULL;
            (current->length)++;
            current = &current->list[current->length - 1]->containers;
        skip_add_cont:
            lastPtr = ptr + 1;
        }
        ptr++;
    }

    cake_free_strutf8(copy);
    return element;
}

Cake_FileObjectElement *cake_fileobject_add_element_from(Cake_FileObjectContainer *container, const char *key, const char *value) {
    Cake_FileObject obj;
    obj.containers.length = container->containers.length;
    obj.containers.list   = container->containers.list;
    obj.elements.length   = container->elements.length;
    obj.elements.list     = container->elements.list;
    Cake_FileObjectElement *ele = cake_fileobject_add_element(&obj, key, value);
    container->containers.length = obj.containers.length;
    container->containers.list   = obj.containers.list;
    container->elements.length   = obj.elements.length;
    container->elements.list     = obj.elements.list;
    return ele;
}

cake_bool cake_fileobject_remove_element(Cake_FileObject *obj, const char *key) {
    Cake_String_UTF8 *copy = cake_strutf8(key);

    uchar *lastPtr = copy->bytes;
    uchar *ptr = copy->bytes;

    Cake_List_FileObjectContainer *current = &obj->containers;
    Cake_List_FileObjectElement *listElements = &obj->elements;
    ulonglong i;

    while(1) {
        if(ptr == &copy->bytes[copy->data.length]) {
            cake_bool retCode = cake_list_fileobject_element_remove(listElements, (cchar_ptr) lastPtr);
            cake_free_strutf8(copy);
            return retCode;
        }
        if(*ptr == '.') {
            *ptr = '\0';
            for(i = 0; i < current->length; ++i) {
                if(cake_strutf8_equals(current->list[i]->key, (cchar_ptr) lastPtr)) {
                    listElements = &current->list[i]->elements;
                    current = &current->list[i]->containers;
                    goto skip_add_cont;
                }
            }
            // Si le container n'existe pas
            break;
        skip_add_cont:
            lastPtr = ptr + 1;
        }
        ptr++;
    }

    cake_free_strutf8(copy);
    return cake_false;
}

cake_bool cake_fileobject_remove_element_from(Cake_FileObjectContainer *container, const char *key) {
    Cake_FileObject obj;
    obj.containers.length = container->containers.length;
    obj.containers.list   = container->containers.list;
    obj.elements.length   = container->elements.length;
    obj.elements.list     = container->elements.list;
    if(cake_fileobject_remove_element(&obj, key)) {
        container->containers.length = obj.containers.length;
        container->containers.list   = obj.containers.list;
        container->elements.length   = obj.elements.length;
        container->elements.list     = obj.elements.list;
        return cake_true;
    }
    return cake_false;
}

Cake_FileObjectContainer *cake_fileobject_get_container_from(Cake_FileObjectContainer *container, const char *key) {
    Cake_FileObject obj;
    obj.containers.length = container->containers.length;
    obj.containers.list   = container->containers.list;
    obj.elements.length   = container->elements.length;
    obj.elements.list     = container->elements.list;
    obj.strList           = container->strList;

    return (Cake_FileObjectContainer *) __cake_fileobject_get(
        &obj, key,
        __cake_fileobject_get_container_callback,
        NULL, NULL, NULL
    );
}

Cake_FileObjectElement *cake_fileobject_get_element_from(Cake_FileObjectContainer *container, const char *key) {
    Cake_FileObject obj;
    obj.containers.length = container->containers.length;
    obj.containers.list   = container->containers.list;
    obj.elements.length   = container->elements.length;
    obj.elements.list     = container->elements.list;
    obj.strList = container->strList;

    return cake_fileobject_get_element(&obj, key);
}

cake_bool cake_fileobject_remove_container(Cake_FileObject *obj, const char *key) {
    Cake_List_FileObjectContainer *fromList;
    Cake_FileObjectContainer *container = cake_fileobject_get_container_ext(obj, key, &fromList);
    if(container == NULL)
        return cake_false;

    // On supprime d'abord la clé puisqu'on "converti" le container en objet et qu'il n'y a pas de clé dans les objets.
    cake_free_strutf8(container->key);
    Cake_FileObject object;
    object.containers.length = container->containers.length;
    object.containers.list   = container->containers.list;
    object.elements.length   = container->elements.length;
    object.elements.list     = container->elements.list;
    object.strList           = container->strList;

    // On free de manière récursive tous les éléments du container.
    cake_free_fileobject_stack(&object);

    // Réallocation de la liste dans laquelle se situait le container.
    if(fromList->length > 1) {
        memcpy(container, container + 1, (fromList->list[fromList->length - 1] - container) * sizeof(Cake_FileObjectContainer *));
        void *ptr = realloc(fromList->list, (fromList->length - 1) * sizeof(Cake_FileObject *));
        if(ptr != NULL)
            fromList->list = (Cake_FileObjectContainer **) ptr;
        (fromList->length)--;
    }else {
        free(fromList->list);
        fromList->list = NULL;
        fromList->length = 0;
    }

    return cake_true;
}

cake_bool cake_fileobject_remove_container_from(Cake_FileObjectContainer *container, const char *key) {
    Cake_FileObject object;
    object.containers.length = container->containers.length;
    object.containers.list   = container->containers.list;
    object.elements.length   = container->elements.length;
    object.elements.list     = container->elements.list;
    object.strList           = container->strList;
    if(cake_fileobject_remove_container(&object, key)) {
        container->containers.length = object.containers.length;
        container->containers.list   = object.containers.list;
        return cake_true;
    }
    return cake_false;
}
