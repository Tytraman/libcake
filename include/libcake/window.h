#ifndef __CAKE_WINDOW_H__
#define __CAKE_WINDOW_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "def.h"
#include "strutf16.h"
#include "strutf8.h"

#ifdef CAKE_WINDOWS
#include <windows.h>

#else
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#endif



#define CAKE_WINDOW_WIDGET_WINDOW  0
#define CAKE_WINDOW_WIDGET_MENU    1
#define CAKE_WINDOW_WIDGET_TEXT    2
#define CAKE_WINDOW_WIDGET_EDITBOX 3

typedef struct cake_window Cake_Window;

// Structure commune à tous les widgets
typedef struct cake_window_widget {
    struct cake_window_widget *parent;

    #ifdef CAKE_WINDOWS
    HWND handle;
    #else
    Display *dpy;
    int screen;
    Window rootWin;
    Window win;
    #endif
    uchar type;

    int x, y;
    int width, height;
} Cake_Window_Widget;

#ifdef CAKE_WINDOWS
typedef WPARAM cake_keycode;
#else
typedef KeySym cake_keycode;
#endif

typedef struct cake_window_move_event {
    int (*callback)(Cake_Window *window, void *args);
    void *args;
} Cake_Window_Move_Event;

typedef struct cake_window_resize_event {
    int (*callback)(Cake_Window *window, void *args);
    void *args;
} Cake_Window_Resize_Event;

typedef struct cake_window_destroy_event {
    int (*callback)(Cake_Window *window, void *args);
    void *args;
} Cake_Window_Destroy_Event;

typedef struct cake_window_keypressed_event {
    int (*callback)(Cake_Window *window, cake_keycode key, void *args);
    void *args;
} Cake_Window_KeyPressed_Event;

typedef struct cake_window_keyreleased_event {
    int (*callback)(Cake_Window *window, cake_keycode key, void *args);
    void *args;
} Cake_Window_KeyReleased_Event;

typedef struct cake_window_events {
    Cake_Window_Move_Event moveEvent;
    Cake_Window_Resize_Event resizeEvent;
    Cake_Window_Destroy_Event destroyEvent;
    Cake_Window_KeyPressed_Event keyPressedEvent;
    Cake_Window_KeyReleased_Event keyReleasedEvent;
} Cake_Window_Events;

typedef struct cake_list_window_widget {
    Cake_Window_Widget **list;
    ulonglong length;
} Cake_List_Window_Widget;

struct cake_window {
    Cake_Window_Widget widget;

    Cake_String_UTF8 *className;
    Cake_String_UTF8 *title;

    Cake_Window_Events events;
    #ifdef CAKE_WINDOWS
    cake_bool active;
    HDC hdc;
    #endif
};

typedef struct cake_list_window {
    Cake_Window **list;
    ulonglong length;
} Cake_List_Window;

Cake_List_Window *cake_list_window();

#ifdef CAKE_WINDOWS

