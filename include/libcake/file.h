/**
* @file file.h
* @brief Fichier contenant les types et les prototypes de tout ce qui se réfère aux fichiers ou aux dossiers.
* @author Tytraman
*/

#ifndef __CAKE_FILE_H__
#define __CAKE_FILE_H__

/// @cond
#include "def.h"
#include "strutf8.h"
/// @endcond

#ifdef CAKE_WINDOWS
#include <windows.h>

// ========== CONSTANTS ========== \\

#define CAKE_FILE_ACCESS_READ   GENERIC_READ
#define CAKE_FILE_ACCESS_WRITE  GENERIC_WRITE
#define CAKE_FILE_ACCESS_APPEND FILE_APPEND_DATA
#define CAKE_FILE_ACCESS_FOLDER FILE_LIST_DIRECTORY

#define CAKE_FILE_SHARE_NOTHING 0x00
#define CAKE_FILE_SHARE_DELETE  FILE_SHARE_DELETE
#define CAKE_FILE_SHARE_READ    FILE_SHARE_READ
#define CAKE_FILE_SHARE_WRITE   FILE_SHARE_WRITE

#define CAKE_FILE_OPEN_CREATE_ALWAYS        CREATE_ALWAYS
#define CAKE_FILE_OPEN_CREATE_IF_NOT_EXISTS CREATE_NEW
#define CAKE_FILE_OPEN_ALWAYS               OPEN_ALWAYS
#define CAKE_FILE_OPEN_IF_EXISTS            OPEN_EXISTING
#define CAKE_FILE_OPEN_TRUNC_IF_EXISTS      TRUNCATE_EXISTING

#define cake_close_dir(x) FindClose(x)

CAKE_C CAKE_API cake_bool cake_file_exists(const char *filename);
CAKE_C CAKE_API cake_bool cake_delete_file(const char *filename);
CAKE_C CAKE_API cake_bool cake_delete_folder(const char *pathname);

#else
/// @cond
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
/// @endcond

typedef DIR *cake_dir;

#define cake_close_dir(x) closedir(x)
#define cake_file_exists(filename) (access(filename, F_OK) == 0)
#define cake_delete_file(filename) unlink(filename)
#define cake_delete_folder(pathname) rmdir(pathname)
#endif


// ========== TYPES ========== \\

typedef cake_bool(*ListFileFilter)(Cake_String_UTF8 *filename, void *args);

typedef struct cake_filesnapshot {
    cake_fd dir;
    #ifdef CAKE_WINDOWS
    WIN32_FIND_DATAW dataw;
    #endif
    Cake_String_UTF8 path;
} Cake_FileSnapshot;

typedef struct cake_list_filesnapshot {
    Cake_FileSnapshot *list;
    ulonglong current;
    ulonglong length;
} Cake_List_FileSnapshot;

/**
 * @brief Structure pour ouvrir, lire et écrire dans des fichiers.
*/
typedef struct cake_file {
    cake_fd fd;                     ///< Descripteur du fichier.
    Cake_String_UTF8 *filename;     ///< Nom du fichier en UTF-8, sous Windows, convertie temporairement en UTF-16 lors de l'ouverture du fichier.
    cake_mask accessMode;           ///< Détermine le mode d'accès au fichier.
    cake_mask shareMode;            ///< Détermine la façon dont l'ouverture du fichier est partagé avec les autres processus.
} Cake_File;

/**
 * @brief Structure pour surveiller un dossier, par exemple lorsqu'un fichier a été modifié.
*/
typedef struct cake_folderwatcher {
    Cake_File folder;                                       ///< Le dossier à surveiller.
    cake_bool (*callback)(Cake_String_UTF8 *filename);      ///< Fonction de callback lorsque quelque chose a été détectée.
} Cake_FolderWatcher;


// ========== FILES AND FOLDERS ========== \\

/**
 * @brief Fonction interne, favoriser l'appel de @ref cake_open_file ou @ref cake_open_folder.
*/
CAKE_C CAKE_API cake_bool __cake_open_file(Cake_File *dest, const char *filename, cake_mask accessMode, cake_mask shareMode, cake_mask openMode, cake_mask attributes);

