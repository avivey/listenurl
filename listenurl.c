
#include "trayicon.h"

//  Entry point
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show )
{
    HWND    hWnd;
    MSG     msg;
    BOOL    bRet;

    //  Detect previous instance, and bail if there is one.
    if ( FindWindow( THIS_CLASSNAME, THIS_TITLE ) )
        return 0;//SendMessage( hPrev, WM_CLOSE, 0, 0 );

    //  We have to have a window, even though we never show it.  This is
    //  because the tray icon uses window messages to send notifications to
    //  its owner.  Starting with Windows 2000, you can make some kind of
    //  "message target" window that just has a message queue and nothing
    //  much else, but we'll be backwardly compatible here.
    RegisterMainWndClass( hInst );

    hWnd = CreateWindow( THIS_CLASSNAME, THIS_TITLE,
                         0, 0, 0, 100, 100, NULL, NULL, hInst, NULL );

    if ( ! hWnd ) {
        MessageBox( NULL, "Ack! I can't create the window!", THIS_TITLE,
                    MB_ICONERROR | MB_OK | MB_TOPMOST );
        return 1;
    }

    //  Message loop
    while ( TRUE ) {
        bRet = GetMessage( &msg, NULL, 0, 0 );
        if ( bRet == 0 || bRet == -1)
          break;
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    UnregisterMainWndClass( hInst );

    return msg.wParam;
}


