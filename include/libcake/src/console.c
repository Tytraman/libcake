#include "../console.h"
#include "../strutf8.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef CAKE_UNIX
#include <ctype.h>
#include <errno.h>
#else
static DWORD s_ConsoleRecoveryMode = 0;

void cake_get_console_size(Cake_ConsoleSize *pConsoleSize) {
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &screenInfo);
    pConsoleSize->ws_col = screenInfo.srWindow.Right - screenInfo.srWindow.Left + 1;
    pConsoleSize->ws_row = screenInfo.srWindow.Bottom - screenInfo.srWindow.Top + 1;
}
#endif

static short lastX, lastY;

void cake_console_update_last_pos() {
    cake_get_console_cursor_pos(&lastX, &lastY);
}

void cake_get_console_cursor_pos(short *x, short *y) {
    *x = 0;
    *y = 0;

    #ifdef CAKE_UNIX
    struct termios term, restore;
    tcgetattr(STDIN_FILENO, &term);
    restore = term;

    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    #else
    DWORD consoleMode, recoveryMode = 0;
    GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &recoveryMode);
    consoleMode = recoveryMode;
    consoleMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    consoleMode |= 0x0200;
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), consoleMode);

    cake_console_enable_ansi_sequence();

    DWORD bytes;
    #endif

    char b[] = { 0x1b, '[', '6', 'n' };
    #ifdef CAKE_UNIX
    write(STDOUT_FILENO, b, sizeof(b));
    #else
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), b, sizeof(b), &bytes, NULL);
    #endif

    unsigned int i;
    char c = 0;
    char buff[30] = { 0 };

    for(i = 0; c != 'R'; i++) {
        #ifdef CAKE_UNIX
        if(read(STDIN_FILENO, &c, sizeof(c)) <= 0) {
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &restore);
            return;
        }
        #else
        if(!ReadFile(GetStdHandle(STD_INPUT_HANDLE), &c, sizeof(c), &bytes, NULL)) {
            SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), recoveryMode);
            cake_console_recover_mode();
            return;
        }
        #endif
        buff[i] = c;
    }

    if(i < 2) {
        #ifdef CAKE_UNIX
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &restore);
        #else
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), recoveryMode);
        cake_console_recover_mode();
        #endif
        return;
    }

    int pow = 1;
    for(i -= 2; buff[i] != ';'; --i, pow *= 10)
        *x += (buff[i] - '0') * pow;
    
    pow = 1;
    for(--i; buff[i] != '['; --i, pow *= 10)
        *y += (buff[i] - '0') * pow;

    #ifdef CAKE_UNIX
    tcsetattr(STDIN_FILENO, TCSANOW, &restore);
    #else
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), recoveryMode);
    cake_console_recover_mode();
    #endif
}

void cake_set_console_cursor_pos(short x, short y) {
    char pos[30];
    int length = snprintf(pos, 30, "\033[%d;%dH", y, x);
    #ifdef CAKE_UNIX
    write(STDOUT_FILENO, pos, length * sizeof(char));
    #else
    /*
    COORD coord;
    coord.X = x - 1;
    coord.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    */
    cake_console_enable_ansi_sequence();
    DWORD bytes;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), pos, length * sizeof(char), &bytes, NULL);
    cake_console_recover_mode();
    #endif
}

