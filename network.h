#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <windows.h>  // for wchar_t

void network_start(int (*target_function)(wchar_t *) );
void network_stop();

#endif
