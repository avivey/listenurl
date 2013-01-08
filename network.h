#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <windows.h>  // for wchar_t, HWND...

void network_start( HWND hWnd, UINT messageCode );
int network_accept();
int network_recv_invoke(WPARAM socket, int (*handle_message)(wchar_t *) );
void network_stop();

#endif
