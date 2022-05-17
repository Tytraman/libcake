#include "../window.h"

#include <stdlib.h>
#include <stdio.h>

struct cake_fusion_index_thread {
    ulonglong current;
    Cake_List_Window list;
};

#ifdef CAKE_WINDOWS
DWORD __indexThreadWindow;
#else
#include <pthread.h>
pthread_key_t __indexThreadWindow;
#endif

cake_bool cake_window_init() {
    #ifdef CAKE_WINDOWS
    __indexThreadWindow = TlsAlloc();
    if(__indexThreadWindow == TLS_OUT_OF_INDEXES)
        return cake_false;
    return cake_true;
    #else
    if(pthread_key_create(&__indexThreadWindow, NULL) == 0)
        return cake_true;
    return cake_false;
    #endif
}

#ifdef CAKE_WINDOWS
void cake_window_cleanup() {
    TlsFree(__indexThreadWindow);
}
#endif

cake_bool cake_window_init_thread() {
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) malloc(sizeof(*fusion));
    if(fusion == NULL)
        return cake_false;
    fusion->current = (ulonglong) -1;
    fusion->list.list = NULL;
    fusion->list.length = 0;
    #ifdef CAKE_WINDOWS
    TlsSetValue(__indexThreadWindow, fusion);
    #else
    pthread_setspecific(__indexThreadWindow, fusion);
    #endif
    return cake_true;
}

void cake_window_cleanup_thread() {
    #ifdef CAKE_WINDOWS
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    #else
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) pthread_getspecific(__indexThreadWindow);
    #endif
    if(fusion != NULL) {
        free(fusion->list.list);
        free(fusion);
    }
}

cake_bool cake_window_set_current(ulonglong value) {
    #ifdef CAKE_WINDOWS
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    #else
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) pthread_getspecific(__indexThreadWindow);
    #endif
    if(fusion == NULL)
        return cake_false;
    fusion->current = value;
    return cake_true;
}

cake_bool cake_window_inc_current(ulonglong value) {
    #ifdef CAKE_WINDOWS
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    #else
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) pthread_getspecific(__indexThreadWindow);
    #endif
    if(fusion == NULL)
        return cake_false;
    (fusion->current) += value;
    return cake_true;
}

cake_bool cake_windows_dec_current(ulonglong value) {
    #ifdef CAKE_WINDOWS
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    #else
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) pthread_getspecific(__indexThreadWindow);
    #endif
    if(fusion == NULL)
        return cake_false;
    (fusion->current) -= value;
    return cake_true;
}

#ifdef CAKE_WINDOWS
cake_window_res __cake_window_proc(cake_window_handle hwnd, uint msg, cake_wparam wparam, cake_lparam lparam) {
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    if(fusion == NULL)
        return cake_window_proc_default(hwnd, msg, wparam, lparam);
    switch(msg) {
        default: break;
        case CAKE_WINDOW_EVENT_RESIZE:
            fusion->list.list[fusion->current]->widget.width = lparam & 0xFFFF;
            fusion->list.list[fusion->current]->widget.height = (lparam >> 16) & 0xFFFF;
            break;
        case CAKE_WINDOW_EVENT_MOVE:
            fusion->list.list[fusion->current]->widget.x = (short) (lparam & 0xFFFF);
            fusion->list.list[fusion->current]->widget.y = (short) (((lparam >> 16) & 0xFFFF));
            break;
    }
    if(fusion->list.list[fusion->current]->proc != NULL)
        return fusion->list.list[fusion->current]->proc(hwnd, msg, wparam, lparam, fusion->list.list[fusion->current]);
    return cake_window_proc_default(hwnd, msg, wparam, lparam);
}
#endif

