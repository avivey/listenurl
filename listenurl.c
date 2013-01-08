#include "trayicon.h"
#include "network.h"
#include "shellapi.h"

#define SOCKET_MESSAGE (WM_APP + 100)

void on_close_listener( HWND );
LRESULT app_window_proc( HWND, UINT, WPARAM, LPARAM );

//  Entry point
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show )
{
  HWND    hWnd;
  MSG     msg;
  BOOL    bRet;

  //  Detect previous instance, and bail if there is one.
  if ( FindWindow( THIS_CLASSNAME, THIS_TITLE ) )
      return 0;

  //  We have to have a window, even though we never show it.  This is
  //  because the tray icon uses window messages to send notifications to
  //  its owner.  Starting with Windows 2000, you can make some kind of
  //  "message target" window that just has a message queue and nothing
  //  much else, but we'll be backwardly compatible here.
  RegisterApplicationClass( hInst );

  hWnd = CreateWindow( THIS_CLASSNAME, THIS_TITLE,
                       0, 0, 0, 100, 100, NULL, NULL, hInst, NULL );

  if ( ! hWnd ) {
      MessageBox( NULL, _T("Ack! I can't create the window!"), THIS_TITLE,
                  MB_ICONERROR | MB_OK | MB_TOPMOST );
      return 1;
  }

  app_close_listener = &on_close_listener;
  WindowProc_fallback = &app_window_proc;

  network_start( hWnd, SOCKET_MESSAGE );

  //  Message loop
  while ( TRUE ) {
      bRet = GetMessage( &msg, NULL, 0, 0 );
      if ( bRet == 0 || bRet == -1)
        break;
      TranslateMessage( &msg );
      DispatchMessage( &msg );
  }

  UnregisterClass( THIS_CLASSNAME, hInst );

  return msg.wParam;
}

void on_close_listener( HWND hWnd )
{
  network_stop();
}

int do_something(LPCTSTR t) {
  // return MessageBox( NULL, "got msg", "eek",
                    // MB_ICONERROR | MB_OK | MB_TOPMOST );
}

LRESULT app_window_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  switch ( uMsg )
  {
    case SOCKET_MESSAGE:
			switch(WSAGETSELECTEVENT(lParam))
			{
        case FD_ACCEPT:
          network_accept();
          return 0;

        case FD_READ:
          network_recv_invoke(wParam, &do_something);
          return 0;
      }
      MessageBox( NULL, _T("This aint right..."), THIS_TITLE,
                  MB_ICONERROR | MB_OK | MB_TOPMOST );
      return -1;

    default:
      return DefWindowProc( hWnd, uMsg, wParam, lParam );
  }
}
