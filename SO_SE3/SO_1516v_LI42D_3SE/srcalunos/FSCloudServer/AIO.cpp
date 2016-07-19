#include "stdafx.h"
#include "aio.h"


// cria e inicia um novo async device  
static PAIO_DEV NewAioDev(HANDLE dev, BOOL isSocket) {
	PAIO_DEV aio  = (PAIO_DEV) malloc(sizeof(AIO_DEV));
	if (aio == NULL) return NULL;
	aio->dev = dev; 
	aio->isSocket = isSocket;
	ZeroMemory(&aio->ioStatus, sizeof(OVERLAPPED));
	if (!CompletionPortAssociateHandle(aio->dev, aio)) {
		free(aio);
		return NULL;
	}
	return aio;
}

PAIO_DEV NewAioSocket(SOCKET s) {
	return NewAioDev((HANDLE)s, TRUE);
}

PAIO_DEV OpenFileAsync(LPCSTR fileName) {
	HANDLE h = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (h == INVALID_HANDLE_VALUE) return NULL;
	PAIO_DEV aiod = NewAioDev(h, FALSE);
	if (aiod == NULL) {
		CloseHandle(h);
		return NULL;
	}
	 
	return aiod;
}

PAIO_DEV CreateFileAsync(LPCSTR fileName) {
	HANDLE h = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	if (h == INVALID_HANDLE_VALUE) return NULL;
	PAIO_DEV aiod = NewAioDev(h, FALSE);
	if (aiod == NULL) {
		CloseHandle(h);
		return NULL;
	}

	return aiod;
}


VOID CloseAsync(PAIO_DEV ad) {
	printf("End session!\n");
	if (ad->isSocket) {
		shutdown((SOCKET)ad->dev, SD_BOTH);
		closesocket((SOCKET)ad->dev);
	}
	else {
		CloseHandle(ad->dev);
	}
	free(ad);
}
 
VOID SetAioOper(PAIO_DEV dev, AIO_CALLBACK cb, LPVOID uCtx) {
	dev->oper.callback = cb;
	dev->oper.ctx = uCtx;
}


// adjust current position for seekable devices
VOID ProcessTransfer(PAIO_DEV dev, DWORD transferedBytes) {
	if (!dev->isSocket && transferedBytes > 0) {
		LARGE_INTEGER pos;
		LPOVERLAPPED ovr = &dev->ioStatus;
		pos.HighPart = ovr->OffsetHigh; pos.LowPart = ovr->Offset;
		pos.QuadPart += transferedBytes;
		ovr->OffsetHigh = pos.HighPart; ovr->Offset = pos.LowPart;
	}
}