Cake_Window *cake_window(
    Cake_Window_Widget *parent,
    const char *className,
    const char *title,
    int x, int y,
    int width, int height,
    uchar r, uchar g, uchar b,
    Cake_Window_Events *events
) {
    #ifdef CAKE_WINDOWS
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    #else
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) pthread_getspecific(__indexThreadWindow);
    #endif
    if(fusion == NULL)
        return NULL;
    Cake_Window *window = (Cake_Window *) malloc(sizeof(*window));
    if(window == NULL)
        return NULL;
    
    void *ptr = realloc(fusion->list.list, (fusion->list.length + 1) * sizeof(*fusion->list.list));
    if(ptr == NULL) {
        free(window);
        return NULL;
    }
    #ifdef CAKE_UNIX
    window->widget.dpy = XOpenDisplay(NULL);
    if(window->widget.dpy == NULL) {
        fprintf(stderr, "Erreur X11: impossible d'ouvrir le Display.\n");
        free(window);
        return cake_false;
    }

    char colorBuff[16];
    snprintf(colorBuff, sizeof(colorBuff), "rgb:%02x/%02x/%02x", r, g, b);
    XColor color;
    Colormap colormap = XDefaultColormap(window->widget.dpy, window->widget.screen);
    XParseColor(window->widget.dpy, colormap, colorBuff, &color);
    XAllocColor(window->widget.dpy, colormap, &color);

    window->widget.screen = XDefaultScreen(window->widget.dpy);

    window->widget.parent = parent;
    Window parentWin;
    if(parent != NULL) {
        parentWin = parent->win;
        window->widget.rootWin = parent->rootWin;
    }else {
        parentWin = window->widget.rootWin = XRootWindow(window->widget.dpy, window->widget.screen);
    }

    window->widget.win = XCreateSimpleWindow(
        window->widget.dpy,
        parentWin,
        x, y, width, height, 1, XBlackPixel(window->widget.dpy, window->widget.screen), color.pixel
    );

    XSelectInput(window->widget.dpy, window->widget.win, ExposureMask | KeyPressMask | StructureNotifyMask);
    #endif

    fusion->list.list = (Cake_Window **) ptr;
    fusion->list.list[fusion->list.length] = window;
    (fusion->list.length)++;

    // Il est initialisé à (ulonglong) -1, donc le premier appel de cette fonction mettra cette valeur à 0
    (fusion->current)++;
        
    window->widget.parent = parent;
    window->widget.type = CAKE_WINDOW_WIDGET_WINDOW;
    window->widget.x = x;
    window->widget.y = y;
    window->widget.width  = width;
    window->widget.height = height;

    window->widgets.list = NULL;
    window->widgets.length = 0;

    window->className = cake_strutf8(className);
    window->title     = cake_strutf8(title);

    XClassHint winHints;
    winHints.res_class = (char *) window->className->bytes;
    winHints.res_name  = (char *) window->className->bytes;
    XSetClassHint(window->widget.dpy, window->widget.win, &winHints);
    if(events != NULL) {
        window->events.moveEvent    = events->moveEvent;
        window->events.resizeEvent  = events->resizeEvent;
        window->events.destroyEvent = events->destroyEvent;
    }else {
        window->events.moveEvent    = NULL;
        window->events.resizeEvent  = NULL;
        window->events.destroyEvent = NULL;
    }

    Atom wmDeleteWin = XInternAtom(window->widget.dpy, "WM_DELETE_WINDOW", 1);
    XSetWMProtocols(window->widget.dpy, window->widget.win, &wmDeleteWin, 1);

    #ifdef CAKE_WINDOWS
    Cake_String_UTF16 class16, title16;
    cake_create_strutf16(&class16);
    cake_create_strutf16(&title16);
    cake_strutf8_to_utf16(window->className, &class16);
    cake_strutf8_to_utf16(window->title, &title16);

    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = class16.characteres;
    wc.hbrBackground = CreateSolidBrush(RGB(r, g, b));
    wc.lpfnWndProc = __cake_window_proc;
    wc.cbWndExtra = sizeof(&window->widget);

    RegisterClassExW(&wc);

    window->widget.handle = CreateWindowExW(
        extendedStyle,
        class16.characteres,
        title16.characteres,
        style,
        x, y,
        width, height,
        (window->widget.parent != NULL ? window->widget.parent->handle : NULL),
        window->menu,
        wc.hInstance, window
    );
    free(class16.characteres);
    free(title16.characteres);
    #endif

    return window;
}

