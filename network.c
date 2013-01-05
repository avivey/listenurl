// network.c
// all network code is here.

#include <winsock2.h>
#include <ws2tcpip.h>
// #include <iphlpapi.h>
#include <stdio.h>
#include "network.h"

#include "port-def.h"
#define BUFLEN 2048

static SOCKET ListenSocket = INVALID_SOCKET;
static BOOL inited = FALSE;

void network_start()
{
  if ( inited )
    return;
  inited = TRUE;
  printf("initing!!\n");

  WSADATA wsaData;
  struct addrinfo *result = NULL, hints;
  int iResult;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
      printf("WSAStartup failed: %d\n", iResult);
  //    return 1;
    return;
  }

  ZeroMemory(&hints, sizeof (hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the local address and port to be used by the server
  iResult = getaddrinfo("localhost", PORT, &hints, &result);
  if (iResult != 0) {
      printf("getaddrinfo failed: %d\n", iResult);

      return;
      // return 1;
  }


  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  if (ListenSocket == INVALID_SOCKET) {
      printf("Error at socket(): %d\n", WSAGetLastError());
      freeaddrinfo(result);

      // return 1;
      return;
  }

  iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  freeaddrinfo(result);
  if (iResult == SOCKET_ERROR) {
      printf("bind failed with error: %d\n", WSAGetLastError());
      closesocket(ListenSocket);

      // return 1;
      return;
  }

  if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
    printf( "Listen failed with error: %d\n", WSAGetLastError() );
    closesocket(ListenSocket);

    return;
  }
}

int network_loop(int (*handle_message)(wchar_t*))
{
  network_start();
  if ( ListenSocket == INVALID_SOCKET )
    return -1;
  int iResult;

  SOCKET ClientSocket = INVALID_SOCKET;

  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
      printf("accept failed: %d\n", WSAGetLastError());
      closesocket(ListenSocket);
      return -2;
  }

  char recvbuf[BUFLEN];
  wchar_t textbuf[BUFLEN];

  do {
      iResult = recv(ClientSocket, recvbuf, BUFLEN, 0);
      if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);
        recvbuf[iResult] = 0;

        iResult = MultiByteToWideChar(CP_UTF8, 0,
                            recvbuf, -1,
                            textbuf, BUFLEN);
        printf("converted %d chars\n", iResult);
        printf ("%S\n", textbuf);

        if ( handle_message ) {
          (*handle_message)(textbuf);
        }

      } else if (iResult == 0)
          printf("Connection closing...\n");
      else {
          printf("recv failed: %d\n", WSAGetLastError());
          closesocket(ClientSocket);
          return -3;
      }

  } while (iResult > 0);

  iResult = shutdown(ClientSocket, SD_SEND);
  closesocket(ClientSocket);

  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed: %d\n", WSAGetLastError());
    return -4;
  }

  return 0;
}

void network_stop()
{
  WSACleanup();
}