#define CAKE_WINDOW_STYLE_EX_ACCEPTFILES         WS_EX_ACCEPTFILES
#define CAKE_WINDOW_STYLE_EX_APPWINDOW           WS_EX_APPWINDOW
#define CAKE_WINDOW_STYLE_EX_CLIENTEDGE          WS_EX_CLIENTEDGE
#define CAKE_WINDOW_STYLE_EX_COMPOSITED          WS_EX_COMPOSITED
#define CAKE_WINDOW_STYLE_EX_CONTEXTHELP         WS_EX_CONTEXTHELP
#define CAKE_WINDOW_STYLE_EX_CONTROLPARENT       WS_EX_CONTROLPARENT
#define CAKE_WINDOW_STYLE_EX_DLGMODALFRAME       WS_EX_DLGMODALFRAME
#define CAKE_WINDOW_STYLE_EX_LAYERED             WS_EX_LAYERED
#define CAKE_WINDOW_STYLE_EX_LAYOUTRTL           WS_EX_LAYOUTRTL
#define CAKE_WINDOW_STYLE_EX_LEFT                WS_EX_LEFT
#define CAKE_WINDOW_STYLE_EX_LEFTSCROLLBAR       WS_EX_LEFTSCROLLBAR
#define CAKE_WINDOW_STYLE_EX_LTRREADING          WS_EX_LTRREADING
#define CAKE_WINDOW_STYLE_EX_MDICHILD            WS_EX_MDICHILD
#define CAKE_WINDOW_STYLE_EX_NOACTIVATE          WS_EX_NOACTIVATE
#define CAKE_WINDOW_STYLE_EX_NOINHERITLAYOUT     WS_EX_NOINHERITLAYOUT
#define CAKE_WINDOW_STYLE_EX_NOPARENTNOTIFY      WS_EX_NOPARENTNOTIFY
#define CAKE_WINDOW_STYLE_EX_NOREDIRECTIONBITMAP WS_EX_NOREDIRECTIONBITMAP
#define CAKE_WINDOW_STYLE_EX_OVERLAPPEDWINDOW    WS_EX_OVERLAPPEDWINDOW
#define CAKE_WINDOW_STYLE_EX_PALETTEWINDOW       WS_EX_PALETTEWINDOW
#define CAKE_WINDOW_STYLE_EX_RIGHT               WS_EX_RIGHT
#define CAKE_WINDOW_STYLE_EX_RIGHTSCROLLBAR      WS_EX_RIGHTSCROLLBAR
#define CAKE_WINDOW_STYLE_EX_RTLREADING          WS_EX_RTLREADING
#define CAKE_WINDOW_STYLE_EX_STATICEDGE          WS_EX_STATICEDGE
#define CAKE_WINDOW_STYLE_EX_TOOLWINDOW          WS_EX_TOOLWINDOW
#define CAKE_WINDOW_STYLE_EX_TOPMOST             WS_EX_TOPMOST
#define CAKE_WINDOW_STYLE_EX_TRANSPARENT         WS_EX_TRANSPARENT
#define CAKE_WINDOW_STYLE_EX_WINDOWEDGE          WS_EX_WINDOWEDGE

#define CAKE_WINDOW_STYLE_BORDER           WS_BORDER
#define CAKE_WINDOW_STYLE_CAPTION          WS_CAPTION
#define CAKE_WINDOW_STYLE_CHILD            WS_CHILD
#define CAKE_WINDOW_STYLE_CHILDWINDOW      WS_CHILDWINDOW
#define CAKE_WINDOW_STYLE_CLIPCHILDREN     WS_CLIPCHILDREN
#define CAKE_WINDOW_STYLE_CLIPSIBLINGS     WS_CLIPSIBLINGS
#define CAKE_WINDOW_STYLE_DISABLED         WS_DISABLED
#define CAKE_WINDOW_STYLE_DLGFRAME         WS_DLGFRAME
#define CAKE_WINDOW_STYLE_GROUP            WS_GROUP
#define CAKE_WINDOW_STYLE_HSCROLL          WS_HSCROLL
#define CAKE_WINDOW_STYLE_ICONIC           WS_ICONIC
#define CAKE_WINDOW_STYLE_MAXIMIZE         WS_MAXIMIZE
#define CAKE_WINDOW_STYLE_MAXIMIZEBOX      WS_MAXIMIZEBOX
#define CAKE_WINDOW_STYLE_MINIMIZE         WS_MINIMIZE
#define CAKE_WINDOW_STYLE_MINIMIZEBOX      WS_MINIMIZEBOX
#define CAKE_WINDOW_STYLE_OVERLAPPED       WS_OVERLAPPED
#define CAKE_WINDOW_STYLE_OVERLAPPEDWINDOW WS_OVERLAPPEDWINDOW
#define CAKE_WINDOW_STYLE_POPUP            WS_POPUP
#define CAKE_WINDOW_STYLE_POPUPWINDOW      WS_POPUPWINDOW
#define CAKE_WINDOW_STYLE_SIZEBOX          WS_SIZEBOX
#define CAKE_WINDOW_STYLE_SYSMENU          WS_SYSMENU
#define CAKE_WINDOW_STYLE_TABSTOP          WS_TABSTOP
#define CAKE_WINDOW_STYLE_THICKFRAME       WS_THICKFRAME
#define CAKE_WINDOW_STYLE_TILED            WS_TILED
#define CAKE_WINDOW_STYLE_TILEDWINDOW      WS_TILEDWINDOW
#define CAKE_WINDOW_STYLE_VISIBLE          WS_VISIBLE
#define CAKE_WINDOW_STYLE_VSCROLL          WS_VSCROLL

