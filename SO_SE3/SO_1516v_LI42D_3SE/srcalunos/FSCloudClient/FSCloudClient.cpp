// cloudFSClient.cpp : Defines the entry point for the console application.
//

/*---------------------------
A Client exemplifing use of sockets
------------------------------------*/

#include "stdafx.h"
#include "../include/FSCloudService.h"		
#include "../include/clientutils.h"

// this is a "black magic" alternative to explicit link with sockets library
#pragma comment (lib, "Ws2_32.lib")

// the  downloads destination and uploads source folder
#define CLIENT_FOLDER _T("c:/clientfiles")

#define MAX_CMDS 10

typedef struct _cmd {
	char name;
	char *fileName;
} CMD, *PCMD;

typedef struct clientArgs {
	char *serverIP;
	int nCmds;
	CMD cmds[MAX_CMDS];
} CLIENT_ARGS, *PCLIENT_ARGS;

// communication buffers
static FSCloudRequestHeader request;
static FSCloudAnswer response;


// process application arguments
BOOL processArgs(int argc, char *argv[], PCLIENT_ARGS cargs) {
	int i = 1;
	int nCmds = 0;

	ZeroMemory(cargs, sizeof(CLIENT_ARGS));
	
	while (i < argc) {
		if (strcmp(argv[i], "-s") == 0) {
			if (cargs->serverIP != NULL || i + 1 == argc) return FALSE;
			cargs->serverIP = argv[i + 1];
		}
		else {
			char name = *(argv[i] + 1);
			if (nCmds == MAX_CMDS || argv[i][2] != 0 || (name  != 'l' && name  != 'u' &&
				name  != 'd')) return FALSE;
			cargs->cmds[nCmds].name = name;
			if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "-d") == 0) {
				if (++i == argc) return FALSE;
				cargs->cmds[nCmds].fileName = argv[i];
			}
			else {
				cargs->cmds[nCmds].fileName = "";
			}
			nCmds++;
		}
		++i;
	}
	if (nCmds == 0) return FALSE;
	cargs->nCmds = nCmds;
	return TRUE;
	
}


void showInfoError(int status, PCMD cmd) {
	char *cmdName;
	 
	if (cmd->name == 'u') cmdName = "upload file";
	else if (cmd->name == 'd') cmdName = "download file";
	else cmdName = "list remote files";

	printf("error %d on %s %s\n", status, cmdName, cmd->fileName);

}

// auxiliary function to get server response
BOOL ReadResponse(SOCKET sd, PFSCloudAnswer response)
{
	return readToBuffer(sd, response, sizeof(FSCloudAnswer));
}

// auxiliary function to send client request
BOOL WriteRequest(SOCKET sd, PFSCloudRequestHeader request)
{
	return writeFromBuffer(sd, request, sizeof(FSCloudRequestHeader));
}

BOOL RemoteFilesList(SOCKET sd) {
	printf("list remote files\n");
	strcpy_s(request.cmdName, LIST_FILES);
	
	if (!WriteRequest(sd, &request))  
		return IO_ERROR;
	 
	if (!ReadResponse(sd, &response))
		return IO_ERROR;
	int status = atoi(response.status);
	if (status != STATUS_OK)
		return status;
	int nEntries = atoi(response.dataSize);
	FSCloudFileEntry *entries =
		(PFSCloudFileEntry)malloc(sizeof(FSCloudFileEntry)*nEntries);
	readToBuffer(sd, entries, sizeof(FSCloudFileEntry)*nEntries);
	for (int i = 0; i < nEntries; ++i) {
		printf("%-30s %-10s bytes\n", entries[i].name, entries[i].size);
	}
	free(entries);
	return STATUS_OK;
}

INT GetFile(SOCKET sd, char *fileName) {
	
	/* Send the the request to the server on socket sd */
	printf("downloading %s...\n", fileName);
	strcpy_s(request.fileName, fileName);
	strcpy_s(request.cmdName, GET_FILE);

	if (!WriteRequest(sd, &request))  
		return IO_ERROR;
	 

	if (!ReadResponse(sd, &response))
		return IO_ERROR;
	int status = atoi(response.status);
	if (status != STATUS_OK) 
		return status;
	int fileSize = atoi(response.dataSize);
	return downloadFile(sd, fileName, fileSize);
		 
}

