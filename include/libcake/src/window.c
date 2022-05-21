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
    int retCode = 0;
    switch(msg) {
        default:
            return cake_window_proc_default(hwnd, msg, wparam, lparam);
        case CAKE_WINDOW_EVENT_RESIZE:{
            RECT size;
            GetWindowRect(hwnd, &size);
            fusion->list.list[fusion->current]->widget.width = size.right - size.left;
            fusion->list.list[fusion->current]->widget.height = size.bottom - size.top;
            if(fusion->list.list[fusion->current]->events.resizeEvent.callback != NULL)
                retCode = fusion->list.list[fusion->current]->events.resizeEvent.callback(fusion->list.list[fusion->current], fusion->list.list[fusion->current]->events.resizeEvent.args);
        } break;
        case CAKE_WINDOW_EVENT_MOVE:{
            RECT pos;
            GetWindowRect(hwnd, &pos);
            fusion->list.list[fusion->current]->widget.x = pos.left;
            fusion->list.list[fusion->current]->widget.y = pos.top;
            if(fusion->list.list[fusion->current]->events.moveEvent.callback != NULL)
                retCode = fusion->list.list[fusion->current]->events.moveEvent.callback(fusion->list.list[fusion->current], fusion->list.list[fusion->current]->events.moveEvent.args);
        } break;
        case CAKE_WINDOW_EVENT_KEYDOWN:{
            if(fusion->list.list[fusion->current]->events.keyPressedEvent.callback != NULL)
                retCode = fusion->list.list[fusion->current]->events.keyPressedEvent.callback(fusion->list.list[fusion->current], wparam, fusion->list.list[fusion->current]->events.keyPressedEvent.args);
        } break;
        case CAKE_WINDOW_EVENT_DESTROY:{
            if(fusion->list.list[fusion->current]->events.destroyEvent.callback != NULL)
                retCode = fusion->list.list[fusion->current]->events.destroyEvent.callback(fusion->list.list[fusion->current], fusion->list.list[fusion->current]->events.destroyEvent.args);
        } break;
    }
    switch(retCode) {
        default: return 0;
        case -1:{
            fusion->list.list[fusion->current]->active = cake_false;
            PostQuitMessage(0);
            return 0;
        }
    }
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

    XSelectInput(window->widget.dpy, window->widget.win, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask);
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

    window->className = cake_strutf8(className);
    window->title     = cake_strutf8(title);

    if(events != NULL) {
        window->events.moveEvent    = events->moveEvent;
        window->events.resizeEvent  = events->resizeEvent;
        window->events.destroyEvent = events->destroyEvent;
        window->events.keyPressedEvent  = events->keyPressedEvent;
        window->events.keyReleasedEvent = events->keyReleasedEvent;
    }else {
        window->events.moveEvent.callback    = NULL;
        window->events.resizeEvent.callback  = NULL;
        window->events.destroyEvent.callback = NULL;
        window->events.keyPressedEvent.callback  = NULL;
        window->events.keyReleasedEvent.callback = NULL;
    }

    #ifdef CAKE_UNIX
    XClassHint winHints;
    winHints.res_class = (char *) window->className->bytes;
    winHints.res_name  = (char *) window->className->bytes;
    XSetClassHint(window->widget.dpy, window->widget.win, &winHints);
    XStoreName(window->widget.dpy, window->widget.win, (cchar_ptr) window->title->bytes);

    Atom wmDeleteWin = XInternAtom(window->widget.dpy, "WM_DELETE_WINDOW", 1);
    XSetWMProtocols(window->widget.dpy, window->widget.win, &wmDeleteWin, 1);
    XStoreName(window->widget.dpy, window->widget.win, (cchar_ptr) window->title->bytes);
    #elif CAKE_WINDOWS
    window->active = cake_true;
    Cake_String_UTF16 class16, title16;
    cake_create_strutf16(&class16);
    cake_create_strutf16(&title16);
    cake_strutf8_to_utf16(window->className, &class16);
    cake_strutf8_to_utf16(window->title, &title16);

    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = class16.characteres;
    wc.hbrBackground = CreateSolidBrush(RGB(r, g, b));
    wc.lpfnWndProc = __cake_window_proc;

    RegisterClassExW(&wc);

    window->widget.handle = CreateWindowExW(
        0,
        class16.characteres,
        title16.characteres,
        CAKE_WINDOW_STYLE_OVERLAPPEDWINDOW,
        x, y,
        width, height,
        (window->widget.parent != NULL ? window->widget.parent->handle : NULL),
        NULL,
        wc.hInstance,
        NULL
    );
    window->hdc = NULL;
    free(class16.characteres);
    free(title16.characteres);
    #endif
    return window;
}

