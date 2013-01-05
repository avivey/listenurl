#include "network.h"
#include <stdio.h>


int do_something(wchar_t* t) {
  return printf("Something with %S", t);
  return MessageBox( NULL, "Ack! I can't create the window!", "eek",
                    MB_ICONERROR | MB_OK | MB_TOPMOST );
}

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show )
{
    printf("hello\n");
    network_start();

    int status;
    do {
      status = network_loop(&do_something);
    } while( status == 0);

    printf("hello hello\n");
    network_stop();
    return 0;
}
