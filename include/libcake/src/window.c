#include "../window.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef CAKE_WINDOWS

struct cake_fusion_index_thread {
    ulonglong current;
    Cake_List_Window list;
};

DWORD __indexThreadWindow;

cake_bool cake_window_init() {
    __indexThreadWindow = TlsAlloc();
    if(__indexThreadWindow == TLS_OUT_OF_INDEXES)
        return cake_false;
    return cake_true;
}

void cake_window_cleanup() {
    TlsFree(__indexThreadWindow);
}

cake_bool cake_window_init_thread() {
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) malloc(sizeof(*fusion));
    if(fusion == NULL)
        return cake_false;
    fusion->current = (ulonglong) -1;
    fusion->list.list = NULL;
    fusion->list.length = 0;
    TlsSetValue(__indexThreadWindow, fusion);
    return cake_true;
}

void cake_window_cleanup_thread() {
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    if(fusion != NULL) {
        free(fusion->list.list);
        free(fusion);
    }
}

cake_bool cake_window_set_current(ulonglong value) {
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    if(fusion == NULL)
        return cake_false;
    fusion->current = value;
    return cake_true;
}

cake_bool cake_window_inc_current(ulonglong value) {
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    if(fusion == NULL)
        return cake_false;
    (fusion->current) += value;
    return cake_true;
}

cake_bool cake_windows_dec_current(ulonglong value) {
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
    if(fusion == NULL)
        return cake_false;
    (fusion->current) -= value;
    return cake_true;
}

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

Cake_Window *cake_window_windows(
    Cake_Window_Widget *parent,
    const char *className,
    const char *title,
    DWORD style,
    DWORD extendedStyle,
    int x, int y,
    int width, int height,
    uchar r, uchar g, uchar b,
    Cake_Window_Proc proc
) {
    struct cake_fusion_index_thread *fusion = (struct cake_fusion_index_thread *) TlsGetValue(__indexThreadWindow);
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
    window->exStyle = extendedStyle;
    window->style = style;
    window->proc = proc;
    window->menu = 0;

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

    return window;
}

void cake_window_exec(Cake_Window *window) {
    MSG msg;
    while(GetMessageW(&msg, window->widget.handle, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

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

#endif

Cake_List_Window *cake_list_window() {
    Cake_List_Window *windows = (Cake_List_Window *) malloc(sizeof(*windows));
    if(windows == NULL)
        return NULL;
    windows->list = NULL;
    windows->length = 0;
    return windows;
}