#define CAKE_WINDOW_MENU_FLAG_BITMAP       MF_BITMAP
#define CAKE_WINDOW_MENU_FLAG_CHECKED      MF_CHECKED
#define CAKE_WINDOW_MENU_FLAG_DISABLED     MF_DISABLED
#define CAKE_WINDOW_MENU_FLAG_ENABLED      MF_ENABLED
#define CAKE_WINDOW_MENU_FLAG_GRAYED       MF_GRAYED
#define CAKE_WINDOW_MENU_FLAG_MENUBARBREAK MF_MENUBARBREAK
#define CAKE_WINDOW_MENU_FLAG_MENUBREAK    MF_MENUBREAK
#define CAKE_WINDOW_MENU_FLAG_OWNERDRAW    MF_OWNERDRAW
#define CAKE_WINDOW_MENU_FLAG_POPUP        MF_POPUP
#define CAKE_WINDOW_MENU_FLAG_SEPARATOR    MF_SEPARATOR
#define CAKE_WINDOW_MENU_FLAG_STRING       MF_STRING
#define CAKE_WINDOW_MENU_FLAG_UNCHECKED    MF_UNCHECKED

#define CAKE_VK_LBUTTON    VK_LBUTTON
#define CAKE_VK_RBUTTON    VK_RBUTTON
#define CAKE_VK_CANCEL     VK_CANCEL
#define CAKE_VK_MBUTTON    VK_MBUTTON
#define CAKE_VK_XBUTTON1   VK_XBUTTON1
#define CAKE_VK_XBUTTON2   VK_XBUTTON2
#define CAKE_VK_BACK       VK_BACK
#define CAKE_VK_TAB        VK_TAB
#define CAKE_VK_CLEAR      VK_CLEAR
#define CAKE_VK_RETURN     VK_RETURN
#define CAKE_VK_SHIFT      VK_SHIFT
#define CAKE_VK_CONTROL    VK_CONTROL
#define CAKE_VK_MENU       VK_MENU
#define CAKE_VK_PAUSE      VK_PAUSE
#define CAKE_VK_CAPITAL    VK_CAPITAL
#define CAKE_VK_ESC        VK_ESCAPE
#define CAKE_VK_SPACE      VK_SPACE
#define CAKE_VK_PAGEUP     VK_PRIOR
#define CAKE_VK_PAGEDOWN   VK_NEXT
#define CAKE_VK_END        VK_END
#define CAKE_VK_HOME       VK_HOME
#define CAKE_VK_LEFT       VK_LEFT
#define CAKE_VK_UP         VK_UP
#define CAKE_VK_RIGHT      VK_RIGHT
#define CAKE_VK_DOWN       VK_DOWN
#define CAKE_VK_SELECT     VK_SELECT
#define CAKE_VK_PRINT      VK_PRINT
#define CAKE_VK_EXECUTE    VK_EXECUTE
#define CAKE_VK_SCREENSHOT VK_SNAPSHOT
#define CAKE_VK_INSERT     VK_INSERT
#define CAKE_VK_DELETE     VK_DELETE
#define CAKE_VK_HELP       VK_HELP
#define CAKE_VK_NUMPAD0    VK_NUMPAD0
#define CAKE_VK_NUMPAD1    VK_NUMPAD1
#define CAKE_VK_NUMPAD2    VK_NUMPAD2
#define CAKE_VK_NUMPAD3    VK_NUMPAD3
#define CAKE_VK_NUMPAD4    VK_NUMPAD4
#define CAKE_VK_NUMPAD5    VK_NUMPAD5
#define CAKE_VK_NUMPAD6    VK_NUMPAD6
#define CAKE_VK_NUMPAD7    VK_NUMPAD7
#define CAKE_VK_NUMPAD8    VK_NUMPAD8
#define CAKE_VK_NUMPAD9    VK_NUMPAD9
#define CAKE_VK_MULTIPLY   VK_MULTIPLY
#define CAKE_VK_ADD        VK_ADD
#define CAKE_VK_SEPARATOR  VK_SEPARATOR
#define CAKE_VK_SUBTRACT   VK_SUBTRACT
#define CAKE_VK_DECIMAL    VK_DECIMAL
#define CAKE_VK_DIVIDE     VK_DIVIDE
#define CAKE_VK_F1         VK_F1
#define CAKE_VK_F2         VK_F2
#define CAKE_VK_F3         VK_F3
#define CAKE_VK_F4         VK_F4
#define CAKE_VK_F5         VK_F5
#define CAKE_VK_F6         VK_F6
#define CAKE_VK_F7         VK_F7
#define CAKE_VK_F8         VK_F8
#define CAKE_VK_F9         VK_F9
#define CAKE_VK_F10        VK_F10
#define CAKE_VK_F11        VK_F11
#define CAKE_VK_F12        VK_F12
#define CAKE_VK_F13        VK_F13
#define CAKE_VK_F14        VK_F14
#define CAKE_VK_F15        VK_F15
#define CAKE_VK_F16        VK_F16
#define CAKE_VK_F17        VK_F17
#define CAKE_VK_F18        VK_F18
#define CAKE_VK_F19        VK_F19
#define CAKE_VK_F20        VK_F20
#define CAKE_VK_F21        VK_F21
#define CAKE_VK_F22        VK_F22
#define CAKE_VK_F23        VK_F23
#define CAKE_VK_F24        VK_F24
#define CAKE_VK_NUMLOCK    VK_NUMLOCK
#define CAKE_VK_SCROLLLOCK VK_SCROLL
#define CAKE_VK_LSHIFT     VK_LSHIFT
#define CAKE_VK_RSHIFT     VK_RSHIFT
#define CAKE_VK_LCONTROL   VK_LCONTROL
#define CAKE_VK_RCONTROL   VK_RCONTROL