void cake_window_exec(Cake_Window *window) {
    #ifdef CAKE_WINDOWS
    MSG msg;
    while(GetMessageW(&msg, window->widget.handle, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    #else
    XEvent event;
    int retCode;
    for(;;) {
        retCode = 0;
        XNextEvent(window->widget.dpy, &event);
        switch(event.type) {
            case DestroyNotify:
            case ClientMessage:{
                if(window->events.destroyEvent != NULL)
                    retCode = window->events.destroyEvent(window);
            } break;

            // Quand la fenêtre est redimenssionnée
            case ConfigureNotify:{
                if(
                    window->widget.width  != event.xconfigure.width ||
                    window->widget.height != event.xconfigure.height
                ) {
                    window->widget.width = event.xconfigure.width;
                    window->widget.height = event.xconfigure.height;
                    if(window->events.resizeEvent != NULL)
                        retCode = window->events.resizeEvent(&window->widget);
                }
                XWindowAttributes attr;
                Window child;
                int x, y;
                XTranslateCoordinates(window->widget.dpy, window->widget.win, window->widget.rootWin, 0, 0, &x, &y, &child);
                XGetWindowAttributes(window->widget.dpy, window->widget.rootWin, &attr);
                if(
                    window->widget.x != x ||
                    window->widget.y != y
                ) {
                    window->widget.x = x;
                    window->widget.y = y;
                    if(window->events.moveEvent != NULL)
                        retCode = window->events.moveEvent(&window->widget);
                }
            } break;
        }
        switch(retCode) {
            default: break;
            case -1:{
                XDestroyWindow(window->widget.dpy, window->widget.win);
            } goto end_loop;
        }
    }
    end_loop:
    #endif
}

/*
void cake_window_update_title(Cake_Window *window) {
    Cake_String_UTF16 name16;
    cake_create_strutf16(&name16);
    cake_strutf8_to_utf16(window->title, &name16);
    SetWindowTextW(window->widget.handle, name16.characteres);
    free(name16.characteres);
}

cake_bool cake_window_set_menu(Cake_Window *window, Cake_Window_Menu *menu) {
    DestroyMenu(window->menu);
    return SetMenu(window->widget.handle, menu);
}

Cake_OpenGL_RC cake_gl_attach(Cake_DC dc) {
    PIXELFORMATDESCRIPTOR pfd = { 
    sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
    1,                     // version number  
    PFD_DRAW_TO_WINDOW |   // support window  
    PFD_SUPPORT_OPENGL |   // support OpenGL  
    PFD_DOUBLEBUFFER,      // double buffered  
    PFD_TYPE_RGBA,         // RGBA type  
    24,                    // 24-bit color depth  
    0, 0, 0, 0, 0, 0,      // color bits ignored  
    0,                     // no alpha buffer  
    0,                     // shift bit ignored  
    0,                     // no accumulation buffer  
    0, 0, 0, 0,            // accum bits ignored  
    32,                    // 32-bit z-buffer      
    0,                     // no stencil buffer  
    0,                     // no auxiliary buffer  
    PFD_MAIN_PLANE,        // main layer  
    0,                     // reserved  
    0, 0, 0                // layer masks ignored  
    };

    int  iPixelFormat = ChoosePixelFormat(dc, &pfd);
    SetPixelFormat(dc, iPixelFormat, &pfd);
    return wglCreateContext(dc);

}
*/

Cake_List_Window *cake_list_window() {
    Cake_List_Window *windows = (Cake_List_Window *) malloc(sizeof(*windows));
    if(windows == NULL)
        return NULL;
    windows->list = NULL;
    windows->length = 0;
    return windows;
}

#ifdef CAKE_UNIX
void cake_window_show(Cake_Window *window) {
    XMapWindow(window->widget.dpy, window->widget.win);
    XFlush(window->widget.dpy);
}
#endif
