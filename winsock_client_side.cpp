/**
 * @file winsock_client_side.cpp
 * @author Wisdom Makokha
 * @brief
 * @version 0.1
 * @date 2023-11-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// need to link with Ws2_32.lib, Mswsock.lib and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "Advapi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET connectsocket = INVALID_SOCKET; //
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    const char *sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // validate the parameters
    if (argc != 2)
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }
    // printf("Client obtained server name.\n");

    // initialize winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    // printf("Server address resolved\nConnecting to server...\n");

    // attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // create a socket for connecting to server
        connectsocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectsocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // connect to server
        iResult = connect(connectsocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(connectsocket);
            connectsocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (connectsocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    // printf("Connection to server established\n");

    // send an initial buffer
    iResult = send(connectsocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectsocket);
        WSACleanup();
        return 1;
    }
    // printf("Message sent to the server\n");

    printf("Bytes sent: %ld\n", iResult);

    // shutdown connection since no more data will be sent
    iResult = shutdown(connectsocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(connectsocket);
        WSACleanup();
        return 1;
    }
    // printf("Client shutdown connection initiation after finish sending\n");

    // receive until the peer closes the connection
    do
    {
        iResult = recv(connectsocket, recvbuf, recvbuflen, 0);
        printf(recvbuf);
        printf("\n");

        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
    } while (iResult > 0);
    // printf("Client receiving until the server finishes\n");

    // cleanup
    closesocket(connectsocket);
    WSACleanup();
    // printf("Closing client socket\n");

    return 0;
}