#ifdef GTKJLDNJKLDJ
void get_console_line(Cake_String_UTF8 *utf) {
    if(read_mode)
        return;
    read_mode = cake_true;

    unsigned long long bufferPosition = 0;
    cake_bool isAnsiSequence = cake_false;

    unsigned long long textPosition = 0;

    #ifdef CAKE_UNIX
    struct termios recovery, term;
    tcgetattr(STDIN_FILENO, &recovery);
    tcgetattr(STDIN_FILENO, &term);

    /*
        ECHO désactive l'affichage des caractères
        ICANON fait que read retourne dès qu'un caractère est entré, au lieu d'attendre NL
        ISIG désactive Ctrl-C et Ctrl-Z
        IEXTEN désactive Ctrl-V (ça ne sert pas à coller)
    */
    term.c_lflag &= ~(ECHO | ICANON | IEXTEN);

    /*
        IXON désactive le contrôle du flow, Ctrl-S et Ctrl-Q
        ICRNL désactive la transformation de '\r' en '\n"
    */
    term.c_iflag &= ~(IXON | ICRNL);

    /*
        OPOST désactive la transformation de "\n" en "\r\n"
    */
    term.c_oflag &= ~(OPOST);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    #else
    DWORD recovery, term;
    GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &recovery);
    term = recovery;
    term &= ~(ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT | ENABLE_MOUSE_INPUT);
    term |= 0x0200;
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), term);
    #endif

    #ifdef CAKE_UNIX
    uchar byteCount = 0;
    #endif
    uchar bytesNeeded = 1;

    cake_char c;
    #ifdef CAKE_WINDOWS
    DWORD bytesRead;
    #endif
    cake_bool loop = cake_true;
    while(loop) {
        c = 0;
        #ifdef CAKE_UNIX
        if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
            loop = cake_false;
        #else
        if(!ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), &c, 1, &bytesRead, NULL))
            loop = cake_false;
        #endif

        switch(c) {
            // Ctrl-C
            case 3:
                break;
            // Backspace
            case 8:
            case 127:{
                if(utf->length > 0 && textPosition > 0) {
                    ulonglong currentPosition = bufferPosition;    // Position avant modification
                    ulonglong currentLength = utf->data.length;    // Longueur avant modification
                    ulonglong lastLength = utf->length;
                    
                    ulonglong i;

                    bufferPosition--;
                    textPosition--;
                    
                    // Tant que c'est un octet UTF-8
                    while((utf->bytes[bufferPosition] & 0xC0) == 0x80) {
                        bufferPosition--;
                    }

                    short x, y;
                    cake_get_console_cursor_pos(&x, &y);
                    if(x == 1) {
                        Cake_ConsoleSize size;
                        cake_get_console_size(&size);
                        x = size.ws_col;
                        y--;
                    }else
                        x--;

                    cake_strutf8_remove_index(utf, textPosition);
                    
                    // On recule le curseur
                    cake_set_console_cursor_pos(x, y);

                    // On affiche les nouvelles données
                    #ifdef CAKE_UNIX
                    write(STDOUT_FILENO, &utf->bytes[bufferPosition], (currentLength - currentPosition) * sizeof(cake_char));
                    #else
                    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), &utf->bytes[bufferPosition], (currentLength - currentPosition), &bytesRead, NULL);
                    #endif

                    // On affiche les espaces
                    char j = ' ';
                    #ifdef CAKE_UNIX
                    write(STDOUT_FILENO, &j, sizeof(j));
                    #else
                    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), &j, sizeof(j), &bytesRead, NULL);
                    #endif

                    // On remet le curseur à l'endroit où il était
                    cake_set_console_cursor_pos(x, y);
                }
                break;
            }
            // Entrée
            case 0xd:{
                loop = cake_false;
                char line[2] = { '\r', '\n' };
                #ifdef CAKE_UNIX
                write(STDOUT_FILENO, line, sizeof(line));
                #else
                WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), line, sizeof(line), &bytesRead, NULL);
                #endif
                break;
            }
            // Ctrl-Q
            case 17:
                loop = cake_false;
                break;
            // Escape code
            case 0x1b:{
                isAnsiSequence = cake_true;
                cake_char *ansiSequence = NULL;
                uchar ansiSequenceLength = 0;
                while(isAnsiSequence) {
                    #ifdef CAKE_UNIX
                    read(STDIN_FILENO, &c, sizeof(cake_char));
                    #else
                    ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), &c, 1, &bytesRead, NULL);
                    #endif
                    ansiSequence = (cake_char *) realloc(ansiSequence, (ansiSequenceLength + 1) * sizeof(cake_char) + sizeof(cake_char));
                    ansiSequence[ansiSequenceLength] = c;
                    ansiSequenceLength++;
                    ansiSequence[ansiSequenceLength] = 0;

                    //MessageBoxW(NULL, ansiSequence, L"", MB_OK);

                    // Del
                    if(ansiSequence[ansiSequenceLength - 1] == L_STR'~') {
                        isAnsiSequence = cake_false;
                    }else if(char_cmp(ansiSequence, L_STR"OP") == 0 || char_cmp(ansiSequence, L_STR"OQ") == 0 || char_cmp(ansiSequence, L_STR"OR") == 0 || char_cmp(ansiSequence, L_STR"OS") == 0) {
                        isAnsiSequence = cake_false;
                    }else {
                        switch(c) {
                            // Cursor up
                            case 'A':
                                isAnsiSequence = cake_false;
                                break;
                            // Cursor back
                            case 'D':{
                                isAnsiSequence = cake_false;
                                if(textPosition > 0) {
                                    short x, y;
                                    cake_get_console_cursor_pos(&x, &y);
                                    if(x == 1) {
                                        Cake_ConsoleSize size;
                                        cake_get_console_size(&size);
                                        cake_set_console_cursor_pos(size.ws_col, y - 1);
                                    }else {
                                        #ifdef CAKE_UNIX
                                        cake_char back[] = { 0x1b, '[', 'D' };
                                        write(STDOUT_FILENO, back, sizeof(back));
                                        #else
                                        cake_set_console_cursor_pos(x - 1, y);
                                        #endif
                                    }
                                    textPosition--;
                                    bufferPosition--;
                                    while((utf->bytes[bufferPosition] & 0xC0) == 0x80)
                                        bufferPosition--;
                                }
                                break;
                            }
                            // Cursor forward
                            case 'C':{
                                isAnsiSequence = cake_false;
                                if(textPosition < utf->length) {
                                    short x, y;
                                    cake_get_console_cursor_pos(&x, &y);
                                    Cake_ConsoleSize size;
                                    cake_get_console_size(&size);
                                    if(x == size.ws_col) {
                                        cake_set_console_cursor_pos(1, y + 1);
                                    }else {
                                        #ifdef CAKE_UNIX
                                        cake_char forward[] = { 0x1b, '[', 'C' };
                                        write(STDOUT_FILENO, forward, sizeof(forward));
                                        #else
                                        cake_set_console_cursor_pos(x + 1, y);
                                        #endif
                                    }
                                    textPosition++;
                                    if((utf->bytes[bufferPosition++] & 0xC0) == 0xC0) {
                                        while((utf->bytes[bufferPosition] & 0xC0) == 0x80)
                                            bufferPosition++;
                                    }
                                }
                                break;
                            }
                            // Cursor down
                            case 'B':
                                isAnsiSequence = cake_false;
                                break;
                            // Home
                            case 'H':
                                isAnsiSequence = cake_false;
                                break;
                            // End
                            case 'F':
                                isAnsiSequence = cake_false;
                                break;
                        }
                    }
                }
                free(ansiSequence);
                break;
            }
            // Caractères
            default:{
                short x, y;
                Cake_ConsoleSize size;
                cake_bool hideCursor = cake_false;
                #ifdef CAKE_WINDOWS
                bytesNeeded = 1;
                #endif

                #ifdef CAKE_UNIX
                // Si c'est le début d'un caractère UTF-8, on récupère le nombre d'octets
                // qui le constitut pour pouvoir afficher le texte à la fin.
                if((c & 0xC0) == 0xC0) {
                    byteCount = 1;
                    uchar hex = 0xE0;
                    bytesNeeded = 2;
                    while((c & hex) == hex) {
                        hex = (hex >> 1) | 0x80;
                        byteCount++;
                        bytesNeeded++;
                    }
                }else if((c & 0xC0) == 0x80) {
                    byteCount--;
                }else {
                    bytesNeeded = 1;
                    byteCount = 0;
                }
                #endif

                // On ajoute l'octet dans le buffer
                if(textPosition == utf->length)
                    #ifdef CAKE_WINDOWS
                    bytesNeeded = cake_strutf8_add_wchar(utf, c);
                    #else
                    strutf8_add_char(utf, c, byteCount == 0);
                    #endif
                else {
                    #ifdef CAKE_WINDOWS
                    bytesNeeded = cake_strutf8_insert_wchar(utf, textPosition, c);
                    #else
                    strutf8_insert_char_internal(utf, bufferPosition, c, byteCount == 0);
                    #endif
                    hideCursor = cake_true;
                }
                #ifdef CAKE_WINDOWS
                bufferPosition += bytesNeeded;
                #else
                bufferPosition++;
                #endif

                #ifdef CAKE_UNIX
                // On affiche le texte si le caractère est en ASCII ou que le caractère UTF-8 est complet.
                if(byteCount == 0) {
                #endif
                    cake_get_console_cursor_pos(&x, &y);
                    if(hideCursor)
                        cake_console_hide_cursor(cake_true);
                    #ifdef CAKE_UNIX
                    write(STDOUT_FILENO, &utf->bytes[bufferPosition - bytesNeeded], (utf->data.length - (bufferPosition - bytesNeeded)) * utf->data.byteSize);
                    #else
                    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), &utf->bytes[bufferPosition - bytesNeeded], (utf->data.length - (bufferPosition - bytesNeeded)) * utf->data.byteSize, &bytesRead, NULL);
                    #endif
                    cake_get_console_size(&size);
                    short tempX, tempY;
                    cake_get_console_cursor_pos(&tempX, &tempY);

                    #ifdef CAKE_UNIX
                    // On saute une ligne si on atteint la limite
                    /*
                    if(x + 1 > size.ws_col) {
                        cake_set_console_cursor_pos(1, y + 1);
                    }else
                    */
                        //cake_set_console_cursor_pos(x + 1, y);
                    #else
                    short destX = x + 1, destY = y;
                    if(tempY == size.ws_row) {
                        short lines = ((utf->length - textPosition) + (x % size.ws_col)) / size.ws_col;
                        destX = x + 1; 
                        destY = y - lines;
                        if(lines > 0)
                            cake_console_scroll(cake_true, lines);
                    }
                    if(textPosition != utf->length && x + 1 > size.ws_col) {
                        destX = 1;
                        destY++;
                    }
                    
                    cake_set_console_cursor_pos(destX, destY);
                    #endif

                    if(hideCursor)
                        cake_console_hide_cursor(cake_false);

                    textPosition++;
                #ifdef CAKE_UNIX
                }
                #endif
                break;
            }
        }
    }
    #ifdef CAKE_UNIX
    tcsetattr(STDIN_FILENO, TCSANOW, &recovery);
    #else
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), recovery);
    #endif
    read_mode = cake_false;
}
#endif

