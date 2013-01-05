#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <windows.h>  // for wchar_t

void network_start();

// Returns 0 for success, non-zero for errors.
int network_loop(int (*handle_message)(wchar_t *) );
void network_stop();

#endif
