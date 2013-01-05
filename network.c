// network.c
// all network code is here.

#include <winsock2.h>
#include <ws2tcpip.h>
// #include <iphlpapi.h>
#include <stdio.h>
#include "network.h"

#include "port-def.h"
#define BUFLEN 2048

void network_start(int (*target_function)(wchar_t*) )
{
  WSADATA wsaData;
  // struct addrinfo *ptr = NULL;
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
      WSACleanup();
      return;
      // return 1;
  }


  SOCKET ListenSocket = INVALID_SOCKET;
  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  if (ListenSocket == INVALID_SOCKET) {
      printf("Error at socket(): %d\n", WSAGetLastError());
      freeaddrinfo(result);
      WSACleanup();
      // return 1;
      return;
  }

  // Setup the TCP listening socket
  iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  freeaddrinfo(result);
  if (iResult == SOCKET_ERROR) {
      printf("bind failed with error: %d\n", WSAGetLastError());
      closesocket(ListenSocket);
      WSACleanup();
      // return 1;
      return;
  }

  if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
    printf( "Listen failed with error: %d\n", WSAGetLastError() );
    closesocket(ListenSocket);
    WSACleanup();
    return;
  }


  // CUT HERE - anything from here down should be handled in the bg.
  //////////////////////////////////////////////


  SOCKET ClientSocket = INVALID_SOCKET;

  ClientSocket = accept(ListenSocket, NULL, NULL);
  if (ClientSocket == INVALID_SOCKET) {
      printf("accept failed: %d\n", WSAGetLastError());
      closesocket(ListenSocket);
      WSACleanup();
      return;
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

        if ( target_function ) {
          (*target_function)(textbuf);
        }

      } else if (iResult == 0)
          printf("Connection closing...\n");
      else {
          printf("recv failed: %d\n", WSAGetLastError());
          closesocket(ClientSocket);
          WSACleanup();
          return;
      }

  } while (iResult > 0);

  // shutdown the send half of the connection since no more data will be sent
  iResult = shutdown(ClientSocket, SD_SEND);
  closesocket(ClientSocket);
  WSACleanup();

  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed: %d\n", WSAGetLastError());
    return;
  }

  return network_start(target_function);
}
void network_stop()
{
}

