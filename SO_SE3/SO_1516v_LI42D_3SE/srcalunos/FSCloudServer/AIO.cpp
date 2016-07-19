#include "stdafx.h"
#include "aio.h"

typedef struct util_ctx {
	LPVOID buffer;
	DWORD cur;
	DWORD size;
	LPVOID userCtx;
	AIO_CALLBACK cb;
	PAIO_DEV devToCall;

} UTIL_CTX, *PUTIL_CTX;

static VOID CpyCallback2(PAIO_DEV aiodev, INT transferedBytes, LPVOID ctx) {
	PUTIL_CTX pCtx = (PUTIL_CTX)ctx;
	pCtx->cb(aiodev, pCtx->size, pCtx->userCtx);
}

static VOID CpyCallback1(PAIO_DEV aiodev, INT transferedBytes, LPVOID ctx) {
	PUTIL_CTX pCtx = (PUTIL_CTX)ctx;
	WriteCompleteAsync(pCtx->devToCall,pCtx->buffer,transferedBytes, CpyCallback2,ctx);
}

static VOID ReadCCallback(PAIO_DEV aiodev, INT transferedBytes, LPVOID ctx) {
	PUTIL_CTX pCtx = (PUTIL_CTX)ctx;
	if (transferedBytes == 0 || pCtx->cur + transferedBytes == pCtx->size) {
		pCtx->cb(aiodev, pCtx->size, pCtx->userCtx);
		return;
	}
	pCtx->cur += transferedBytes;
	ReadAsync(aiodev, (CHAR*)pCtx->buffer + pCtx->cur, pCtx->size - pCtx->cur, ReadCCallback, pCtx);
}

static VOID WriteCCallback(PAIO_DEV aiodev, INT transferedBytes, LPVOID ctx) {
	PUTIL_CTX pCtx = (PUTIL_CTX)ctx;
	if (transferedBytes == 0 || pCtx->cur + transferedBytes == pCtx->size) {
		pCtx->cb(aiodev, pCtx->size, pCtx->userCtx);
		return;
	}
	pCtx->cur += transferedBytes;
	WriteAsync(aiodev, (CHAR*)pCtx->buffer + pCtx->cur, pCtx->size - pCtx->cur, WriteCCallback, pCtx);
}

BOOL WriteCompleteAsync(PAIO_DEV dev, LPVOID buffer, int bsize, AIO_CALLBACK cb, LPVOID ctx) {
	PUTIL_CTX pCtx = (PUTIL_CTX)malloc(sizeof(UTIL_CTX));
	pCtx->buffer = buffer;
	pCtx->cb = cb;
	pCtx->cur = 0;
	pCtx->size = bsize;
	pCtx->userCtx = ctx;

	return WriteAsync(dev, buffer, bsize, ReadCCallback, pCtx);
}

BOOL ReadCompleteAsync(PAIO_DEV dev, LPVOID buffer, int size, AIO_CALLBACK cb, LPVOID ctx) {
	PUTIL_CTX pCtx = (PUTIL_CTX)malloc(sizeof(UTIL_CTX));
	pCtx->buffer = buffer;
	pCtx->cb = cb;
	pCtx->cur = 0;
	pCtx->size = size;
	pCtx->userCtx = ctx;

	return ReadAsync(dev, buffer, size, ReadCCallback, pCtx);
}

BOOL CopyStreamAsync(PAIO_DEV devIn, PAIO_DEV devOut, int size, AIO_CALLBACK cb, LPVOID ctx) {

	char* buffer= (char *) malloc(size);
	PUTIL_CTX pCtx = (PUTIL_CTX)malloc(sizeof(UTIL_CTX));
	pCtx->buffer = buffer;
	pCtx->cb = cb;
	pCtx->cur = 0;
	pCtx->size = size;
	pCtx->userCtx = ctx;
	pCtx->devToCall =devOut;
	
	return ReadCompleteAsync(devIn,buffer,size, CpyCallback1,pCtx);
	

}

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