cake_bool cake_window_poll_events(Cake_Window *window) {
    #ifdef CAKE_WINDOWS
    MSG msg;
    if(PeekMessageW(&msg, window->widget.handle, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return window->active;
    #else
    if(XPending(window->widget.dpy) == 0)
        return cake_true;
    XEvent event;
    int retCode = 0;
    XNextEvent(window->widget.dpy, &event);
    switch(event.type) {
        case KeyPress:{
            if(window->events.keyPressedEvent.callback != NULL)
                retCode = window->events.keyPressedEvent.callback(window, XLookupKeysym(&event.xkey, 0), window->events.keyPressedEvent.args);
        } break;
        case KeyRelease:{
            if(window->events.keyReleasedEvent.callback != NULL)
                retCode = window->events.keyReleasedEvent.callback(window, XLookupKeysym(&event.xkey, 0), window->events.keyReleasedEvent.args);
        } break;
        case DestroyNotify:
        case ClientMessage:{
            if(window->events.destroyEvent.callback != NULL)
                retCode = window->events.destroyEvent.callback(window, window->events.destroyEvent.args);
        } break;

        // Quand la fenêtre est redimenssionnée ou déplacée
        case ConfigureNotify:{
            if(
                window->widget.width  != event.xconfigure.width ||
                window->widget.height != event.xconfigure.height
            ) {
                window->widget.width = event.xconfigure.width;
                window->widget.height = event.xconfigure.height;
                if(window->events.resizeEvent.callback != NULL)
                    retCode = window->events.resizeEvent.callback(window, window->events.resizeEvent.args);
            }
            XWindowAttributes attr;
            Window child;
            int x, y;
            XTranslateCoordinates(window->widget.dpy, window->widget.win, window->widget.rootWin, 0, 0, &x, &y, &child);
            XGetWindowAttributes(window->widget.dpy, window->widget.win, &attr);
            x -= attr.x;
            y -= attr.y;
            if(
                window->widget.x != x ||
                window->widget.y != y
            ) {
                window->widget.x = x;
                window->widget.y = y;
                //printf("x: %d y: %d\n", window->widget.x, window->widget.y);
                if(window->events.moveEvent.callback != NULL)
                    retCode = window->events.moveEvent.callback(window, window->events.moveEvent.args);
            }
        } break;
    }
    switch(retCode) {
        default: return cake_true;
        case -1:{
            XDestroyWindow(window->widget.dpy, window->widget.win);
        } return cake_false;
    }
    #endif
}


void cake_window_update_title(Cake_Window *window) {
    #ifdef CAKE_WINDOWS
    Cake_String_UTF16 name16;
    cake_create_strutf16(&name16);
    cake_strutf8_to_utf16(window->title, &name16);
    SetWindowTextW(window->widget.handle, name16.characteres);
    free(name16.characteres);
    #else

    #endif
}

/*

cake_bool cake_window_set_menu(Cake_Window *window, Cake_Window_Menu *menu) {
    DestroyMenu(window->menu);
    return SetMenu(window->widget.handle, menu);
}

*/

Cake_OpenGL_RC cake_gl_attach(Cake_Window *window) {
    #ifdef CAKE_WINDOWS
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

    window->hdc = GetDC(window->widget.handle);
    int  iPixelFormat = ChoosePixelFormat(window->hdc, &pfd);
    SetPixelFormat(window->hdc, iPixelFormat, &pfd);
    Cake_OpenGL_RC rc = wglCreateContext(window->hdc);
    return rc;
    #else
    int attr[] = {
      GLX_X_RENDERABLE    , cake_true,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 24,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , cake_true,
      0
    };

    int fbcount;
    GLXFBConfig *fbc = glXChooseFBConfig(window->widget.dpy, window->widget.screen, attr, &fbcount);
    if(fbc == NULL)
        return NULL;
    
    int i, bestFbc = -1, bestSamples = -1;
    for(i = 0; i < fbcount; ++i) {
        XVisualInfo *viInfo = glXGetVisualFromFBConfig(window->widget.dpy, fbc[i]);
        if(viInfo != NULL) {
            int sampBuff, samples;
            glXGetFBConfigAttrib(window->widget.dpy, fbc[i], GLX_SAMPLE_BUFFERS, &sampBuff);
            glXGetFBConfigAttrib(window->widget.dpy, fbc[i], GLX_SAMPLES, &samples);
            if((bestFbc < 0) || (sampBuff && (samples > bestSamples))) {
                bestFbc = i;
                bestSamples = samples;
            }
        }
    }
    GLXFBConfig myFbc = fbc[bestFbc];
    XFree(fbc);
    XVisualInfo *bestViInfo = glXGetVisualFromFBConfig(window->widget.dpy, myFbc);
    GLXContext glContext = glXCreateContext(window->widget.dpy, bestViInfo, NULL, cake_true);
    XFree(bestViInfo);
    return glContext;
    #endif
}


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

/*
void cake_window_move(Cake_Window *window, int x, int y) {
    XMoveWindow(window->widget.dpy, window->widget.win, x, y);
    XFlush(window->widget.dpy);
}
*/
#endif

void cake_free_window(Cake_Window *window) {
    cake_free_strutf8(window->className);
    cake_free_strutf8(window->title);
    #ifdef CAKE_WINDOWS
    ReleaseDC(window->widget.handle, window->hdc);
    #else
    XCloseDisplay(window->widget.dpy);
    #endif
    free(window);
}
