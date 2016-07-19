// FSCloudSynchServer.cpp : Defines the entry point for the console application.
//

// SocketServer.cpp : Defines the entry point for the console application.
// A synchronous server exemplifying use of sockets  

#include "stdafx.h"
#include "aio.h"
#include "../include/FSCloudService.h"

// this is a "black magic" alternative to explicit link with sockets library
#pragma comment (lib, "Ws2_32.lib")

// the  remote  folder
#define ROOT_FS "c:/remotefs"

VOID StartSession(SOCKET s);

int _tmain(int argc, LPCTSTR argv[])
{
	/* Server listening and connected sockets. */

	SOCKET SrvSock = INVALID_SOCKET, connectSock = INVALID_SOCKET;
	int addrLen;
	struct sockaddr_in srvSAddr;		/* Server's Socket address structure */
	struct sockaddr_in connectSAddr;	/* Connected socket with client details   */
	WSADATA WSStartData;				/* Socket library data structure   */
	BOOL terminate = FALSE;


	// sets the current directory to the remote file folder
	SetCurrentDirectoryA(ROOT_FS);
	
	/*	Initialize the WS library. Ver 2.0 */


	if (WSAStartup(MAKEWORD(2, 0), &WSStartData) != 0) {
		_tprintf(_T("Cannot support sockets"));
		return 1;
	}

	/* Create IO Completion Port and associated thread Poll*/
	if (!CreateCompletionPort(MAX_CONCURRENCY)) {
		_tprintf(_T("Error %d creating completion port"), GetLastError());
		WSACleanup();
		return 1;
	}

	/*	Follow the standard server socket/bind/listen/accept sequence */
	SrvSock = socket(AF_INET, SOCK_STREAM, 0); 
	if (SrvSock == INVALID_SOCKET) {
		_tprintf(_T("Failed server socket() call"));
		WSACleanup();
		return 1;
	}

	/*	Prepare the socket address structure for binding the
	server socket to port number "reserved" for this service. */


	srvSAddr.sin_family = AF_INET;
	srvSAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvSAddr.sin_port = htons(SERVER_PORT);

	if (bind(SrvSock, (struct sockaddr *)&srvSAddr, sizeof(srvSAddr)) == SOCKET_ERROR) {
		_tprintf(_T("Failed server bind() call"));
		goto end;
	}

	// define the accept queue length
	if (listen(SrvSock, SOMAXCONN) != 0) {
		_tprintf(_T("Server listen() error"));
		goto end;
	}

	/* Main thread becomes listening/connecting/monitoring thread */
	while (!terminate) {
		addrLen = sizeof(connectSAddr);
		_tprintf(_T("waiting  connection..\n"));
		/* Accept requests from any client machine.  */
		connectSock = accept(SrvSock,
			(struct sockaddr *)&connectSAddr, &addrLen);
		if (connectSock == INVALID_SOCKET) {
			_tprintf(_T("accept: invalid socket error"));
			terminate = TRUE;
			continue;
		}
		printf("Session started, connected with %s, port %d.\n", inet_ntoa(connectSAddr.sin_addr), connectSAddr.sin_port);
		 
		StartSession(connectSock);
	}
end:
	// Cleanup
	shutdown(SrvSock, SD_BOTH); /* Disallow sends and receives */
	closesocket(SrvSock);
	WSACleanup();

	return 0;
}



