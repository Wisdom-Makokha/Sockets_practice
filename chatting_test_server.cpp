/**
 * @file chatting_test_server.cpp
 * @author Wisdom Makokha P15/81777/2017
 * @brief
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023
 *
 */

//This is a windows specific implementation of sockets on the server side for a chatting like application 
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
#define END_CONN_STR "&$shut$&"
#define END_STRING "$es$"
#define END_STRING_LEN 4

int iResult;
SOCKET clientsocket = INVALID_SOCKET;
SOCKET listensocket = INVALID_SOCKET;

int __cdecl main(void)
{
    int recvfromclient(char *recvbuf);
    int finishsend(void);
    int sendtoclient(char *sendbuf);
    int startupconnection(void);

    bool serversend = false;
    bool stopnow = false;

    printf("\nChat with the client\nTo shutdown this client type: ");
    printf(END_CONN_STR);
    printf("\n\n");

    //sockets are initialized and bound with the startup function
    iResult = startupconnection();
    if (iResult != 0)
        return 1;

    //the do while loop uses the created connection to shift between a sending and receiving mode
    //there is no multithreading implemented 
    do
    {
        //server in send mode
        if (serversend)
        {
            char sendbuf[DEFAULT_BUFLEN];

            //the sender will be on the right side of terminal when it is their turn 
            //user input is captured to be sent
            printf("\n\t\t\tsrv>> ");
            scanf("%[^\n]%*c", sendbuf);

            //END_CONN_STR is a defined preprocessor that can be typed in the terminal to stop communications
            if (strcmp(sendbuf, END_CONN_STR) == 0)
            {
                finishsend;
                break;
            }
            iResult = sendtoclient(sendbuf);
        }
        
        //server in receiving mode
        //the server begins in reeiving mode
        else
        {
            char recvbuf[DEFAULT_BUFLEN];

            iResult = recvfromclient(recvbuf);

            //shutdown message received from client which is used by server to close connection on its side
            if (strcmp(recvbuf, END_CONN_STR) == 0)
            {
                finishsend;
                break;
            }
            printf(recvbuf);
        }

        //communication mode flipped from sending to receiving
        serversend = !serversend;

        //error check to stop loop whenever errors are detected
        if (iResult > 0)
            stopnow = true;
    } while (!stopnow);

    closesocket(clientsocket);
    WSACleanup;

    return 0;
}

int startupconnection(void)
{
    WSADATA wsaData;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;

    //initialize Winsock
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

    //pick an address and port for the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("gettaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    //create listening socket that will wait for connection requests
    listensocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listensocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //setup TCP listening socket
    iResult = bind(listensocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listensocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    //now listening for client
    iResult = listen(listensocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listensocket);
        WSACleanup();
        return 1;
    }
    
    //Accept client connection
    clientsocket = accept(listensocket, NULL, NULL);
    if (clientsocket == INVALID_SOCKET)
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listensocket);
        WSACleanup();
        return 1;
    }

    //done listening for connections so close socket
    closesocket(listensocket);

    return 0;
}

//function to send messages to client
int sendtoclient(char *sendbuf)
{
    strcat(sendbuf, END_STRING);

    iResult = send(clientsocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError);
        closesocket(clientsocket);
        WSACleanup;
        return 1;
    }

    return 0;
}

//function to shutdown connection on server side
int finishsend(void)
{
    iResult = shutdown(clientsocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError);
        closesocket(clientsocket);
        WSACleanup;
        return 1;
    }

    return 0;
}

//function to receive messages from the client
int recvfromclient(char *recvbuf)
{
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = recv(clientsocket, recvbuf, recvbuflen, 0);

    if (iResult > 0)
    {
        recvbuf[iResult - END_STRING_LEN] = '\0';
        printf("clnt>> ");
        return 0;
    }
    else if (iResult == 0)
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError);

    return 1;
}

/**
 * @brief This is a test program to see if I can change enough of the Winsock programs to allow for chatting
 * 
 */