#ifndef __PIKA_CONSOLE_H__
#define __PIKA_CONSOLE_H__

#include "def.h"
#include "utf8.h"

#ifdef PIKA_UNIX
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

typedef struct winsize ConsoleSize;

#define get_console_size(pConsoleSize) ioctl(STDOUT_FILENO, TIOCGWINSZ, pConsoleSize)
#else
typedef struct ConsoleSize {
    short ws_col;
    short ws_row;
} ConsoleSize;

void get_console_size(ConsoleSize *pConsoleSize);

void console_enable_ansi_sequence();
#define console_recover_mode() SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), s_ConsoleRecoveryMode)
#endif

// Caractère d'échappement.
#define CONSOLE_ESC "\x1B"
// \033 est la valeur octale de \x1B

#define CONSOLE_CLEAR CONSOLE_ESC"[2J"

// Liste des codes d'échappement, ne pas les utiliser en tant que tels mais favoriser les combos, par exemple CONSOLE_FG_WHITE pour écrire en blanc.
#define CONSOLE_CODE_RESET      "0"
#define CONSOLE_CODE_FG_BLACK   "30"
#define CONSOLE_CODE_FG_RED     "31"
#define CONSOLE_CODE_FG_GREEN   "32"
#define CONSOLE_CODE_FG_YELLOW  "33"
#define CONSOLE_CODE_FG_BLUE    "34"
#define CONSOLE_CODE_FG_MAGENTA "35"
#define CONSOLE_CODE_FG_CYAN    "36"
#define CONSOLE_CODE_FG_WHITE   "37"
#define CONSOLE_CODE_BG_BLACK   "40"
#define CONSOLE_CODE_BG_RED     "41"
#define CONSOLE_CODE_BG_GREEN   "42"
#define CONSOLE_CODE_BG_YELLOW  "43"
#define CONSOLE_CODE_BG_BLUE    "44"
#define CONSOLE_CODE_BG_MAGENTA "45"
#define CONSOLE_CODE_BG_CYAN    "46"
#define CONSOLE_CODE_BG_WHITE   "47"

// Retire tous les attributs d'échappement.
#define CONSOLE_RESET CONSOLE_ESC"["CONSOLE_CODE_RESET"m"

#define CONSOLE_FG_BLACK   CONSOLE_ESC"["CONSOLE_CODE_FG_BLACK"m"
#define CONSOLE_FG_RED     CONSOLE_ESC"["CONSOLE_CODE_FG_RED"m"
#define CONSOLE_FG_GREEN   CONSOLE_ESC"["CONSOLE_CODE_FG_GREEN"m"
#define CONSOLE_FG_YELLOW  CONSOLE_ESC"["CONSOLE_CODE_FG_YELLOW"m"
#define CONSOLE_FG_BLUE    CONSOLE_ESC"["CONSOLE_CODE_FG_BLUE"m"
#define CONSOLE_FG_MAGENTA CONSOLE_ESC"["CONSOLE_CODE_FG_MAGENTA"m"
#define CONSOLE_FG_CYAN    CONSOLE_ESC"["CONSOLE_CODE_FG_CYAN"m"
#define CONSOLE_FG_WHITE   CONSOLE_ESC"["CONSOLE_CODE_FG_WHITE"m"

#define CONSOLE_BG_BLACK   CONSOLE_ESC"["CONSOLE_CODE_BG_BLACK"m"
#define CONSOLE_BG_RED     CONSOLE_ESC"["CONSOLE_CODE_BG_RED"m"
#define CONSOLE_BG_GREEN   CONSOLE_ESC"["CONSOLE_CODE_BG_GREEN"m"
#define CONSOLE_BG_YELLOW  CONSOLE_ESC"["CONSOLE_CODE_BG_YELLOW"m"
#define CONSOLE_BG_BLUE    CONSOLE_ESC"["CONSOLE_CODE_BG_BLUE"m"
#define CONSOLE_BG_MAGENTA CONSOLE_ESC"["CONSOLE_CODE_BG_MAGENTA"m"
#define CONSOLE_BG_CYAN    CONSOLE_ESC"["CONSOLE_CODE_BG_CYAN"m"
#define CONSOLE_BG_WHITE   CONSOLE_ESC"["CONSOLE_CODE_BG_WHITE"m"

// Lettres noires sur fond...
#define CONSOLE_BLACK_ON_BLACK   CONSOLE_ESC"["CONSOLE_CODE_FG_BLACK";"CONSOLE_CODE_BG_BLACK"m"
#define CONSOLE_BLACK_ON_RED     CONSOLE_ESC"["CONSOLE_CODE_FG_BLACK";"CONSOLE_CODE_BG_RED"m"
#define CONSOLE_BLACK_ON_GREEN   CONSOLE_ESC"["CONSOLE_CODE_FG_BLACK";"CONSOLE_CODE_BG_GREEN"m"
#define CONSOLE_BLACK_ON_YELLOW  CONSOLE_ESC"["CONSOLE_CODE_FG_BLACK";"CONSOLE_CODE_BG_YELLOW"m"
#define CONSOLE_BLACK_ON_BLUE    CONSOLE_ESC"["CONSOLE_CODE_FG_BLACK";"CONSOLE_CODE_BG_BLUE"m"
#define CONSOLE_BLACK_ON_MAGENTA CONSOLE_ESC"["CONSOLE_CODE_FG_BLACK";"CONSOLE_CODE_BG_MAGENTA"m"
#define CONSOLE_BLACK_ON_CYAN    CONSOLE_ESC"["CONSOLE_CODE_FG_BLACK";"CONSOLE_CODE_BG_CYAN"m"
#define CONSOLE_BLACK_ON_WHITE   CONSOLE_ESC"["CONSOLE_CODE_FG_BLACK";"CONSOLE_CODE_BG_WHITE"m"

void get_console_cursor_pos(short *x, short *y);
void set_console_cursor_pos(short x, short y);
void console_clear_screen();

void draw_progress_bar(unsigned int current, unsigned int target, short widthScale, pika_char fillChar, pika_char emptyChar);
void clear_progress_bar();
void console_update_last_pos();

/*
    FONCTION ABANDONNEE, les efforts à fournir pour dev cette fonction sont trop énormes par rapport au gain,
    les cas d'utilisation de la fonction sont trop minimes pour réellement en avoir besoin.

        Récupère une entrée clavier.

        Paramètres :
        - buffer : pointeur vers un tableau de char, doit être initialisé à NULL.
        - bufferSize : pointeur vers la variable qui contiendra la taille du buffer, peut être NULL.

        La fonction free le buffer si il n'est pas égale à NULL.

        Retourne le nombre de caractères de la chaîne.

        Notes :
        La fonction est fonctionnelle mais a des énormes bugs qu'il faudrait prendre le temps de patcher.

        Pourquoi s'embêter à refaire une fonction comme celle-ci alors que scanf, fgets etc existent ?
        Aucune des fonctions déjà existantes ne me satisfaisait, scanf à cause du bufferoverflow,
        fgets pour le '\\n' et les effets bizarres quand le texte est plus long que la taille indiquée.
        Mais la raison principale est pour désactiver l'affichage de ^C quand Ctrl-C est appuyé, sur certains
        programmes ça casse tout.

        La fonction a été faite sur le tas, elle peut avoir des effets indésirables qu'il faudra patcher lors
        de leurs découvertes.
*/
void get_console_line(String_UTF8 *utf);

void console_hide_cursor(pika_bool value);
void console_scroll(pika_bool up, short value);

#endif