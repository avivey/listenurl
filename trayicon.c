//-----------------------------------------------------------------------------
//  trayicon.c
//
//  Including most ui.
//  Based on trayicon.c by wharfinger 8/05/2001
//-----------------------------------------------------------------------------

#include "trayicon.h"

static BOOL g_bModalState       = FALSE;

//-----------------------------------------------------------------------------
//  Add an icon to the system tray.
void AddTrayIcon( HWND hWnd, UINT uID, UINT uCallbackMsg, UINT uIcon,
                  LPSTR pszToolTip )
{
    NOTIFYICONDATA  nid;

    memset( &nid, 0, sizeof( nid ) );

    nid.cbSize              = sizeof( nid );
    nid.hWnd                = hWnd;
    nid.uID                 = uID;
    nid.uFlags              = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage    = uCallbackMsg;
    //  Uncomment this if you've got your own icon.  GetModuleHandle( NULL )
    //  gives us our HINSTANCE.  I hate globals.
//  nid.hIcon               = LoadSmallIcon( GetModuleHandle( NULL ), uIcon );

    //  Comment this if you've got your own icon.
    {
    char    szIconFile[512];

    GetSystemDirectory( szIconFile, sizeof( szIconFile ) );
    if ( szIconFile[ strlen( szIconFile ) - 1 ] != '\\' )
        strcat( szIconFile, "\\" );
    strcat( szIconFile, "shell32.dll" );
    //  Icon #23 (0-indexed) in shell32.dll is a "help" icon.
    ExtractIconEx( szIconFile, 23, NULL, &(nid.hIcon), 1 );
    }

    strcpy( nid.szTip, pszToolTip );

    Shell_NotifyIcon( NIM_ADD, &nid );
}


//-----------------------------------------------------------------------------
void ModifyTrayIcon( HWND hWnd, UINT uID, UINT uIcon, LPSTR pszToolTip )
{
    NOTIFYICONDATA  nid;

    memset( &nid, 0, sizeof( nid ) );

    nid.cbSize  = sizeof( nid );
    nid.hWnd    = hWnd;
    nid.uID     = uID;

    if ( uIcon != (UINT)-1 ) {
        nid.hIcon   = LoadSmallIcon( GetModuleHandle( NULL ), uIcon );
        nid.uFlags  |= NIF_ICON;
    }

    if ( pszToolTip ) {
        strcpy( nid.szTip, pszToolTip );
        nid.uFlags  |= NIF_TIP;
    }

    if ( uIcon != (UINT)-1 || pszToolTip )
        Shell_NotifyIcon( NIM_MODIFY, &nid );
}


//-----------------------------------------------------------------------------
//  Remove an icon from the system tray.
void RemoveTrayIcon( HWND hWnd, UINT uID )
{
    NOTIFYICONDATA  nid;

    memset( &nid, 0, sizeof( nid ) );

    nid.cbSize  = sizeof( nid );
    nid.hWnd    = hWnd;
    nid.uID     = uID;

    Shell_NotifyIcon( NIM_DELETE, &nid );
}


//-----------------------------------------------------------------------------
//  OUR NERVE CENTER.
static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch ( uMsg ) {
        case WM_CREATE:
            AddTrayIcon( hWnd, ID_TRAYICON, APPWM_TRAYICON, 0, THIS_TITLE );
            return 0;

        case APPWM_NOP:
            //  There's a long comment in OnTrayIconRBtnUp() which explains
            //  what we're doing here.
            return 0;

        //  This is the message which brings tidings of mouse events involving
        //  our tray icon.  We defined it ourselves.  See AddTrayIcon() for
        //  details of how we told Windows about it.
        case APPWM_TRAYICON:
            SetForegroundWindow( hWnd );

            switch ( lParam ) {
                case WM_MOUSEMOVE:
                    OnTrayIconMouseMove( hWnd );
                    return 0;

                case WM_RBUTTONUP:
                    //  There's a long comment in OnTrayIconRBtnUp() which
                    //  explains what we're doing here.
                    OnTrayIconRBtnUp( hWnd );
                    return 0;

                case WM_LBUTTONDBLCLK:
                    OnTrayIconLBtnDblClick( hWnd );
                    return 0;
            }
            return 0;

        case WM_COMMAND:
            return OnCommand( hWnd, LOWORD(wParam), (HWND)lParam );

        case WM_INITMENUPOPUP:
            OnInitMenuPopup( hWnd, (HMENU)wParam, lParam );
            return 0;

        case WM_CLOSE:
            OnClose( hWnd );
            return DefWindowProc( hWnd, uMsg, wParam, lParam );

        default:
            return DefWindowProc( hWnd, uMsg, wParam, lParam );
    }
}


//-----------------------------------------------------------------------------
void OnClose( HWND hWnd )
{
    //  Remove icon from system tray.
    RemoveTrayIcon( hWnd, ID_TRAYICON );

    PostQuitMessage( 0 );
}


