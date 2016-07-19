#include "stdafx.h"
#include "aio.h"


typedef struct util_ctx {
	LPVOID buffer;
	DWORD cur;
	DWORD size;
	LPVOID userCtx;
	AIO_CALLBACK cb;

} UTIL_CTX, *PUTIL_CTX;

static VOID ReadCCallback(PAIO_DEV aiodev, INT transferedBytes, LPVOID ctx) {
	PUTIL_CTX pCtx=(PUTIL_CTX) ctx;
	if (transferedBytes == 0 || pCtx->cur + transferedBytes == pCtx->size) {
		pCtx->cb(aiodev, pCtx->size, pCtx->userCtx);
		return;
	}
	pCtx->cur+=transferedBytes;
	ReadAsync(aiodev, (CHAR*)pCtx->buffer + pCtx->cur, pCtx->size - pCtx->cur, ReadCCallback,pCtx);
}

BOOL ReadAsync(PAIO_DEV dev, LPVOID buffer, int bsize, AIO_CALLBACK cb, LPVOID ctx) {
	SetAioOper(dev, cb, ctx);

	return AsyncRead(dev->dev, buffer, bsize, &dev->ioStatus);
}

BOOL ReadCompleteAsync(PAIO_DEV dev, LPVOID buffer, int size, AIO_CALLBACK cb, LPVOID ctx) {
	PUTIL_CTX pCtx=(PUTIL_CTX)malloc(sizeof(UTIL_CTX));
	pCtx->buffer = buffer;
	pCtx->cb = cb;
	pCtx->cur = 0;
	pCtx->size = size;
	pCtx->userCtx = ctx;

	return ReadAsync(dev, buffer, size, ReadCCallback, pCtx);
}