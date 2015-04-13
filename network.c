// network.c
// all network code is here.

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "network.h"

#define PORT "6663"
#define BUFLEN 2048

static SOCKET ListenSocket = INVALID_SOCKET;
static BOOL inited = FALSE;

void network_start( HWND hWnd, UINT messageCode )
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
  }


  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  if (ListenSocket == INVALID_SOCKET) {
      printf("Error at socket(): %d\n", WSAGetLastError());
      freeaddrinfo(result);

      return;
  }

  iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  freeaddrinfo(result);
  if (iResult == SOCKET_ERROR) {
      printf("bind failed with error: %d\n", WSAGetLastError());
      closesocket(ListenSocket);

      return;
  }

  iResult = WSAAsyncSelect( ListenSocket, hWnd,messageCode, FD_ACCEPT | FD_READ);

  if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
    printf( "Listen failed with error: %d\n", WSAGetLastError() );
    closesocket(ListenSocket);

    return;
  }
}

int network_accept()
{
  SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
      printf("accept failed: %d\n", WSAGetLastError());
      closesocket(ListenSocket);
      return -2;
  }

  return 0;
}

int network_recv_invoke(WPARAM socket, HINSTANCE (*handle_message)(LPCTSTR) )
{
  int iResult;
  char recvbuf[BUFLEN];
  wchar_t textbuf[BUFLEN];

  do {
      iResult = recv(socket, recvbuf, BUFLEN, 0);
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
          closesocket(socket);
          socket = INVALID_SOCKET;
          return -3;
      }

  } while (iResult > 0);

  iResult = shutdown(socket, SD_SEND);
  closesocket(socket);

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