INT PutFile(SOCKET sd, char *fileName)
{
	INT status;
	INT fileSize;

	/* Send the the request to the server on socket sd */
	printf("uploading %s...\n", fileName);
	strcpy_s(request.fileName, fileName);
	if ((fileSize=getFileSize(fileName)) == -1)  
		return IO_ERROR;
	_itoa_s(fileSize, request.fileSize, 10);
	strcpy_s(request.cmdName, PUT_FILE);
 
	if (!WriteRequest(sd, &request))  
		return IO_ERROR;
	 
		
	if ((status = uploadFile(fileName, sd, fileSize)) != STATUS_OK)
		return status;
	 
	if (!ReadResponse(sd, &response))
		return IO_ERROR;
	
	return  atoi(response.status);
	 
}

int processCmd(SOCKET sock, PCMD cmd) {
	int res;
	switch (cmd->name) {
		case 'l':
			res = RemoteFilesList(sock); break;
		case 'u':
			res = PutFile(sock, cmd->fileName); break;
		case 'd':
			res = GetFile(sock, cmd->fileName); break;
		default:
			res= INTERNAL_ERROR;
	}
	return res;
}

int main(int argc, CHAR *argv[])
{
	SOCKET clientSock = INVALID_SOCKET;
	struct sockaddr_in remoteSAddr;		/* Address structure of socket to connect */

	WSADATA WSStartData;				/* Socket library data structure   */
	CLIENT_ARGS cargs;
	

	// check application args
	if (!processArgs(argc, argv, &cargs)) {
		_tprintf(_T("usage: cloudFSC [-s <serverIp>] { -d(ownload) <file> | -u(pload) <file> | -l(ist) }+ \n"));
		return 1;
	}

	// sets the current directory to the local file folder
	SetCurrentDirectory(CLIENT_FOLDER);

	/*	Initialize the WS library. Ver 2.0 */
	if (WSAStartup(MAKEWORD(2, 0), &WSStartData) != 0) {
		_tprintf(_T("Cannot support sockets lib version 2.0"));
		return 1;
	}
 
	/* Connect to the server */
	/* Follow the standard client socket/connect sequence */
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (clientSock == INVALID_SOCKET) {
		_tprintf(_T("Failed client socket() call"));
		WSACleanup();
		return 1;
	}

	memset(&remoteSAddr, 0, sizeof(remoteSAddr));
	remoteSAddr.sin_family = AF_INET;
	if (cargs.serverIP != NULL)
		remoteSAddr.sin_addr.s_addr = inet_addr(argv[1]);
	else
		remoteSAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (remoteSAddr.sin_addr.s_addr == INADDR_NONE) {
		_tprintf(_T("Invalid server IP address!\n"));
	}
	else {
		remoteSAddr.sin_port = htons(SERVER_PORT);

		DWORD conVal = connect(clientSock, (struct sockaddr *)&remoteSAddr,
			sizeof(remoteSAddr));
		if (conVal == SOCKET_ERROR) {
			_tprintf(_T("Failed client connect() call)"));
		}
		else {
			_tprintf(_T("Connection done!\n"));

			LONGLONG start = GetTickCount64();
			BOOL error = FALSE;
			for (int i = 0; i < cargs.nCmds; ++i) {
				int res = processCmd(clientSock, &cargs.cmds[i]);
				if (res != STATUS_OK) {
					error = TRUE;
					showInfoError(res, &cargs.cmds[i]);
				}
					 
			}
			TCHAR *extra = error ? _T("(with errors)") : _T("");
			_tprintf(_T("done %s in %I64ums!\n"), extra, GetTickCount64() - start);
			// Cleanup
			shutdown(clientSock, SD_BOTH); /* Disallow sends and receives */
		}
	}
	closesocket(clientSock);
	WSACleanup();

	return 0;
}