#define CAKE_WINDOW_EVENT_RESIZE  WM_SIZE
#define CAKE_WINDOW_EVENT_KEYDOWN WM_KEYDOWN
#define CAKE_WINDOW_EVENT_MOVE    WM_MOVE
#define CAKE_WINDOW_EVENT_CLOSE   WM_CLOSE
#define CAKE_WINDOW_EVENT_DESTROY WM_DESTROY
#define CAKE_WINDOW_EVENT_COMMAND WM_COMMAND

typedef HWND cake_window_handle;
typedef LRESULT cake_window_res;
typedef WPARAM cake_wparam;
typedef LPARAM cake_lparam;
typedef UINT_PTR uint_ptr;
typedef MSG Cake_Window_Event;

typedef HDC Cake_DC;
#define cake_get_dc(__handle) GetDC(__handle)
#define cake_release_dc(__handle, __dc) ReleaseDC(__handle, __dc)

typedef HGLRC Cake_OpenGL_RC;

typedef struct cake_window Cake_Window;
typedef cake_window_res (*Cake_Window_Proc)(cake_window_handle handle, uint msg, cake_wparam wparam, cake_lparam lparam, Cake_Window *window);

// Menu qui apparaît au dessus du contenu de la fenêtre
typedef struct HMENU__ Cake_Window_Menu;
#define cake_window_menu() CreateMenu()
#define cake_window_menu_append(__menu, __flags, __id, __cakeCharValue) AppendMenuW(__menu, __flags, __id, __cakeCharValue)

#define cake_destroy_window(__handle) DestroyWindow(__handle)

