#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(void)
{
    WSADATA wsaData; 
    int iResult;

    SOCKET listensocket = INVALID_SOCKET;
    SOCKET clientsocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialise Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return -1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    // printf("Server address resolved\n");

    // create a socket for the server to listen to client connection
    listensocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listensocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    // printf("Listen socket created\n");

    // setup the TCP listening socket
    iResult = bind(listensocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listensocket);
        WSACleanup();
        return 1;
    }
    // printf("Listen socket bound\n");

    freeaddrinfo(result);

    // listen for a client
    iResult = listen(listensocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listensocket);
        WSACleanup();
        return 1;
    }
    // printf("Server now listening...\n");

    // Accept client socket
    clientsocket = accept(listensocket, NULL, NULL);
    if (clientsocket == INVALID_SOCKET)
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listensocket);
        WSACleanup();
        return 1;
    }
    // printf("Client connection accepted\n");

    // no longer need server listen socket
    closesocket(listensocket);

    // receive until the peer shuts down the connection
    do
    {
        iResult = recv(clientsocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            printf("Bytes received: %d\n", iResult);
            printf(recvbuf);
            printf("\n");

            // echo the buffer back to the sender
            iSendResult = send(clientsocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(clientsocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError);
            closesocket(clientsocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);
    // printf("Receive message from client and echo it back: %s\n", recvbuf);

    // shutdown the connection since we are done
    iResult = shutdown(clientsocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(clientsocket);
        WSACleanup();
        return 1;
    }
    // printf("Shutdown socket connection\n");

    // cleanup
    closesocket(clientsocket);
    WSACleanup();
    // printf("Close socket and end program\n");

    return 0;
}