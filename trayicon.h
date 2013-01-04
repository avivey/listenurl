#ifndef _TRAYICON_H
#define _TRAYICON_H

#include <windows.h>

#define HELP_ABOUT "listenurl: Listen on TCP for a url, then open it."

#define THIS_CLASSNAME      "ListenUrl"
#define THIS_TITLE          "About: ListenUrl"


enum {
    ID_TRAYICON         = 1,

    APPWM_TRAYICON      = WM_APP,
    APPWM_NOP           = WM_APP + 1,

    //  Our commands
    ID_EXIT             = 2000,
    ID_ABOUT,
};


void    AddTrayIcon( HWND hWnd, UINT uID, UINT uCallbackMsg, UINT uIcon,
                     LPSTR pszToolTip );
void    RemoveTrayIcon( HWND hWnd, UINT uID);
void    ModifyTrayIcon( HWND hWnd, UINT uID, UINT uIcon, LPSTR pszToolTip );

HICON   LoadSmallIcon( HINSTANCE hInstance, UINT uID );

BOOL    ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem );
void    OnInitMenuPopup( HWND hWnd, HMENU hMenu, UINT uID );

BOOL    OnCommand( HWND hWnd, WORD wID, HWND hCtl );

void    OnTrayIconMouseMove( HWND hWnd );
void    OnTrayIconRBtnUp( HWND hWnd );
void    OnTrayIconLBtnDblClick( HWND hWnd );

void    OnClose( HWND hWnd );

void    RegisterMainWndClass( HINSTANCE hInstance );
void    UnregisterMainWndClass( HINSTANCE hInstance );

#endif
