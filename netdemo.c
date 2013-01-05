#include "network.h"
#include <stdio.h>


int do_something(wchar_t* t) {
return MessageBox( NULL, "Ack! I can't create the window!", "eek",
                    MB_ICONERROR | MB_OK | MB_TOPMOST );


  // return printf("Something with %S", t);
}

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show )
{
    printf("hello\n");
    network_start(&do_something);
    printf("hello hello\n");
    return 0;
}