/**
 * @brief Ouvre un fichier.
 * @param[out] dest Structure destination qui contiendra le descripteur de fichier.
 * @param[in] filename Chemin absolu ou relatif du fichier à ouvrir.
 * @param[in] accessMode Mode d'accès au fichier.
 * @param[in] shareMode Mode de partage du fichier.
 * @param[in] openMode Mode d'ouverture du fichier.
 * @return @ref cake_false en cas d'erreur.
*/
inline cake_bool cake_open_file(Cake_File *dest, const char *filename, cake_mask accessMode, cake_mask shareMode, cake_mask openMode) {
    // TODO: Portage Linux
#ifdef CAKE_WINDOWS
    return __cake_open_file(dest, filename, accessMode, shareMode, openMode, FILE_ATTRIBUTE_NORMAL);
#else

#endif
}

/**
 * @brief Ouvre un dossier.
 * @param[out] dest Structure destination qui contiendra le descripteur de fichier.
 * @param[in] folderpath Chemin absolu ou relatif du dossier à ouvrir.
 * @param[in] accessMode Mode d'accès au dossier.
 * @param[in] shareMode Mode de partage du dossier.
 * @param[in] openMode Mode d'ouverture du dossier.
 * @return @ref cake_false en cas d'erreur.
*/
inline cake_bool cake_open_folder(Cake_File *dest, const char *folderpath, cake_mask accessMode, cake_mask shareMode, cake_mask openMode) {
    // TODO: Portage Linux
#ifdef CAKE_WINDOWS
    return __cake_open_file(dest, folderpath, accessMode | CAKE_FILE_ACCESS_FOLDER, shareMode, openMode, FILE_FLAG_BACKUP_SEMANTICS);
#else

#endif
}

/**
 * @brief Ferme un fichier ou un dossier ouvert.
 * @param[in] file Le fichier ou le dossier à fermer.
*/
CAKE_C CAKE_API void cake_close_file(Cake_File *file);

/**
 * @brief Lit le contenu du fichier à partir de la position interne géré par l'OS.
 * @param[in] file Le fichier à lire.
 * @param[out] buffer Destination des octets lus.
 * @param[in] size Nombre d'octets à lire.
 * @param[out] bytesRead Nombre d'octets lus.
 * @return @ref cake_false en cas d'erreur.
*/
CAKE_C CAKE_API inline cake_bool cake_file_read(Cake_File *file, void *buffer, cake_size size, cake_size *bytesRead) {
    // TODO: Portage Linux
#ifdef CAKE_WINDOWS
    return ReadFile(file->fd, buffer, size, bytesRead, NULL);
#else

#endif
}

/**
 * @brief Écrit du contenu dans le fichier à partir de la position interne géré par l'OS.
 * @param[out] file Le fichier dans lequel écrire.
 * @param[in] buffer Adresse des valeurs sources à écrire.
 * @param[in] size Taille du buffer source.
 * @param[out] bytesWritten Nombre d'octets écris.
 * @return @ref cake_false en cas d'erreur.
*/
CAKE_C CAKE_API inline cake_bool cake_file_write(Cake_File *file, void *buffer, cake_size size, cake_size *bytesWritten) {
    // TODO: Portage Linux
#ifdef CAKE_WINDOWS
    return WriteFile(file->fd, buffer, size, bytesWritten, NULL);
#else

#endif
}


// ========== FOLDER WATCHER ========== \\

/**
 * @brief Attend qu'un fichier situé dans le dossier voit son contenu modifié.
 * @param[in] watcher Le dossier à surveiller.
 * @return La valeur du callback. @ref cake_false si le `callback` est `NULL`.
*/
CAKE_C CAKE_API cake_bool cake_folder_watcher_start(Cake_FolderWatcher *watcher);


// ========== FILES LISTING ========== \\

CAKE_C CAKE_API void cake_create_list_filesnapshot(Cake_List_FileSnapshot *list);
CAKE_C CAKE_API cake_bool cake_list_filesnapshot_add(Cake_String_UTF8 *newPath, Cake_List_FileSnapshot *list);
CAKE_C CAKE_API void cake_list_filesnapshot_remove_last(Cake_List_FileSnapshot *list);

CAKE_C CAKE_API void cake_list_files_recursive(const char *path, Cake_List_String_UTF8 *files, Cake_List_String_UTF8 *folders, ListFileFilter filter, void *args);

/**
 * @brief Crée tous les dossiers du chemin passé.
 * @param filepath Chemin final voulu.
 * @return @ref cake_false si une erreur est survenue.
 */
CAKE_C CAKE_API cake_bool cake_mkdirs(const char *filepath);

#endif