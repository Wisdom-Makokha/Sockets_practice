/**
 * @file chatting_test_client.cpp
 * @author Wisdom Makokha P15/81777/2017
 * @brief
 * @version 0.1
 * @date 2023-11-15
 *
 * @copyright Copyright (c) 2023
 *
 */

//This is a windows specific implementation of sockets on the client side for a chatting like application
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


//need to link with Ws2_32.lib, Mswsock.lib and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "Advapi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define END_CONN_STR "&$shut$&"
#define END_STRING "$es$"
#define END_STRING_LEN 4

int iResult;
SOCKET connectsocket = INVALID_SOCKET;

//This program has been with the assumption of communication only on the same device
//to run it type the program name then after a space type "localhost" or -nameoflocalcomputer-
int __cdecl main(int argc, char *argv[])
{
    int startupconnection(char *servername);
    int sendtoserver(char *sendbuf);
    int finishsend(void);
    int recvfromserver(char *recvbuf);

    bool stopnow = false;
    bool clientsend = true;

    //check that every arguement needed to start the client application is present when starting the application
    if (argc != 2)
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    printf("\nChat with the server\nTo shutdown this client type: ");
    printf(END_CONN_STR);
    printf("\n\n");

    //the startup function initializes and binds the sockets
    iResult = startupconnection(argv[1]);
    if (iResult != 0)
        return 1;

    //the do while loop uses the created connection to shift between a sending and receiving mode
    //there is no multithreading implemented 
    do
    {
        //client in send mode
        //client begins in send mode
        if (clientsend)
        {
            char sendbuf[DEFAULT_BUFLEN];

            //the sender will be on the right side of terminal when it is their turn 
            //user input is captured to be sent
            printf("\n\t\t\tclnt>> ");
            scanf("%[^\n]%*c", sendbuf);

            //END_CONN_STR is a defined preprocessor that can be typed in the terminal to stop communications
            if (strcmp(sendbuf, END_CONN_STR) == 0)
            {
                finishsend;
                break;
            }
            iResult = sendtoserver(sendbuf);
        }

        //server in receiving mode
        else
        {
            char recvbuf[DEFAULT_BUFLEN];

            iResult = recvfromserver(recvbuf);

            //shutdown message received from server which is used by client to close connection on its side
            if (strcmp(recvbuf, END_CONN_STR) == 0)
            {
                finishsend;
                break;
            }
            printf(recvbuf);
        }

        //communication mode flipped from sending to receiving
        clientsend = !clientsend;

        //error check to stop loop whenever errors are detected
        if (iResult > 0)
            stopnow = true;
    } while (!stopnow);

    closesocket(connectsocket);
    WSACleanup;

    return 0;
}

int startupconnection(char *servername)
{
    WSADATA wsaData;
    struct addrinfo hints,
        *result = NULL,
        *ptr = NULL;

    //Initialise Winsock
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

    //use the server name enterd at the terminal to determine the server address and port
    iResult = getaddrinfo(servername, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    //attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        //create a socket to connect to server
        connectsocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectsocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        //connect to server through created socket
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

    //check socket is valid
    if (connectsocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    return 0;
}

//function to send messages to the server
int sendtoserver(char *sendbuf)
{
    strcat(sendbuf, END_STRING);

    iResult = send(connectsocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectsocket);
        WSACleanup();
        return 1;
    }

    return 0;
}

//function to shutdown connection on client side
int finishsend(void)
{
    iResult = shutdown(connectsocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(connectsocket);
        WSACleanup();
        return 1;
    }

    return 0;
}

//function to receive messages from the client
int recvfromserver(char *recvbuf)
{
    int recvbuflen = DEFAULT_BUFLEN;
    iResult = recv(connectsocket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
        recvbuf[iResult - END_STRING_LEN] = '\0';
        printf("\nsrv>> ");
        return 0;
    }
    else if (iResult == 0)
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());

    return 1;
}

/**
 * @brief This is a test program to see if I can change enough of the winsock programs to allow for chatting
 *
 */