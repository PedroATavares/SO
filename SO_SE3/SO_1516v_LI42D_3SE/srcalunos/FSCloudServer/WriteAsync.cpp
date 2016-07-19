#include "stdafx.h"
#include "aio.h"

typedef struct write_ctx {
	LPVOID buffer;
	DWORD cur;
	DWORD size;
	LPVOID userCtx;
	AIO_CALLBACK cb;

} WRITE_CTX, *PWRITE_CTX;

BOOL WriteAsync(PAIO_DEV dev, LPVOID buffer, int bsize, AIO_CALLBACK cb, LPVOID ctx) {
	SetAioOper(dev, cb, ctx);

	return AsyncWrite(dev->dev, buffer, bsize, &dev->ioStatus);
}
