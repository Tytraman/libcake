#include "../fileobject.h"
#include "../fdio.h"

#include <stdio.h>

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

    ushort id = 0;

    Cake_FileObjectSnapshot *snapshots = (Cake_FileObjectSnapshot *) malloc(sizeof(Cake_FileObjectSnapshot));
    snapshots[0].actualContainer   = NULL;
    snapshots[0].currentElements   = &obj->elements;
    snapshots[0].currentContainers = &obj->containers;
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
        }else
            ++i;
        if(i == copy->data.length)
            break;
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
    (containers->length)++;
    return snapshot;
}

void cake_free_file_object(Cake_FileObject *obj) {
    cake_free_list_file_object_element(&obj->elements);
    free(obj);
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

typedef struct file_object_print {
    Cake_List_FileObjectContainer *currentContainers;
    ulonglong iContainers;
} FileObjectPrint;


Cake_String_UTF8 *cake_file_object_to_strutf8(Cake_FileObject *obj) {
    Cake_String_UTF8 *utf = cake_strutf8("");

    ulonglong i;
    ushort number = 0, k;
    for(i = 0; i < obj->elements.length; ++i) {
        cake_strutf8_add_char_array(utf, obj->elements.list[i]->key->bytes);
        cake_strutf8_add_char_array(utf, ":");
        cake_strutf8_add_char_array(utf, obj->elements.list[i]->value->bytes);
        cake_strutf8_add_char_array(utf, "\n");
    }
    if(obj->containers.list != NULL) {
        FileObjectPrint *snapshots = (FileObjectPrint *) malloc(sizeof(FileObjectPrint));
        ulonglong length = 1;
        snapshots[0].currentContainers = &obj->containers;
        snapshots[0].iContainers = 0;

        while(1) {
            /*
            printf(
                "[DEBUG] Affichage de la clé du container %llu/%llu\n",
                snapshots[length - 1].iContainers,
                snapshots[length - 1].currentContainers->length
            );
            */
            // Affichage de la clé du container
            for(k = 0; k < number; ++k)
                cake_strutf8_add_char_array(utf, " ");
            cake_strutf8_add_char_array(utf, snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->key->bytes);
            cake_strutf8_add_char_array(utf, " {\n");

            //printf("[DEBUG] Affichage des éléments...\n");
            // Affichage des éléments
            for(i = 0; i < snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->elements.length; ++i) {
                for(k = 0; k < number; ++k)
                    cake_strutf8_add_char_array(utf, " ");
                cake_strutf8_add_char_array(utf, "    ");
                cake_strutf8_add_char_array(utf, snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->elements.list[i]->key->bytes);
                cake_strutf8_add_char_array(utf, ":");
                cake_strutf8_add_char_array(utf, snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->elements.list[i]->value->bytes);
                cake_strutf8_add_char_array(utf, "\n");
            }

            /*
            ulonglong iTest;
            for(iTest = 0; iTest < snapshots[length - 1].currentContainers->length; ++iTest)
                printf("[DEBUG] <%s>\n", snapshots[length - 1].currentContainers->list[iTest]->key->bytes);
            */
            
            // Si le container possède d'autres containers
            if(snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->containers.list != NULL) {
                /*
                printf(
                    "[DEBUG] <%s> possède un autre container !\n",
                    snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->key->bytes
                );
                */
                snapshots = (FileObjectPrint *) realloc(snapshots, (length + 1) * sizeof(FileObjectPrint));
                snapshots[length].currentContainers = &snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->containers;
                snapshots[length].iContainers = 0;
                (snapshots[length - 1].iContainers)++;
                length++;
                number += 4;
            }else {
                /*
                printf(
                    "[DEBUG] <%s> ne possède pas d'autres containers.\n",
                    snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->key->bytes
                );
                */
               (snapshots[length - 1].iContainers)++;
                for(k = 0; k < number; ++k)
                    cake_strutf8_add_char_array(utf, " ");
                cake_strutf8_add_char_array(utf, "}\n");
                
                while(snapshots[length - 1].iContainers == snapshots[length - 1].currentContainers->length) {
                    length--;
                    if(length == 0)
                        goto file_object_print_exit;
                    number -= 4;
                    for(k = 0; k < number; ++k)
                        cake_strutf8_add_char_array(utf, " ");
                    cake_strutf8_add_char_array(utf, "}\n");
                }
            }
            //printf("[DEBUG] %llu\n", snapshots[length - 1].currentContainers->length);
            
            /*
            printf(
                "[DEBUG] <%s> iContainers: %llu/%llu\n",
                snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->key->bytes,
                snapshots[length - 1].iContainers,
                snapshots[length - 1].currentContainers->list[snapshots[length - 1].iContainers]->containers.length
            );
            */
            //printf("[DEBUG] Length à la fin: %llu\n", length);
            if(length == 0)
                break;
            
        }
    file_object_print_exit:
        free(snapshots);
    }
    return utf;
}