#define cake_window_first_show(__window) ShowWindow((*__window).widget.handle, SW_SHOWNORMAL)
#define cake_window_show(__window) ShowWindow((*__window).widget.handle, SW_SHOW)
#define cake_window_hide(__window) ShowWindow((*__window).widget.handle, SW_HIDE)
#define cake_window_show_minimized(__window) ShowWindow((*__window).widget.handle, SW_SHOWMINIMIZED)
#define cake_window_show_maximized(__window) ShowWindow((*__window).widget.handle, SW_SHOWMAXIMIZED)
#define cake_window_minimize(__window) ShowWindow((*__window).widget.handle, SW_MINIMIZE);

#define cake_window_post_quit_event(__retCode) PostQuitMessage(__retCode)

#define cake_window_proc_default(hwnd, msg, wparam, lparam) DefWindowProcW(hwnd, msg, wparam, lparam)

#define cake_window_move(__window, __x, __y) MoveWindow((__window).widget.handle, __x, __y, (__window).widget.width, (__window).widget.height, cake_true)

cake_bool cake_window_set_menu(Cake_Window *window, Cake_Window_Menu *menu);

#define cake_gl_make_current(__window, __gl_rc) wglMakeCurrent((__window).hdc, __gl_rc)
#define cake_gl_delete_context(__window, __gl_rc) wglDeleteContext(__gl_rc)
#define cake_swap_buffers(__window) SwapBuffers((__window).hdc)

void cake_window_cleanup();
#else

#define CAKE_VK_BACK     XK_BackSpace
#define CAKE_VK_TAB      XK_Tab
#define CAKE_VK_CLEAR    XK_Clear
#define CAKE_VK_RETURN   XK_Return
#define CAKE_VK_LSHIFT   XK_Shift_L
#define CAKE_VK_RSHIFT   XK_Shift_R
#define CAKE_VK_SHIFT    CAKE_VK_LSHIFT
#define CAKE_VK_LCONTROL XK_Control_L
#define CAKE_VK_RCONTROL XK_Control_R
#define CAKE_VK_CONTROL  CAKE_VK_LCONTROL
#define CAKE_VK_MENU     XK_Menu
#define CAKE_VK_PAUSE    XK_Pause
#define CAKE_VK_CAPITAL  XK_Caps_Lock
#define CAKE_VK_ESC      XK_Escape
#define CAKE_VK_SPACE    XK_space
#define CAKE_VK_PAGEUP   XK_Page_Up
#define CAKE_VK_PAGEDOWN XK_Page_Down
#define CAKE_VK_END      XK_End
#define CAKE_VK_HOME     XK_Home
#define CAKE_VK_LEFT     XK_Left
#define CAKE_VK_UP       XK_Up
#define CAKE_VK_RIGHT    XK_Right
#define CAKE_VK_DOWN     XK_Down
#define CAKE_VK_SELECT   XK_Select
#define CAKE_VK_PRINT    XK_Print
#define CAKE_VK_EXECUTE  XK_Execute
#define CAKE_VK_INSERT   XK_Insert

#define CAKE_VK_F1 XK_F1
#define CAKE_VK_F2 XK_F2
#define CAKE_VK_F3 XK_F3
#define CAKE_VK_F4 XK_F4
#define CAKE_VK_F5 XK_F5
#define CAKE_VK_F6 XK_F6
#define CAKE_VK_F7 XK_F7
#define CAKE_VK_F8 XK_F8
#define CAKE_VK_F9 XK_F9
#define CAKE_VK_F10 XK_F10
#define CAKE_VK_F11 XK_F11
#define CAKE_VK_F12 XK_F12
#define CAKE_VK_F13 XK_F13
#define CAKE_VK_F14 XK_F14
#define CAKE_VK_F15 XK_F15
#define CAKE_VK_F16 XK_F16
#define CAKE_VK_F17 XK_F17
#define CAKE_VK_F18 XK_F18
#define CAKE_VK_F19 XK_F19
#define CAKE_VK_F20 XK_F20
#define CAKE_VK_F21 XK_F21
#define CAKE_VK_F22 XK_F22
#define CAKE_VK_F23 XK_F23
#define CAKE_VK_F24 XK_F24printf("%d FBConfigs.\n", fbcount);

#define CAKE_VK_NUMLOCK XK_Num_Lock

