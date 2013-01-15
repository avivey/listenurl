#ifndef _TRAYICON_H
#define _TRAYICON_H

#include <tchar.h>
#include <windows.h>

#define HELP_ABOUT _T("Listens on TCP for URIs to open.\n\ngithub.com/avivey/listenurl")

#define THIS_CLASSNAME      _T("ListenUrl")
#define THIS_TITLE          _T("ListenUrl")


enum {
    ID_TRAYICON         = 1,

    APPWM_TRAYICON      = WM_APP,
    APPWM_NOP           = WM_APP + 1,

    //  Our commands
    ID_EXIT             = 2000,
    ID_ABOUT,
};

void (*app_close_listener)( HWND );
LRESULT (*WindowProc_fallback)( HWND, UINT, WPARAM, LPARAM );

void    RegisterApplicationClass( HINSTANCE hInstance );

#endif
