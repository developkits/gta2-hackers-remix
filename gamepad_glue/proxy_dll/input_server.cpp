/*
	This file is heavily copypasted from this tcp server example:
	http://msdn.microsoft.com/en-us/library/windows/desktop/ms737593%28v=vs.85%29.aspx

	Basically we listen on ports 19990-19999 (depending on which one works, think
	of multiple GTA2 instances!), until a connection is established. As soon, as we
	get data, we stuff 2 bytes at a time to the address, where the current movement
	is stored (overwriting the previous two).
	
	When a connection goes down, this program tries the next port (this is useful
	when we want to change the sdl_client code: no gta2 restart required!)
	
	All of this works surprisingly well :)
*/

#undef UNICODE
#define WIN32_LEAN_AND_MEAN // what the hell?

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Speed up the build process
// http://stackoverflow.com/a/11040290
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 2
#define GAMEPADGLUE_START_PORT 19990


// Returns NULL (listen or connection fails!)
// or the client socket, if the connection was successful.
SOCKET input_server_listen(int port)
{
	int iResult;
	SOCKET ListenSocket = INVALID_SOCKET;

	// TODO: find better names for these two!
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
	char port_string[10];
	_itoa(port,port_string,10);

    iResult = getaddrinfo(NULL, port_string, &hints, &result);

    // Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) return NULL; // TODO: display an error here

    // Setup the TCP listening socket
	// If it fails, the outer loop will try the next port.
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
	
	if (iResult == SOCKET_ERROR)
	{
        closesocket(ListenSocket);
		return NULL;
    }

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
	{
        closesocket(ListenSocket);
        WSACleanup();
        return NULL;
    }

    // Accept a client socket
	// TODO: perform some sort of simple handshake here
	SOCKET ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
		return NULL;

    // No longer need server socket
    closesocket(ListenSocket);

	return ClientSocket;
}

// returns 1 if receiving still works
// returns 0 if the connection is dead
int input_server_receive(SOCKET ClientSocket)
{
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

	// return 0 on disconnect or error
	if(recv(ClientSocket, recvbuf, recvbuflen, 0) <= 0)
		return 0;
	
	// put the two bytes to the position in memory,
	// where the current player movement is stored
	// address reverse engineered by robotanarchy
	memcpy((void*)0x5ecacc, recvbuf, 2);

	return 1; // everything's fine
}


void __cdecl input_server(void* param)
{
	// Initialize WinSock
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2,2), &wsa_data);

	while(1)
	{
		// try ports from 19990 to 19999
		for(int i=0;i<10;i++)
		{
			SOCKET ClientSocket = input_server_listen(GAMEPADGLUE_START_PORT + i);
			if(ClientSocket == NULL) continue;

			while(input_server_receive(ClientSocket));
		}

		Sleep(100); // Wait 100ms
	}
	WSACleanup();
}