//-----------------------------------------------------------------------------
//  Create and display our little popupmenu when the user right-clicks on the
//  system tray.
BOOL ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem )
{
    HMENU   hPop        = NULL;
    int     i           = 0;
    WORD    cmd;
    POINT   pt;

    if ( g_bModalState )
        return FALSE;

    hPop = CreatePopupMenu();

    if ( ! curpos ) {
        GetCursorPos( &pt );
        curpos = &pt;
    }

    InsertMenu( hPop, i++, MF_BYPOSITION | MF_STRING, ID_ABOUT, "About..." );
    InsertMenu( hPop, i++, MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit" );

    SetMenuDefaultItem( hPop, ID_ABOUT, FALSE );

    SetFocus( hWnd );

    SendMessage( hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0 );

    cmd = TrackPopupMenu( hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON
                            | TPM_RETURNCMD | TPM_NONOTIFY,
                          curpos->x, curpos->y, 0, hWnd, NULL );

    SendMessage( hWnd, WM_COMMAND, cmd, 0 );

    DestroyMenu( hPop );

    return cmd;
}


//-----------------------------------------------------------------------------
BOOL OnCommand( HWND hWnd, WORD wID, HWND hCtl )
{
    if ( g_bModalState )
        return 1;

    //  Have a look at the command and act accordingly
    switch ( wID ) {
        case ID_ABOUT:
            g_bModalState = TRUE;
            MessageBox( hWnd, HELP_ABOUT, THIS_TITLE,
                        MB_ICONINFORMATION | MB_OK );
            g_bModalState = FALSE;
            return 0;

        case ID_EXIT:
            PostMessage( hWnd, WM_CLOSE, 0, 0 );
            return 0;

        default:
            // This happens when the right-click menu is canceled.
            return 0;
    }

}


//-----------------------------------------------------------------------------
//  When the mouse pointer drifts over the tray icon, a "tooltip" will be
//  displayed.  Before that happens, we get notified about the movement, so we
//  get a chance to set the "tooltip" text to be something useful.
void OnTrayIconMouseMove( HWND hWnd )
{
    //  stub
}


//-----------------------------------------------------------------------------
//  Right-click on tray icon displays menu.
void OnTrayIconRBtnUp( HWND hWnd )
{
    /*
    This SetForegroundWindow() and PostMessage( , APPWM_NOP, , ) are
    recommended in some MSDN sample code; apparently there's a bug in most if
    not all versions of Windows: Tray icon menus don't vanish properly when
    cancelled unless you provide special coddling.

    In MSDN, see: "PRB: Menus for Notification Icons Don't Work Correctly",
    Q135788:

        mk:@ivt:kb/Source/win32sdk/q135788.htm

    Example code:

        mk:@ivt:pdref/good/code/graphics/gdi/setdisp/c3447_7lbt.htm

    Both of these pseudo-URL's are from the July 1998 MSDN.  Those geniuses
    have since completely re-broken MSDN at least once, so the pseudo-URL's
    are useless with more recent MSDN's.

    In the April 2000 MSDN, you can search titles for "Menus for
    Notification Icons"; "Don't" in the title has been changed to "Do Not",
    and searching for either complete title doesn't work anyway.  Good old
    MSDN.
    */

    SetForegroundWindow( hWnd );

    ShowPopupMenu( hWnd, NULL, -1 );

    PostMessage( hWnd, APPWM_NOP, 0, 0 );
}


//-----------------------------------------------------------------------------
void OnTrayIconLBtnDblClick( HWND hWnd )
{
    SendMessage( hWnd, WM_COMMAND, ID_ABOUT, 0 );
}


//-----------------------------------------------------------------------------
void OnInitMenuPopup( HWND hWnd, HMENU hPop, UINT uID )
{
    //  stub
}


//-----------------------------------------------------------------------------
void RegisterMainWndClass( HINSTANCE hInstance )
{
    WNDCLASSEX wclx;
    memset( &wclx, 0, sizeof( wclx ) );

    wclx.cbSize         = sizeof( wclx );
    wclx.style          = 0;
    wclx.lpfnWndProc    = &WindowProc;
    wclx.cbClsExtra     = 0;
    wclx.cbWndExtra     = 0;
    wclx.hInstance      = hInstance;
    //wclx.hIcon        = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_TRAYICON ) );
    //wclx.hIconSm      = LoadSmallIcon( hInstance, IDI_TRAYICON );
    wclx.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wclx.hbrBackground  = (HBRUSH)( COLOR_BTNFACE + 1 );    //  COLOR_* + 1 is
                                                            //  special magic.
    wclx.lpszMenuName   = NULL;
    wclx.lpszClassName  = THIS_CLASSNAME;

    RegisterClassEx( &wclx );
}


//-----------------------------------------------------------------------------
void UnregisterMainWndClass( HINSTANCE hInstance )
{
    UnregisterClass( THIS_CLASSNAME, hInstance );
}


//-----------------------------------------------------------------------------
HICON LoadSmallIcon( HINSTANCE hInstance, UINT uID )
{
    return LoadImage( hInstance, MAKEINTRESOURCE( uID ), IMAGE_ICON,
                      16, 16, 0 );
}

