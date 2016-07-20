#include "stdafx.h"

#include "aio.h"
#include "../include/FSCloudService.h"
#include "../include/serverutils.h"

typedef struct serviceContext {
	FSCloudRequestHeader req;
	FSCloudAnswer resp;
} SERVICE_CONTEXT, *PSERVICE_CONTEXT;

// create new service context. For now just communication buffers
PSERVICE_CONTEXT NewCtx() {
	return (PSERVICE_CONTEXT)malloc(sizeof(SERVICE_CONTEXT));
}

// terminate session and cleanup session context
static VOID Abort(PAIO_DEV dev, LPVOID ctx) {
	free(ctx);
	CloseAsync(dev);
}


// dispatch commands stuff
typedef INT(*CmdExecuter)(PAIO_DEV sock, PSERVICE_CONTEXT ctx);

INT ExecPut(PAIO_DEV sock, PSERVICE_CONTEXT);
INT ExecGet(PAIO_DEV sock, PSERVICE_CONTEXT);
INT ExecList(PAIO_DEV sock, PSERVICE_CONTEXT);

typedef struct cmdDispatcher {
	PCHAR cmdName;
	CmdExecuter exec;
} CMD_DISPACTHER, *PCMD_DISPATCHER;
 
CMD_DISPACTHER cmds[] = {
	{ PUT_FILE, ExecPut },
	{ GET_FILE, ExecGet },
	{ LIST_FILES, ExecList },
};
DWORD nCmds = sizeof(cmds) / sizeof(cmds[0]);

 
static BOOL dispatchOper(PAIO_DEV sd, PSERVICE_CONTEXT sc) {
	for (DWORD c = 0; c < nCmds; ++c) {
		if (!strcmp(cmds[c].cmdName, sc->req.cmdName))
			return cmds[c].exec(sd, sc) == STATUS_OK;
	}
	return FALSE;
}


/* cmds implementation */

static BOOL writeResponse(HANDLE sd, PFSCloudAnswer response) {
	return writeFromBuffer((SOCKET)sd, response, sizeof(FSCloudAnswer));
}

// upload a file, i.e. put it on remote fs
static INT ExecPut(PAIO_DEV sock, PSERVICE_CONTEXT sc) {
	int fileSize = atoi(sc->req.fileSize);
	
	HANDLE hFile = CreateFileA(sc->req.fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Error %d creating file %s!\n", GetLastError(), sc->req.fileName);
		return FS_ERROR;
	}
	
	int status = uploadFile((SOCKET) sock->dev, hFile, fileSize);
	CloseHandle(hFile);
	
	_itoa_s(status, sc->resp.status, 10);	
	if (!writeResponse(sock->dev, &sc->resp)) return IO_ERROR;
	return STATUS_OK;
}


// download a file, i.e., get it from remote FS
INT ExecGet(PAIO_DEV sock, PSERVICE_CONTEXT sc) {
	int status = STATUS_OK;
	HANDLE hFile= NULL;

	int fileSize = getFileSize(sc->req.fileName);
	if (fileSize == -1) status = FS_ERROR;
	else {
		hFile = CreateFileA(sc->req.fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			printf("Error %d opening file %s!\n", GetLastError(), sc->req.fileName);
			status = FS_ERROR;
		}
	}

	// Start send Response
	_itoa_s(status, sc->resp.status, 10);
	_itoa_s(fileSize, sc->resp.dataSize, 10);
	if (!writeResponse(sock->dev, &sc->resp)) status = IO_ERROR;

	if (status == STATUS_OK)
		status = downloadFile(hFile, (SOCKET) sock->dev, fileSize);
	CloseHandle(hFile);
	
	return status;
}

// List remote files, not implemented
#define MAX_ENTRIES 128
INT ExecList(PAIO_DEV sock, PSERVICE_CONTEXT sc) {
	 
	printf("command not implemented: files list\n");
	
	_itoa_s(INTERNAL_ERROR, sc->resp.status, 10);
	 
	if (!writeResponse(sock->dev, &sc->resp)) 
		return IO_ERROR;
	 
	return STATUS_OK;
}


// callback for process received command
static VOID readCmdCallback(PAIO_DEV aiodev, INT transferedBytes, LPVOID ctx) {
	PSERVICE_CONTEXT sc = (PSERVICE_CONTEXT)ctx;
 
	if (transferedBytes != sizeof(FSCloudRequestHeader)) {
		Abort(aiodev, ctx);
		return;
	}
	if (!dispatchOper(aiodev, sc)) {
		printf("error processing command!\n");
		Abort(aiodev, ctx);
		return;
	}
	//start new read! 
	if (!ReadAsync(aiodev, ctx, sizeof(FSCloudRequestHeader), readCmdCallback, ctx)) {
		_tprintf(_T("Error start receiving request\n"));
		Abort(aiodev, ctx);
	}
}



// session init
VOID StartSession(SOCKET s) {
	/* associate the socket to completion port */
	PAIO_DEV aiod = NewAioSocket(s);
	if (aiod == NULL) return;

	PSERVICE_CONTEXT ctx = NewCtx();
	if (ctx == NULL) {
		CloseAsync(aiod);
		return;
	}

	// start the first command receive
	if (!ReadAsync(aiod, ctx, sizeof(FSCloudRequestHeader), readCmdCallback, ctx)) {
		_tprintf(_T("Error start receiving request\n"));
		Abort(aiod, ctx);
	}
}