#define CAKE_VK_A XK_A
#define CAKE_VK_B XK_B
#define CAKE_VK_C XK_C
#define CAKE_VK_D XK_D
#define CAKE_VK_E XK_E
#define CAKE_VK_F XK_F
#define CAKE_VK_G XK_G
#define CAKE_VK_H XK_H
#define CAKE_VK_I XK_I
#define CAKE_VK_J XK_J
#define CAKE_VK_K XK_K
#define CAKE_VK_L XK_L
#define CAKE_VK_M XK_M
#define CAKE_VK_N XK_N
#define CAKE_VK_O XK_O
#define CAKE_VK_P XK_P
#define CAKE_VK_Q XK_Q
#define CAKE_VK_R XK_R
#define CAKE_VK_S XK_S
#define CAKE_VK_T XK_T
#define CAKE_VK_U XK_U
#define CAKE_VK_V XK_V
#define CAKE_VK_W XK_W
#define CAKE_VK_X XK_X
#define CAKE_VK_Y XK_Y
#define CAKE_VK_Z XK_Z

#define CAKE_VK_a XK_a
#define CAKE_VK_b XK_b
#define CAKE_VK_c XK_c
#define CAKE_VK_d XK_d
#define CAKE_VK_e XK_e
#define CAKE_VK_f XK_f
#define CAKE_VK_g XK_g
#define CAKE_VK_h XK_h
#define CAKE_VK_i XK_i
#define CAKE_VK_j XK_j
#define CAKE_VK_k XK_k
#define CAKE_VK_l XK_l
#define CAKE_VK_m XK_m
#define CAKE_VK_n XK_n
#define CAKE_VK_o XK_o
#define CAKE_VK_p XK_p
#define CAKE_VK_q XK_q
#define CAKE_VK_r XK_r
#define CAKE_VK_s XK_s
#define CAKE_VK_t XK_t
#define CAKE_VK_u XK_u
#define CAKE_VK_v XK_v
#define CAKE_VK_w XK_w
#define CAKE_VK_x XK_x
#define CAKE_VK_y XK_y
#define CAKE_VK_z XK_z

#define CAKE_VK_0 XK_0
#define CAKE_VK_1 XK_1
#define CAKE_VK_2 XK_2
#define CAKE_VK_3 XK_3
#define CAKE_VK_4 XK_4
#define CAKE_VK_5 XK_5
#define CAKE_VK_6 XK_6
#define CAKE_VK_7 XK_7
#define CAKE_VK_8 XK_8
#define CAKE_VK_9 XK_9

typedef GLXContext Cake_OpenGL_RC;

#define cake_window_move(__window, __x, __y) XMoveWindow((*__window).widget.dpy, (*__window).widget.win, __x, __y)

void cake_window_show(Cake_Window *window);

#define cake_window_first_show(__window) cake_window_show(__window)

#define cake_window_cleanup()

#define cake_gl_make_current(__window, __gl_rc) glXMakeCurrent((*__window).widget.dpy, (*__window).widget.win, __gl_rc)
#define cake_gl_delete_context(__window, __gl_rc) glXDestroyContext((*__window).widget.dpy, __gl_rc)
#define cake_swap_buffers(__window) glXSwapBuffers((*__window).widget.dpy, (*__window).widget.win)

#endif

cake_bool cake_window_init();

cake_bool cake_window_init_thread();
void cake_window_cleanup_thread();
cake_bool cake_window_set_current(ulonglong value);
cake_bool cake_window_inc_current(ulonglong value);
cake_bool cake_windows_dec_current(ulonglong value);

void cake_window_update_title(Cake_Window *window);

Cake_Window *cake_window(
    Cake_Window_Widget *parent,
    const char *className,
    const char *title,
    int x, int y,
    int width, int height,
    uchar r, uchar g, uchar b,
    Cake_Window_Events *events
);

cake_bool cake_window_poll_events(Cake_Window *window);

Cake_OpenGL_RC cake_gl_attach(Cake_Window *window);

void cake_free_window(Cake_Window *window);


#ifdef __cplusplus
}
#endif

#endif