void cake_console_clear_screen() {
    char sequence[] = { 0x1B, '[', 'H', 0x1B, '[', 'J' };
    #ifdef CAKE_UNIX
    write(STDOUT_FILENO, sequence, sizeof(sequence));
    #else
    DWORD consoleMode, recoveryMode = 0;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &recoveryMode);
    consoleMode = recoveryMode;
    consoleMode |= 0x0004;
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), consoleMode);
    DWORD bytes;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), sequence, sizeof(sequence), &bytes, NULL);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), recoveryMode);
    #endif
}

/*
void cake_draw_progress_bar(unsigned int current, unsigned int target, short widthScale, cake_char fillChar, cake_char emptyChar) {
    char percent[6];

    // Il y a 4 signes pourcent car dans la librairie standard, pour afficher un pourcent, il faut mettre 2 signes,
    // si il n'y en avait que 2 dans le sprintf, une fois dans le printf, il n'y en aurait plus qu'un,
    // en en mettant 4, une fois dans le printf il y en aura 2, donc le signe s'affichera correctement.
    snprintf(percent, 6, "%d%%%%", ((100 * current) / target));
    short percentLength = strlen(percent) - 1;

    #ifdef CAKE_WINDOWS
    if(g_ScreenInfo.srWindow.Right < widthScale + 16 + percentLength)
        return;
    #else
    
    #endif

    #ifdef CAKE_WINDOWS
    COORD bottom;
    bottom.X = 0;
    bottom.Y = g_ScreenInfo.srWindow.Bottom;

    WORD recoveryAttributes = g_ScreenInfo.wAttributes;
    #endif

    Cake_ConsoleSize size;
    cake_get_console_size(&size);
    
    // Formule pour calculer la longueur de la barre d'avancement.
    short number = (((100.0f * (float) current) / (float) target) / 100.0f) * (float) widthScale;

    unsigned int i;

    const char *text = "Compilation: ";
    cake_char *progress = (cake_char *) malloc(number * sizeof(cake_char) + sizeof(cake_char));
    for(i = 0; i < number; i++)
        progress[i] = fillChar;
    progress[i] = 0;

    short emptLength = widthScale - number;
    cake_char *empt = (cake_char *) malloc(emptLength * sizeof(cake_char) + sizeof(cake_char));
    for(i = 0; i < emptLength; i++)
        empt[i] = emptyChar;
    empt[i] = 0;

    cake_set_console_cursor_pos(1, size.ws_row);
    
    if(g_LastY == bottom.Y) {
        char *emptiness = (char *) malloc(g_ScreenInfo.srWindow.Right * sizeof(char) + sizeof(char) * 2);
        for(i = 0; i < g_ScreenInfo.srWindow.Right; i++)
            emptiness[i] = ' ';
        emptiness[i] = '\0';
        printf(emptiness);
        GetConsoleScreenBufferInfo(g_Out, &g_ScreenInfo);
        bottom.Y = g_ScreenInfo.srWindow.Bottom;
        SetConsoleCursorPosition(g_Out, bottom);
        printf("\n");
        GetConsoleScreenBufferInfo(g_Out, &g_ScreenInfo);
        bottom.Y = g_ScreenInfo.srWindow.Bottom;
        SetConsoleCursorPosition(g_Out, bottom);
        g_LastY = bottom.Y - 1;
        free(emptiness);
    }
    //SetConsoleTextAttribute(g_Out, 1);
    printf(text);
    //SetConsoleTextAttribute(g_Out, recoveryAttributes);
    printf("[");
    //SetConsoleTextAttribute(g_Out, 2);
    printf(progress);
    //SetConsoleTextAttribute(g_Out, recoveryAttributes);
    printf("%s] %s", empt, percent);
    fflush(stdout);
    free(progress);
    free(empt);
    cake_set_console_cursor_pos(lastX, lastY);
}

void cake_clear_progress_bar() {
    unsigned int i;
    char *emptiness = (char *) malloc(g_ScreenInfo.srWindow.Right * sizeof(char) + sizeof(char) * 2);
    for(i = 0; i < g_ScreenInfo.srWindow.Right; i++)
        emptiness[i] = ' ';
    emptiness[i] = '\0';
    COORD bottom;
    bottom.X = 0;
    bottom.Y = g_ScreenInfo.srWindow.Bottom;
    SetConsoleCursorPosition(g_Out, bottom);
    printf(emptiness);
    free(emptiness);
    bottom.X = g_LastX;
    bottom.Y = g_LastY;
    SetConsoleCursorPosition(g_Out, bottom);
}
*/

void cake_console_hide_cursor(cake_bool value) {
    char sequence[] = { 0x1b, '[', '?', '2', '5', (value ? 'l' : 'h') };
    #ifdef CAKE_WINDOWS
    cake_console_enable_ansi_sequence();
    DWORD bytes;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), sequence, sizeof(sequence), &bytes, NULL);
    cake_console_recover_mode();
    #else
    write(STDOUT_FILENO, sequence, sizeof(sequence));
    #endif
}

void cake_console_scroll(cake_bool up, short value) {
    char sequence[30];
    int length = snprintf(sequence, sizeof(sequence), "\033[%hi%c", value, up ? 'S' : 'T');
    #ifdef CAKE_WINDOWS
    cake_console_enable_ansi_sequence();
    DWORD bytes;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), sequence, length * sizeof(char), &bytes, NULL);
    cake_console_recover_mode();
    #else
    write(STDOUT_FILENO, sequence, length * sizeof(char));
    #endif
}

#ifdef CAKE_WINDOWS
void cake_console_enable_ansi_sequence() {
    DWORD consoleMode;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &s_ConsoleRecoveryMode);
    consoleMode = s_ConsoleRecoveryMode;
    consoleMode |= 0x0004;
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), consoleMode);
}
#endif
